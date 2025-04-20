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

PlayState* Interface_DrawItemButtons_play;
u16 Interface_DrawItemButtons_pause_state;

RECOMP_HOOK("Interface_DrawItemButtons") void Interface_DrawItemButtons_Init(PlayState* play) {
    Interface_DrawItemButtons_play = play;
    // @modern_layout Disable Start button in pause menu by tricking conditions
    Interface_DrawItemButtons_pause_state = (&play->pauseCtx)->state;
    (&play->pauseCtx)->state = PAUSE_STATE_OFF;
}

Gfx* Gfx_DrawRect_DropShadow_gfx = NULL;

RECOMP_HOOK("Gfx_DrawRect_DropShadow") void Gfx_DrawRect_DropShadow_Init(Gfx* gfx, s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b, s16 a) {
    // @modern_layout If drawing C-Up button
    if (rectLeft == 0xFE && rectTop == 0x10 && rectWidth == 0x10 && rectHeight == 0x10 && dsdx == 0x800 && dtdy == 0x800 && r == 0xFF && g == 0xF0 && b == 0) {
        Gfx_DrawRect_DropShadow_gfx = gfx;
    }
}

extern Gfx* Gfx_DrawRect_DropShadow(Gfx* gfx, s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b, s16 a);

RECOMP_HOOK_RETURN("Interface_DrawItemButtons") void Interface_DrawItemButtons_Return() {
    // @modern_layout Reset pause state
    (&Interface_DrawItemButtons_play->pauseCtx)->state = Interface_DrawItemButtons_pause_state;

    if (Gfx_DrawRect_DropShadow_gfx != NULL) {
        static TexturePtr cUpLabelTextures[] = {
            gTatlCUpENGTex, gTatlCUpENGTex, gTatlCUpGERTex, gTatlCUpFRATex, gTatlCUpESPTex,
        };
        s16 temp;

        OPEN_DISPS(Interface_DrawItemButtons_play->state.gfxCtx);

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
        Gfx* c_up_texrect = find_dl_commands(Gfx_DrawRect_DropShadow_gfx, OVERLAY_DISP, to_compare, ARRAY_COUNT(to_compare));
        if (c_up_texrect != NULL) {
            // @modern_layout Allocate a buffer to hold the new commands. Should only need 26 commands, but allocate 32 just to be safe.
            Gfx* replacement_dl = GRAPH_ALLOC(Interface_DrawItemButtons_play->state.gfxCtx, sizeof(Gfx) * 32);

            // @modern_layout Overwrite the beginning of the C-Up drop shadow graphics commands with a branch to the new displaylist.
            gSPBranchList(Gfx_DrawRect_DropShadow_gfx, replacement_dl);

            // @modern_layout Draw correct C-Up on new displaylist.
            
            // C-Up Button Texture, Color & Label (Tatl Text)
            gDPPipeSync(replacement_dl++);

            if ((gSaveContext.hudVisibility == HUD_VISIBILITY_NONE) ||
                (gSaveContext.hudVisibility == HUD_VISIBILITY_NONE_ALT) ||
                (gSaveContext.hudVisibility == HUD_VISIBILITY_A_HEARTS_MAGIC_WITH_OVERWRITE) ||
                ((&Interface_DrawItemButtons_play->msgCtx)->msgMode != MSGMODE_NONE)) {
                temp = 0;
            }
            else if (GET_PLAYER(Interface_DrawItemButtons_play)->stateFlags1 & PLAYER_STATE1_200000) {
                temp = 70;
            }
            else {
                temp = (&Interface_DrawItemButtons_play->interfaceCtx)->aAlpha;
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
        gSPBranchList(OVERLAY_DISP, Gfx_DrawRect_DropShadow_gfx);
    
        CLOSE_DISPS(Interface_DrawItemButtons_play->state.gfxCtx);

        // ------- code by Wiseguy -------

        Gfx_DrawRect_DropShadow_gfx = NULL;
    }
}

// @modern_layout Fixes a vanilla bug that becomes more noticable with this mod due to how it disables the Start button
// @modern_layout The bug causes the ammo count to be green under certain conditions before gaining the magic meter at the start of the game
// @modern_layout This fix ensures that code in the magic meter function that amends this bug is present all the time
extern TexturePtr gButtonBackgroundTex[];
Gfx* Gfx_DrawTexRectIA8_DropShadow_gfx = NULL;

RECOMP_HOOK("Gfx_DrawTexRectIA8_DropShadow") void Gfx_DrawTexRectIA8_DropShadow_Init(Gfx* gfx, TexturePtr texture, s16 textureWidth, s16 textureHeight,
    s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b, s16 a) {
    if (texture == gButtonBackgroundTex && textureWidth == 0x20 && textureHeight == 0x20 && rectLeft == 218 &&
    rectTop == 55 && rectWidth == 0x1D && rectHeight == 0x1D && dsdx == 0x23F * 2 && dtdy == 0x23F * 2 && r == 100 && g == 255 && b == 120) {
        Gfx_DrawTexRectIA8_DropShadow_gfx = gfx;
    }
}

RECOMP_HOOK_RETURN("Gfx_DrawTexRectIA8_DropShadow") void Gfx_DrawTexRectIA8_DropShadow_Return() {
    if (Gfx_DrawTexRectIA8_DropShadow_gfx != NULL){
        gDPSetEnvColor(Gfx_DrawTexRectIA8_DropShadow_gfx, 0, 0, 0, 255);
        Gfx_DrawTexRectIA8_DropShadow_gfx = NULL;
    }
}