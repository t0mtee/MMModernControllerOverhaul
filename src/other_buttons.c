#include "modding.h"
#include "global.h"
#include "overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"

// function by Wiseguy
Gfx* find_dl_commands(Gfx* search_start, Gfx* search_end, Gfx* to_find, u32 num_commands) {
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

extern TexturePtr gTatlCUpENGTex[];
extern TexturePtr gTatlCUpGERTex[];
extern TexturePtr gTatlCUpFRATex[];
extern TexturePtr gTatlCUpESPTex[];
extern TexturePtr gButtonBackgroundTex[];

PlayState* bPlayState_o;
u16 bPauseState;

RECOMP_HOOK("Interface_DrawItemButtons") void Interface_DrawItemButtons_Init(PlayState* play) {
    bPlayState_o = play;
}

extern TexturePtr gButtonBackgroundTex[];
Gfx* bGfx_o = NULL;

RECOMP_HOOK("Gfx_DrawTexRectIA8_DropShadow") void Gfx_DrawTexRectIA8_DropShadow_Init(Gfx* gfx, TexturePtr texture, s16 textureWidth, s16 textureHeight,
    s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b, s16 a) {
    // @modern_layout If being run for the B-button
    if (texture == gButtonBackgroundTex && rectWidth == 0x1D) {
        // @modern_layout Trick conditions to disable Pause button. Do it after D-pad has stopped being drawn.
        bPauseState = (&bPlayState_o->pauseCtx)->state;
        (&bPlayState_o->pauseCtx)->state = PAUSE_STATE_OFF;
        // @modern_layout Save Gfx for use in fixing bug
        bGfx_o = gfx;
    }
}

RECOMP_HOOK_RETURN("Gfx_DrawTexRectIA8_DropShadow") void Gfx_DrawTexRectIA8_DropShadow_Return() {
    // @modern_layout Fixes a vanilla bug that becomes more noticable with this mod due to how it disables the Start button
    // @modern_layout The bug causes the ammo count to be green under certain conditions before gaining the magic meter at the start of the game
    // @modern_layout This fix ensures that code in the magic meter function that amends this bug is present all the time
    if (bGfx_o != NULL){
        gDPSetEnvColor(bGfx_o, 0, 0, 0, 255);
        bGfx_o = NULL;
    }
}

RECOMP_HOOK("Gfx_DrawRect_DropShadow") void Gfx_DrawRect_DropShadow_Init(Gfx* gfx, s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b, s16 a) {
    // @modern_layout If drawing C-Up button
    if (rectWidth == 0x10 && rectHeight == 0x10 && r == 0xFF && g == 0xF0 && b == 0) {
        bGfx_o = gfx;
    }
}

extern Gfx* Gfx_DrawRect_DropShadow(Gfx* gfx, s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b, s16 a);

RECOMP_HOOK_RETURN("Interface_DrawItemButtons") void Interface_DrawItemButtons_Return() {
    // @modern_layout Reset pause state
    (&bPlayState_o->pauseCtx)->state = bPauseState;

    if (bGfx_o != NULL) {
        static TexturePtr cUpLabelTextures[] = {
            gTatlCUpENGTex, gTatlCUpENGTex, gTatlCUpGERTex, gTatlCUpFRATex, gTatlCUpESPTex,
        };
        s16 temp;

        OPEN_DISPS(bPlayState_o->state.gfxCtx);

        // +++++++ code by Wiseguy +++++++
        
        // @modern_layout Build a dummy displaylist to compare against to find the vanilla C-Up texrect command. Mimic the original
        // command so it's bytewise identical.
        // TODO this can be done once and kept in data instead of being built every frame.
        Gfx to_compare[3]; // Texrects are 3 commands.
        Gfx* to_compare_ptr = to_compare;
        gSPTextureRectangle(to_compare_ptr++, 247 << 2, 18 << 2, (247 + 32) << 2, (18 + 12) << 2, G_TX_RENDERTILE, 0,
                            0, 1 << 10, 1 << 10);

        // @modern_layout Search from the start of the C-Up displaylist commands to find the texrect command that drew the button itself.
        // Limit searching until reaching the current OVERLAY_DISP value.
        Gfx* c_up_texrect = find_dl_commands(bGfx_o, OVERLAY_DISP, to_compare, ARRAY_COUNT(to_compare));
        if (c_up_texrect != NULL) {
            // @modern_layout Allocate a buffer to hold the new commands. Should only need 26 commands, but allocate 32 just to be safe.
            Gfx* replacement_dl = GRAPH_ALLOC(bPlayState_o->state.gfxCtx, sizeof(Gfx) * 32);

            // @modern_layout Overwrite the beginning of the C-Up drop shadow graphics commands with a branch to the new displaylist.
            gSPBranchList(bGfx_o, replacement_dl);

            // @modern_layout Draw correct C-Up on new displaylist.
            
            // C-Up Button Texture, Color & Label (Tatl Text)
            gDPPipeSync(replacement_dl++);

            if ((gSaveContext.hudVisibility == HUD_VISIBILITY_NONE) ||
                (gSaveContext.hudVisibility == HUD_VISIBILITY_NONE_ALT) ||
                (gSaveContext.hudVisibility == HUD_VISIBILITY_A_HEARTS_MAGIC_WITH_OVERWRITE) ||
                ((&bPlayState_o->msgCtx)->msgMode != MSGMODE_NONE)) {
                temp = 0;
            }
            else if (GET_PLAYER(bPlayState_o)->stateFlags1 & PLAYER_STATE1_200000) {
                temp = 70;
            }
            else {
                temp = (&bPlayState_o->interfaceCtx)->aAlpha;
            }

            // @modern_layout Reposition C-Up button
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
            // @modern_layout Reposition C-Up text
            gSPTextureRectangle(replacement_dl++, 0x03DC - 23, 0x0048, 0x045C, 0x0078, G_TX_RENDERTILE, 0, 0, 1 << 10,
                1 << 10);

            // @modern_layout branch back to the command after the original C-Up texrect.
            Gfx* return_point = c_up_texrect + ARRAY_COUNT(to_compare);
            gSPBranchList(replacement_dl++, return_point);
        }
        
        // @modern_layout Jump back to before the C-Up was drawn
        gSPBranchList(OVERLAY_DISP, bGfx_o);
    
        CLOSE_DISPS(bPlayState_o->state.gfxCtx);

        // ------- code by Wiseguy -------

        bGfx_o = NULL;
    }
}