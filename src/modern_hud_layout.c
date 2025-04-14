#include "modding.h"
#include "global.h"
#include "overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"

RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));

#define INCBIN(identifier, filename)          \
    asm(".pushsection .rodata\n"              \
        "\t.local " #identifier "\n"          \
        "\t.type " #identifier ", @object\n"  \
        "\t.balign 8\n"                       \
        #identifier ":\n"                     \
        "\t.incbin \"" filename "\"\n\n"      \
                                              \
        "\t.balign 8\n"                       \
        "\t.popsection\n");                   \
    extern u8 identifier[]

typedef enum {
    /* 0 */ PICTO_BOX_STATE_OFF,         // Not using the pictograph
    /* 1 */ PICTO_BOX_STATE_LENS,        // Looking through the lens of the pictograph
    /* 2 */ PICTO_BOX_STATE_SETUP_PHOTO, // Looking at the photo currently taken
    /* 3 */ PICTO_BOX_STATE_PHOTO
} PictoBoxState;

extern s16 sPictoState;

#define DO_ACTION_TEX_WIDTH 48
#define DO_ACTION_TEX_HEIGHT 16
#define DO_ACTION_TEX_SIZE ((DO_ACTION_TEX_WIDTH * DO_ACTION_TEX_HEIGHT) / 2) // (sizeof(gCheckDoActionENGTex))
extern TexturePtr gTatlCUpENGTex[];
extern TexturePtr gTatlCUpGERTex[];
extern TexturePtr gTatlCUpFRATex[];
extern TexturePtr gTatlCUpESPTex[];
extern TexturePtr gButtonBackgroundTex[];
extern s16 D_801BF9D4[4];
extern s16 D_801BF9DC[4];
extern s16 D_801BF9E4[4];
extern Gfx* Gfx_DrawRect_DropShadow(Gfx* gfx, s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b, s16 a);
extern Gfx* Gfx_DrawTexRectIA8_DropShadow(Gfx* gfx, TexturePtr texture, s16 textureWidth, s16 textureHeight, s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b, s16 a);
extern u16 sCUpInvisible;
extern u16 sCUpTimer;

RECOMP_PATCH void Interface_DrawItemButtons(PlayState* play) {
    static TexturePtr cUpLabelTextures[] = {
        gTatlCUpENGTex, gTatlCUpENGTex, gTatlCUpGERTex, gTatlCUpFRATex, gTatlCUpESPTex,
    };
    static s16 startButtonLeftPos[] = {
        // Remnant of OoT
        130, 136, 136, 136, 136,
    };
    static s16 D_801BFAF4[] = {
        0x1D, // EQUIP_SLOT_B
        0x1B, // EQUIP_SLOT_C_LEFT
        0x1B, // EQUIP_SLOT_C_DOWN
        0x1B, // EQUIP_SLOT_C_RIGHT
    };
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    Player* player = GET_PLAYER(play);
    PauseContext* pauseCtx = &play->pauseCtx;
    MessageContext* msgCtx = &play->msgCtx;
    s16 temp; // Used as both an alpha value and a button index
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    gDPPipeSync(OVERLAY_DISP++);
    gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    // @modern_layout Move UI elements if modern layout is enabled
    // Left distance
    D_801BF9D4[0] = 218;    // EQUIP_SLOT_B
    D_801BF9D4[1] = 194;    // EQUIP_SLOT_C_LEFT
    D_801BF9D4[2] = 219;    // EQUIP_SLOT_C_DOWN

    // Down distance
    D_801BF9DC[0] = 55;     // EQUIP_SLOT_B
    D_801BF9DC[1] = 35;     // EQUIP_SLOT_C_LEFT
    D_801BF9DC[2] = 15;     // EQUIP_SLOT_C_DOWN

    // B Button Color & Texture
    OVERLAY_DISP = Gfx_DrawTexRectIA8_DropShadow(
        OVERLAY_DISP, gButtonBackgroundTex, 0x20, 0x20, D_801BF9D4[EQUIP_SLOT_B], D_801BF9DC[EQUIP_SLOT_B],
        D_801BFAF4[EQUIP_SLOT_B], D_801BFAF4[EQUIP_SLOT_B], D_801BF9E4[EQUIP_SLOT_B] * 2, D_801BF9E4[EQUIP_SLOT_B] * 2,
        100, 255, 120, interfaceCtx->bAlpha);
    gDPPipeSync(OVERLAY_DISP++);

    // C-Left Button Color & Texture
    OVERLAY_DISP = Gfx_DrawRect_DropShadow(OVERLAY_DISP, D_801BF9D4[EQUIP_SLOT_C_LEFT], D_801BF9DC[EQUIP_SLOT_C_LEFT],
        D_801BFAF4[EQUIP_SLOT_C_LEFT], D_801BFAF4[EQUIP_SLOT_C_LEFT],
        D_801BF9E4[EQUIP_SLOT_C_LEFT] * 2, D_801BF9E4[EQUIP_SLOT_C_LEFT] * 2, 255,
        240, 0, interfaceCtx->cLeftAlpha);
    // C-Down Button Color & Texture
    OVERLAY_DISP = Gfx_DrawRect_DropShadow(OVERLAY_DISP, D_801BF9D4[EQUIP_SLOT_C_DOWN], D_801BF9DC[EQUIP_SLOT_C_DOWN],
        D_801BFAF4[EQUIP_SLOT_C_DOWN], D_801BFAF4[EQUIP_SLOT_C_DOWN],
        D_801BF9E4[EQUIP_SLOT_C_DOWN] * 2, D_801BF9E4[EQUIP_SLOT_C_DOWN] * 2, 255,
        240, 0, interfaceCtx->cDownAlpha);
    // C-Right Button Color & Texture
    OVERLAY_DISP = Gfx_DrawRect_DropShadow(OVERLAY_DISP, D_801BF9D4[EQUIP_SLOT_C_RIGHT], D_801BF9DC[EQUIP_SLOT_C_RIGHT],
        D_801BFAF4[EQUIP_SLOT_C_RIGHT], D_801BFAF4[EQUIP_SLOT_C_RIGHT],
        D_801BF9E4[EQUIP_SLOT_C_RIGHT] * 2, D_801BF9E4[EQUIP_SLOT_C_RIGHT] * 2, 255,
        240, 0, interfaceCtx->cRightAlpha);

    // @modern_layout Don't show start button as it is not consistent with modern games, isn't needed and looks ugly
    //if (!IS_PAUSE_STATE_GAMEOVER) {
    //    if ((play->pauseCtx.state != PAUSE_STATE_OFF) || (play->pauseCtx.debugEditor != DEBUG_EDITOR_NONE)) {
    //        OVERLAY_DISP = Gfx_DrawRect_DropShadow(OVERLAY_DISP, 136, 0x11, 0x16, 0x16, 0x5B6, 0x5B6, 0xFF, 0x82, 0x3C,
    //            interfaceCtx->startAlpha);
    //        // Start Button Texture, Color & Label
    //        gDPPipeSync(OVERLAY_DISP++);
    //        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->startAlpha);
    //        gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 0);
    //        gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
    //            PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    //        gDPLoadTextureBlock_4b(OVERLAY_DISP++, interfaceCtx->doActionSegment + DO_ACTION_TEX_SIZE * 2, G_IM_FMT_IA,
    //            DO_ACTION_TEX_WIDTH, DO_ACTION_TEX_HEIGHT, 0, G_TX_NOMIRROR | G_TX_WRAP,
    //            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    //        gSPTextureRectangle(OVERLAY_DISP++, 0x01F8, 0x0054, 0x02D4, 0x009C, G_TX_RENDERTILE, 0, 0, 0x04A6, 0x04A6);
    //    }
    //}

    if (interfaceCtx->tatlCalling && (play->pauseCtx.state == PAUSE_STATE_OFF) &&
        (play->pauseCtx.debugEditor == DEBUG_EDITOR_NONE) && (play->csCtx.state == CS_STATE_IDLE) &&
        (sPictoState == PICTO_BOX_STATE_OFF)) {
        if (sCUpInvisible == 0) {
            // C-Up Button Texture, Color & Label (Tatl Text)
            gDPPipeSync(OVERLAY_DISP++);

            if ((gSaveContext.hudVisibility == HUD_VISIBILITY_NONE) ||
                (gSaveContext.hudVisibility == HUD_VISIBILITY_NONE_ALT) ||
                (gSaveContext.hudVisibility == HUD_VISIBILITY_A_HEARTS_MAGIC_WITH_OVERWRITE) ||
                (msgCtx->msgMode != MSGMODE_NONE)) {
                temp = 0;
            }
            else if (player->stateFlags1 & PLAYER_STATE1_200000) {
                temp = 70;
            }
            else {
                temp = interfaceCtx->aAlpha;
            }

            // @modern_layout Reposition C-Up button
            OVERLAY_DISP =
                Gfx_DrawRect_DropShadow(OVERLAY_DISP, 0xFE - 3, 0x10, 0x10, 0x10, 0x800, 0x800, 0xFF, 0xF0, 0, temp);

            gDPPipeSync(OVERLAY_DISP++);
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, temp);
            gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 0);
            gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
            gDPLoadTextureBlock_4b(OVERLAY_DISP++, cUpLabelTextures[gSaveContext.options.language], G_IM_FMT_IA, 32, 12,
                0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOLOD);
            // @modern_layout Reposition C-Up text
            gSPTextureRectangle(OVERLAY_DISP++, 0x03DC - 23, 0x0048, 0x045C, 0x0078, G_TX_RENDERTILE, 0, 0, 1 << 10,
                1 << 10);
        }

        sCUpTimer--;
        if (sCUpTimer == 0) {
            sCUpInvisible ^= 1;
            sCUpTimer = 10;
        }
    }

    gDPPipeSync(OVERLAY_DISP++);

    // Empty C Button Arrows
    for (temp = EQUIP_SLOT_C_LEFT; temp <= EQUIP_SLOT_C_RIGHT; temp++) {
        if (GET_CUR_FORM_BTN_ITEM(temp) > 0xF0) {
            if (temp == EQUIP_SLOT_C_LEFT) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 240, 0, interfaceCtx->cLeftAlpha);
            }
            else if (temp == EQUIP_SLOT_C_DOWN) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 240, 0, interfaceCtx->cDownAlpha);
            }
            else { // EQUIP_SLOT_C_RIGHT
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 240, 0, interfaceCtx->cRightAlpha);
            }
            OVERLAY_DISP = Gfx_DrawTexRectIA8(OVERLAY_DISP, ((u8*)gButtonBackgroundTex + ((32 * 32) * (temp + 1))),
                0x20, 0x20, D_801BF9D4[temp], D_801BF9DC[temp], D_801BFAF4[temp],
                D_801BFAF4[temp], D_801BF9E4[temp] * 2, D_801BF9E4[temp] * 2);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

extern f32 D_801BF9CC[2];
extern void Interface_SetPerspectiveView(PlayState* play, s32 topY, s32 bottomY, s32 leftX, s32 rightX);

INCBIN(aButtonEmpty, "./textures/a_icon.ia8.bin");

RECOMP_PATCH void Interface_DrawAButton(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s16 aAlpha;

    OPEN_DISPS(play->state.gfxCtx);

    aAlpha = interfaceCtx->aAlpha;

    if (aAlpha > 100) {
        aAlpha = 100;
    }

    Gfx_SetupDL42_Overlay(play->state.gfxCtx);

    // @modern_layout Change A button position
    Interface_SetPerspectiveView(play, 28 + R_A_BTN_Y_OFFSET, 73 + R_A_BTN_Y_OFFSET, 237, 282);

    gSPClearGeometryMode(OVERLAY_DISP++, G_CULL_BOTH);
    gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetAlphaCompare(OVERLAY_DISP++, G_AC_THRESHOLD);

    Matrix_Translate(0.0f, 0.0f, -38.0f, MTXMODE_NEW);
    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
    Matrix_RotateXFApply(interfaceCtx->aButtonRoll / 10000.0f);

    // Draw A button Shadow
    gSPMatrix(OVERLAY_DISP++, Matrix_NewMtx(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPPipeSync(OVERLAY_DISP++);
    gSPVertex(OVERLAY_DISP++, &interfaceCtx->actionVtx[4], 4, 0);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 0, 0, 0, aAlpha);

    // @modern_layout Draw empty a button if no action present
    if (interfaceCtx -> aButtonDoAction == DO_ACTION_NONE && recomp_get_config_u32("icon_replacement") == 1)
    {
        OVERLAY_DISP = Gfx_DrawTexQuadIA8(OVERLAY_DISP, aButtonEmpty, 32, 32, 0);
    }
    else{
        OVERLAY_DISP = Gfx_DrawTexQuadIA8(OVERLAY_DISP, gButtonBackgroundTex, 32, 32, 0);
    }

    // Draw A Button Colored
    gDPPipeSync(OVERLAY_DISP++);
    // @modern_layout Change A button position
    Interface_SetPerspectiveView(play, 26 + R_A_BTN_Y_OFFSET, 71 + R_A_BTN_Y_OFFSET, 235, 280);
    gSPVertex(OVERLAY_DISP++, &interfaceCtx->actionVtx[0], 4, 0);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 100, 200, 255, interfaceCtx->aAlpha);
    gSP1Quadrangle(OVERLAY_DISP++, 0, 2, 3, 1, 0);

    // Draw A Button Do-Action
    gDPPipeSync(OVERLAY_DISP++);
    // @modern_layout Change A button position
    Interface_SetPerspectiveView(play, 26 + R_A_BTN_Y_OFFSET, 71 + R_A_BTN_Y_OFFSET, 235, 280);
    gSPSetGeometryMode(OVERLAY_DISP++, G_CULL_BACK);
    gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
        ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->aAlpha);
    gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 0);

    Matrix_Translate(0.0f, 0.0f, D_801BF9CC[gSaveContext.options.language] / 10.0f, MTXMODE_NEW);
    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
    Matrix_RotateXFApply(interfaceCtx->aButtonRoll / 10000.0f);
    gSPMatrix(OVERLAY_DISP++, Matrix_NewMtx(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPVertex(OVERLAY_DISP++, &interfaceCtx->actionVtx[8], 4, 0);

    // Draw Action Label
    if (((interfaceCtx->aButtonState <= A_BTN_STATE_1) || (interfaceCtx->aButtonState == A_BTN_STATE_3))) {
        OVERLAY_DISP = Gfx_DrawTexQuad4b(OVERLAY_DISP, interfaceCtx->doActionSegment, 3, DO_ACTION_TEX_WIDTH,
            DO_ACTION_TEX_HEIGHT, 0);
    }
    else {
        OVERLAY_DISP = Gfx_DrawTexQuad4b(OVERLAY_DISP, interfaceCtx->doActionSegment + DO_ACTION_TEX_SIZE, 3,
            DO_ACTION_TEX_WIDTH, DO_ACTION_TEX_HEIGHT, 0);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

extern s16 D_801BFB04[4];
extern s16 D_801BFB0C[4];

RECOMP_HOOK("Interface_DrawAmmoCount") void Interface_DrawAmmoCount_Init(PlayState* play, s16 button, s16 alpha) {
    // @modern_layout Move ammo UI if modern layout is enabled
    // Left distance
    D_801BFB04[0] = 213;    // EQUIP_SLOT_B
    D_801BFB04[1] = 195;    // EQUIP_SLOT_C_LEFT
    D_801BFB04[2] = 220;    // EQUIP_SLOT_C_DOWN

    // Down distance
    D_801BFB0C[0] = 73;     // EQUIP_SLOT_B
    D_801BFB0C[1] = 52;     // EQUIP_SLOT_C_LEFT
    D_801BFB0C[2] = 32;     // EQUIP_SLOT_C_DOWN
}

extern s16 D_801BF9C4[2];
extern s16 D_801BF9C8[2];

INCBIN(bButtonEmpty, "./textures/b_empty_icon.ia8.bin");
bool bButtonEmptyDrawn = false;

RECOMP_HOOK("Interface_DrawBButtonIcons") void Interface_DrawBButtonIcons(PlayState* play) {
    bButtonEmptyDrawn = false;

    D_801BF9C4[0] = 209;
    D_801BF9C4[1] = 206;

    D_801BF9C8[0] = 61;
    D_801BF9C8[1] = 60;

    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    OPEN_DISPS(play->state.gfxCtx);

    if (recomp_get_config_u32("icon_replacement") == 1 &&
        (BUTTON_ITEM_EQUIP(CUR_FORM, EQUIP_SLOT_B) == ITEM_NONE || interfaceCtx->bButtonDoAction == DO_ACTION_NONE) &&
        (&play->pauseCtx)->state == PAUSE_STATE_OFF) {
        bButtonEmptyDrawn = true;
        gDPPipeSync(OVERLAY_DISP++);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 100, 255, 120, interfaceCtx->bAlpha);
        OVERLAY_DISP = Gfx_DrawTexRectIA8(OVERLAY_DISP, bButtonEmpty,
            0x20, 0x20, 218, 55, 0x1D,
            0x1D, ((s32)(1.1230469f * (1 << 10)) >> 1) * 2, ((s32)(1.1230469f * (1 << 10)) >> 1) * 2);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

// @modern_layout Change equip animation destination
extern s16 sCButtonPosX[3];
extern s16 sCButtonPosY[3];

RECOMP_HOOK("KaleidoScope_UpdateItemEquip") void KaleidoScope_UpdateItemEquip_Init(PlayState* play) {
    sCButtonPosX[0] = 350;
    sCButtonPosY[0] = 950;

    sCButtonPosX[1] = 550;
    sCButtonPosY[1] = 1100;
}

// @modern_layout Change position of the equip animation destination.
extern s16 sMaskCButtonPosX[3];
extern s16 sMaskCButtonPosY[3];

RECOMP_HOOK("KaleidoScope_UpdateMaskEquip") void KaleidoScope_UpdateMaskEquip_Init(PlayState* play) {
    sMaskCButtonPosX[0] = 350;
    sMaskCButtonPosX[1] = 550;

    sMaskCButtonPosY[0] = 950;
    sMaskCButtonPosY[1] = 1110;
}

// @modern_layout To keep the buttons consistent, if there is no item in the B-button, keep the alpha at max.
PlayState* play_save;

RECOMP_HOOK("Interface_UpdateButtonAlphasByStatus") void Interface_UpdateButtonAlphasByStatus_Init(PlayState* play, s16 risingAlpha) {
    play_save = play;
}

RECOMP_HOOK_RETURN("Interface_UpdateButtonAlphasByStatus") void Interface_UpdateButtonAlphasByStatus_Return() {
    if (bButtonEmptyDrawn == true) {
        (&play_save->interfaceCtx)->bAlpha = 255;
    }
}