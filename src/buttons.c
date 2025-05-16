#include "modding.h"
#include "global.h"
#include "libc64/sprintf.h"
#include "overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"

#define INCBIN(identifier, filename)          \
    asm(".pushsection .rodata\n"              \
        "\t.globl " #identifier "\n"          \
        "\t.type " #identifier ", @object\n"  \
        "\t.balign 8\n"                       \
        #identifier ":\n"                     \
        "\t.incbin \"" filename "\"\n\n"      \
                                              \
        "\t.balign 8\n"                       \
        "\t.popsection\n");                   \
    extern u8 identifier[]

RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));

// Function by Wiseguy.
Gfx* Mod_FindDLCommands(Gfx* search_start, Gfx* search_end, Gfx* to_find, u32 num_commands) {
    Gfx* cur_cmd = search_start;
    for (Gfx* cur_cmd = search_start; cur_cmd != search_end; cur_cmd++) {
        // Check if the current command is identical to the first expected command.
        if (cur_cmd->words.w0 == to_find[0].words.w0 && cur_cmd->words.w1 == to_find[0].words.w1) {
            // Check the remaining commands.
            bool matched = true;
            for (u32 i = 1; i < num_commands; i++) {
                if (cur_cmd[i].words.w0 != to_find[i].words.w0 || cur_cmd[i].words.w1 != to_find[i].words.w1) {
                    matched = false;
                    break;
                }
            }
            if (matched) {
                return cur_cmd;
            }
        }
    }

    return NULL;
}

// Function is used to determine what the correct glyph texture is.
extern TexturePtr gButtonBackgroundTex[];

INCBIN(tBGlyph, "./textures/ia8/b_empty_icon.ia8.bin");
INCBIN(tAGlyph, "./textures/ia8/a_empty_icon.ia8.bin");

TexturePtr Mod_GlyphTexture(char button[]) {
    s32 config = recomp_get_config_u32(button);

    // If glyphs aren't fully disabled.
    if (recomp_get_config_u32("glyphs") != 0) {
        TexturePtr tUpTexture;
        TexturePtr tLeftTexture;
        TexturePtr tDownTexture;
        TexturePtr tRightTexture;
    
        switch (config) {
            case 0: // Up
                return (u8*)gButtonBackgroundTex + ((32 * 32) * (EQUIP_SLOT_C_DOWN + 1));
                break;
            case 1: // Left
                return (u8*)gButtonBackgroundTex + ((32 * 32) * (EQUIP_SLOT_C_LEFT + 1));
                break;
            case 2: // Down
                return tBGlyph;
                break;
            case 3: // Right
                return tAGlyph;
                break;
            case 4: // Shoulder
                return (u8*)gButtonBackgroundTex + ((32 * 32) * (EQUIP_SLOT_C_RIGHT + 1));
                break;
        }
    }

    return NULL;
}

PlayState* bPlayState;
u16 bPauseState;

u8 bCLeftItem;
u8 bCDownItem;
u8 bCRightItem;

RECOMP_HOOK("Interface_DrawItemButtons") void Interface_DrawItemButtons_Init(PlayState* play) {
    bPlayState = play;

    // Don't draw C-button glyphs by tricking conditions.
    bCLeftItem = GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_LEFT);
    bCDownItem = GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_DOWN);
    bCRightItem = GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_RIGHT);

    SET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_LEFT, ITEM_F0);
    SET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_DOWN, ITEM_F0);
    SET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_RIGHT, ITEM_F0);
}

Gfx* bGfx = NULL;

RECOMP_HOOK("Gfx_DrawTexRectIA8_DropShadow") void Gfx_DrawTexRectIA8_DropShadow_Init(Gfx* gfx, TexturePtr texture, s16 textureWidth, s16 textureHeight,
    s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b, s16 a) {
    // If being run for the Attack button.
    if (texture == gButtonBackgroundTex && rectWidth == 0x1D) {
        // Trick conditions to disable Pause button while also keeping C-Up disabled when paused. Do it after D-pad has stopped being drawn.
        bPauseState = (&bPlayState->pauseCtx)->state;
        if (bPauseState != PAUSE_STATE_OFF) {
            (&bPlayState->pauseCtx)->state = PAUSE_STATE_GAMEOVER_0;
        }
        // Save Gfx for use in fixing bug.
        bGfx = gfx;
    }
}

RECOMP_HOOK_RETURN("Gfx_DrawTexRectIA8_DropShadow") void Gfx_DrawTexRectIA8_DropShadow_Return() {
    // Fixes a vanilla bug that becomes more noticable with this mod due to how it disables the Start button.
    // The bug causes the ammo count to be green under certain conditions before gaining the magic meter at the start of the game.
    // This fix ensures that code in the magic meter function that amends this bug is present all the time.
    if (bGfx != NULL){
        gDPSetEnvColor(bGfx, 0, 0, 0, 255);
        bGfx = NULL;
    }
}

RECOMP_HOOK("Gfx_DrawRect_DropShadow") void Gfx_DrawRect_DropShadow_Init(Gfx* gfx, s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b, s16 a) {
    // If drawing C-Up button.
    if (rectWidth == 0x10 && rectHeight == 0x10 && r == 0xFF && g == 0xF0 && b == 0) {
        bGfx = gfx;
    }
}

// C-Up variables.
extern TexturePtr gTatlCUpENGTex[];
extern TexturePtr gTatlCUpGERTex[];
extern TexturePtr gTatlCUpFRATex[];
extern TexturePtr gTatlCUpESPTex[];
extern Gfx* Gfx_DrawRect_DropShadow(Gfx* gfx, s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b, s16 a);

extern Gfx to_compare[3]; // Texrects are 3 commands.

// Attack button variables.
extern s16 sBCButtonXPositions[4];
extern s16 sBCButtonYPositions[4];
extern s16 sBCButtonScales[4];

extern TexturePtr Mod_GlyphTexture(char button[]);

RECOMP_HOOK_RETURN("Interface_DrawItemButtons") void Interface_DrawItemButtons_Return() {
    // Reset pause state.
    (&bPlayState->pauseCtx)->state = bPauseState;

    // Set items back.
    SET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_LEFT, bCLeftItem);
    SET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_DOWN, bCDownItem);
    SET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_RIGHT, bCRightItem);

    OPEN_DISPS(bPlayState->state.gfxCtx);

    // Move C-Up.
    if (bGfx != NULL) {
        static TexturePtr cUpLabelTextures[] = {
            gTatlCUpENGTex, gTatlCUpENGTex, gTatlCUpGERTex, gTatlCUpFRATex, gTatlCUpESPTex,
        };
        s16 temp;

        // +++++++ code by Wiseguy +++++++

        // Search from the start of the C-Up displaylist commands to find the texrect command that drew the button itself.
        // Limit searching until reaching the current OVERLAY_DISP value.
        Gfx* c_up_texrect = Mod_FindDLCommands(bGfx, OVERLAY_DISP, to_compare, ARRAY_COUNT(to_compare));
        if (c_up_texrect != NULL) {
            // Allocate a buffer to hold the new commands. Should only need 26 commands, but allocate 32 just to be safe.
            Gfx* replacement_dl = GRAPH_ALLOC(bPlayState->state.gfxCtx, sizeof(Gfx) * 32);

            // Overwrite the beginning of the C-Up drop shadow graphics commands with a branch to the new displaylist.
            gSPBranchList(bGfx, replacement_dl);

            // Draw correct C-Up on new displaylist.
            
            // C-Up Button Texture, Color & Label (Tatl Text)
            gDPPipeSync(replacement_dl++);

            if ((gSaveContext.hudVisibility == HUD_VISIBILITY_NONE) ||
                (gSaveContext.hudVisibility == HUD_VISIBILITY_NONE_ALT) ||
                (gSaveContext.hudVisibility == HUD_VISIBILITY_A_HEARTS_MAGIC_WITH_OVERWRITE) ||
                ((&bPlayState->msgCtx)->msgMode != MSGMODE_NONE)) {
                temp = 0;
            }
            else if (GET_PLAYER(bPlayState)->stateFlags1 & PLAYER_STATE1_200000) {
                temp = 70;
            }
            else {
                temp = (&bPlayState->interfaceCtx)->aAlpha;
            }

            // Reposition C-Up button
            replacement_dl =
                Gfx_DrawRect_DropShadow(replacement_dl, 0xFE - 3, 0x10, 0x10, 0x10, 0x800, 0x800, 0xFF, 0xF0, 0, temp);

            gDPPipeSync(replacement_dl++);
            gDPSetPrimColor(replacement_dl++, 0, 0, 255, 255, 255, temp);
            gDPSetEnvColor(replacement_dl++, 0, 0, 0, 0);
            gDPSetCombineLERP(replacement_dl++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
            gDPLoadTextureBlock_4b(replacement_dl++, cUpLabelTextures[gSaveContext.options.language], G_IM_FMT_IA, 32, 12,
                0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOLOD);
            // Reposition C-Up text
            gSPTextureRectangle(replacement_dl++, 0x03DC - 23, 0x0048, 0x045C, 0x0078, G_TX_RENDERTILE, 0, 0, 1 << 10,
                1 << 10);

            // branch back to the command after the original C-Up texrect.
            Gfx* return_point = c_up_texrect + ARRAY_COUNT(to_compare);
            gSPBranchList(replacement_dl++, return_point);

        }
        
        // Jump back to before the C-Up was drawn
        gSPBranchList(OVERLAY_DISP, bGfx);

        // ------- code by Wiseguy -------

        bGfx = NULL;
    }

    // Draw correct C-button glyphs.
    InterfaceContext* interfaceCtx = &bPlayState->interfaceCtx;

    for (int temp = EQUIP_SLOT_C_LEFT; temp <= EQUIP_SLOT_C_RIGHT; temp++) {
        if (GET_CUR_FORM_BTN_ITEM(temp) > 0xF0) {
            char button[9];
            sprintf(button, "%u_button", temp);

            if (temp == EQUIP_SLOT_C_LEFT) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 240, 0, interfaceCtx->cLeftAlpha);
            } else if (temp == EQUIP_SLOT_C_DOWN) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 240, 0, interfaceCtx->cDownAlpha);
            } else { // EQUIP_SLOT_C_RIGHT
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 240, 0, interfaceCtx->cRightAlpha);
            }
            OVERLAY_DISP = Gfx_DrawTexRectIA8(OVERLAY_DISP, Mod_GlyphTexture(button),
                                              0x20, 0x20, sBCButtonXPositions[temp], sBCButtonYPositions[temp], 0x1B,
                                              0x1B, sBCButtonScales[temp] * 2, sBCButtonScales[temp] * 2);
        }
    }

    // Save the last ran instruction to check if Attack button icon was drawn.
    bGfx = OVERLAY_DISP;
    bGfx--;

    CLOSE_DISPS(bPlayState->state.gfxCtx);

    // Makes sure the Action button's offset is always zero - it doesn't need to be changed with this mod.
    R_A_BTN_Y_OFFSET = 0;
}

extern Gfx mSetCombineMode[1];
extern Gfx* mSetCombineMode_ptr;

RECOMP_HOOK("Interface_DrawCButtonIcons") void Interface_DrawCButtonIcons_Init(PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    // Get the last ran instruction.
    Gfx* last_ran = OVERLAY_DISP;
    last_ran--;

    if (recomp_get_config_u32("glyphs") == 2 &&
        // If Interface_DrawBButtonIcons wasn't called at all (last ran instruction is the same as the one at the end of Interface_DrawItemButtons).
        (bGfx == last_ran ||
        // If last ran instruction is the one that would be called if Interface_DrawBButtonIcons was called but nothing was drawn.
        (last_ran->words.w0 == mSetCombineMode_ptr->words.w0 && last_ran->words.w1 == mSetCombineMode_ptr->words.w1))) {
        // Draw Attack button glyph.
        gDPPipeSync(OVERLAY_DISP++);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 100, 255, 120, (&play->interfaceCtx)->bAlpha);
        OVERLAY_DISP = Gfx_DrawTexRectIA8(OVERLAY_DISP, Mod_GlyphTexture("attack_button"),
            0x20, 0x20, sBCButtonXPositions[0], sBCButtonYPositions[0], 0x1D,
            0x1D, ((s32)(1.1230469f * (1 << 10)) >> 1) * 2, ((s32)(1.1230469f * (1 << 10)) >> 1) * 2);
    }

    bGfx = NULL;

    CLOSE_DISPS(play->state.gfxCtx);
}