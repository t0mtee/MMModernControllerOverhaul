#include "modding.h"
#include "global.h"
#include "overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"

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

    OVERLAY_DISP = Gfx_DrawTexQuadIA8(OVERLAY_DISP, gButtonBackgroundTex, 32, 32, 0);

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

extern TexturePtr gAmmoDigit0Tex[];
extern s16 D_801BFB04[4];
extern s16 D_801BFB0C[4];

RECOMP_PATCH void Interface_DrawAmmoCount(PlayState* play, s16 button, s16 alpha) {
    u8 i;
    u16 ammo;

    // @modern_layout Move ammo UI if modern layout is enabled
    // Left distance
    D_801BFB04[0] = 213;    // EQUIP_SLOT_B
    D_801BFB04[1] = 195;    // EQUIP_SLOT_C_LEFT
    D_801BFB04[2] = 220;    // EQUIP_SLOT_C_DOWN

    // Down distance
    D_801BFB0C[0] = 73;     // EQUIP_SLOT_B
    D_801BFB0C[1] = 52;     // EQUIP_SLOT_C_LEFT
    D_801BFB0C[2] = 32;     // EQUIP_SLOT_C_DOWN

    OPEN_DISPS(play->state.gfxCtx);

    i = ((void)0, GET_CUR_FORM_BTN_ITEM(button));

    if ((i == ITEM_DEKU_STICK) || (i == ITEM_DEKU_NUT) || (i == ITEM_BOMB) || (i == ITEM_BOW) ||
        ((i >= ITEM_BOW_FIRE) && (i <= ITEM_BOW_LIGHT)) || (i == ITEM_BOMBCHU) || (i == ITEM_POWDER_KEG) ||
        (i == ITEM_MAGIC_BEANS) || (i == ITEM_PICTOGRAPH_BOX)) {

        if ((i >= ITEM_BOW_FIRE) && (i <= ITEM_BOW_LIGHT)) {
            i = ITEM_BOW;
        }

        ammo = AMMO(i);

        if (i == ITEM_PICTOGRAPH_BOX) {
            if (!CHECK_QUEST_ITEM(QUEST_PICTOGRAPH)) {
                ammo = 0;
            }
            else {
                ammo = 1;
            }
        }

        gDPPipeSync(OVERLAY_DISP++);

        if ((button == EQUIP_SLOT_B) && (gSaveContext.minigameStatus == MINIGAME_STATUS_ACTIVE)) {
            ammo = play->interfaceCtx.minigameAmmo;
        }
        else if ((button == EQUIP_SLOT_B) && (play->unk_1887C > 1)) {
            ammo = play->unk_1887C - 1;
        }
        else if (((i == ITEM_BOW) && (AMMO(i) == CUR_CAPACITY(UPG_QUIVER))) ||
            ((i == ITEM_BOMB) && (AMMO(i) == CUR_CAPACITY(UPG_BOMB_BAG))) ||
            ((i == ITEM_DEKU_STICK) && (AMMO(i) == CUR_CAPACITY(UPG_DEKU_STICKS))) ||
            ((i == ITEM_DEKU_NUT) && (AMMO(i) == CUR_CAPACITY(UPG_DEKU_NUTS))) ||
            ((i == ITEM_BOMBCHU) && (AMMO(i) == CUR_CAPACITY(UPG_BOMB_BAG))) ||
            ((i == ITEM_POWDER_KEG) && (ammo == 1)) || ((i == ITEM_PICTOGRAPH_BOX) && (ammo == 1)) ||
            ((i == ITEM_MAGIC_BEANS) && (ammo == 20))) {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 120, 255, 0, alpha);
        }

        if ((u32)ammo == 0) {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 100, 100, 100, alpha);
        }

        for (i = 0; ammo >= 10; i++) {
            ammo -= 10;
        }

        // Draw upper digit (tens)
        if ((u32)i != 0) {
            OVERLAY_DISP = Gfx_DrawTexRectIA8(OVERLAY_DISP, ((u8*)gAmmoDigit0Tex + ((8 * 8) * i)), 8, 8,
                D_801BFB04[button], D_801BFB0C[button], 8, 8, 1 << 10, 1 << 10);
        }

        // Draw lower digit (ones)
        OVERLAY_DISP = Gfx_DrawTexRectIA8(OVERLAY_DISP, ((u8*)gAmmoDigit0Tex + ((8 * 8) * ammo)), 8, 8,
            D_801BFB04[button] + 6, D_801BFB0C[button], 8, 8, 1 << 10, 1 << 10);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

extern s16 D_801BF9B0;
extern f32 D_801BF9B4[2];
extern s16 D_801BF9C4[2];
extern s16 D_801BF9C8[2];
extern void Interface_DrawItemIconTexture(PlayState* play, TexturePtr texture, s16 button);

RECOMP_PATCH void Interface_DrawBButtonIcons(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    Player* player = GET_PLAYER(play);

    OPEN_DISPS(play->state.gfxCtx);

    gDPPipeSync(OVERLAY_DISP++);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->bAlpha);
    gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    if ((interfaceCtx->unk_222 == 0) && (player->stateFlags3 & PLAYER_STATE3_1000000)) {
        if (gSaveContext.buttonStatus[EQUIP_SLOT_B] != BTN_DISABLED) {
            Interface_DrawItemIconTexture(play, interfaceCtx->iconItemSegment, EQUIP_SLOT_B);
            gDPPipeSync(OVERLAY_DISP++);
            gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

            Interface_DrawAmmoCount(play, EQUIP_SLOT_B, interfaceCtx->bAlpha);
        }
    }
    else if ((!interfaceCtx->unk_21C && (interfaceCtx->unk_222 == 0)) ||
        ((interfaceCtx->unk_21C &&
            ((BUTTON_ITEM_EQUIP(CUR_FORM, EQUIP_SLOT_B) < ITEM_SWORD_KOKIRI) ||
                (BUTTON_ITEM_EQUIP(CUR_FORM, EQUIP_SLOT_B) > ITEM_SWORD_GILDED)) &&
            BUTTON_ITEM_EQUIP(CUR_FORM, EQUIP_SLOT_B) != ITEM_NONE) &&
            (BUTTON_ITEM_EQUIP(CUR_FORM, EQUIP_SLOT_B) != ITEM_DEKU_NUT))) {
        if ((player->transformation == PLAYER_FORM_FIERCE_DEITY) || (player->transformation == PLAYER_FORM_HUMAN)) {
            if (BUTTON_ITEM_EQUIP(CUR_FORM, EQUIP_SLOT_B) != ITEM_NONE) {
                Interface_DrawItemIconTexture(play, interfaceCtx->iconItemSegment, EQUIP_SLOT_B);
                if ((player->stateFlags1 & PLAYER_STATE1_800000) || CHECK_WEEKEVENTREG(WEEKEVENTREG_08_01) ||
                    (play->unk_1887C >= 2)) {
                    gDPPipeSync(OVERLAY_DISP++);
                    gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE,
                        0, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

                    if ((play->sceneId != SCENE_SYATEKI_MIZU) && (play->sceneId != SCENE_SYATEKI_MORI) &&
                        (play->sceneId != SCENE_BOWLING) &&
                        ((gSaveContext.minigameStatus != MINIGAME_STATUS_ACTIVE) ||
                            (gSaveContext.save.entrance != ENTRANCE(ROMANI_RANCH, 0))) &&
                        ((gSaveContext.minigameStatus != MINIGAME_STATUS_ACTIVE) || !CHECK_EVENTINF(EVENTINF_35)) &&
                        (!CHECK_WEEKEVENTREG(WEEKEVENTREG_31_80) || (play->unk_1887C != 100))) {
                        Interface_DrawAmmoCount(play, EQUIP_SLOT_B, interfaceCtx->bAlpha);
                    }
                }
            }
        }
    }
    else if (interfaceCtx->unk_222 != 0) {
        gDPPipeSync(OVERLAY_DISP++);
        gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
            PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->bAlpha);
        gDPLoadTextureBlock_4b(OVERLAY_DISP++, interfaceCtx->doActionSegment + 0x480, G_IM_FMT_IA, 48, 16, 0,
            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
            G_TX_NOLOD, G_TX_NOLOD);

        D_801BF9B0 = 1024.0f / (D_801BF9B4[gSaveContext.options.language] / 100.0f);

        // @modern_layout Move resume label
        gSPTextureRectangle(
            OVERLAY_DISP++, (D_801BF9C4[gSaveContext.options.language] + 51) * 4,
            (D_801BF9C8[gSaveContext.options.language] + 38) * 4, ((D_801BF9C4[gSaveContext.options.language] + 0x30 + 51) << 2),
            ((D_801BF9C8[gSaveContext.options.language] + 38 + 0x10) << 2), G_TX_RENDERTILE, 0, 0, D_801BF9B0, D_801BF9B0);
    }
    else if (interfaceCtx->bButtonDoAction != DO_ACTION_NONE) {
        gDPPipeSync(OVERLAY_DISP++);
        gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
            PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->bAlpha);
        gDPLoadTextureBlock_4b(OVERLAY_DISP++, interfaceCtx->doActionSegment + 0x600, G_IM_FMT_IA, 48, 16, 0,
            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
            G_TX_NOLOD, G_TX_NOLOD);

        D_801BF9B0 = 1024.0f / (D_801BF9B4[gSaveContext.options.language] / 100.0f);

        // @modern_layout Move action labels
        gSPTextureRectangle(
            OVERLAY_DISP++, (D_801BF9C4[gSaveContext.options.language] + 51) * 4,
            (D_801BF9C8[gSaveContext.options.language] + 38) * 4, ((D_801BF9C4[gSaveContext.options.language] + 0x30 + 51) << 2),
            ((D_801BF9C8[gSaveContext.options.language] + 0x10 + 38) << 2), G_TX_RENDERTILE, 0, 0, D_801BF9B0, D_801BF9B0);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

extern s16 sCButtonPosX[3];
extern s16 sCButtonPosY[3];
extern s16 sEquipState;
extern s16 sEquipMagicArrowSlotHoldTimer;
extern s16 sEquipAnimTimer;

RECOMP_PATCH void KaleidoScope_UpdateItemEquip(PlayState* play) {
    // @modern_layout Change equip animation destination
    sCButtonPosX[0] = 350;
    sCButtonPosY[0] = 950;

    sCButtonPosX[1] = 550;
    sCButtonPosY[1] = 1100;

    static s16 sEquipMagicArrowBowSlotHoldTimer = 0;
    PauseContext* pauseCtx = &play->pauseCtx;
    Vtx* bowItemVtx;
    u16 offsetX;
    u16 offsetY;

    // Grow glowing orb when equipping magic arrows
    if (sEquipState == EQUIP_STATE_MAGIC_ARROW_GROW_ORB) {
        pauseCtx->equipAnimAlpha += 14;
        if (pauseCtx->equipAnimAlpha > 255) {
            pauseCtx->equipAnimAlpha = 254;
            sEquipState++;
        }
        // Hover over magic arrow slot when the next state is reached
        sEquipMagicArrowSlotHoldTimer = 5;
        return;
    }

    if (sEquipState == EQUIP_STATE_MAGIC_ARROW_HOVER_OVER_BOW_SLOT) {
        sEquipMagicArrowBowSlotHoldTimer--;

        if (sEquipMagicArrowBowSlotHoldTimer == 0) {
            pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
            pauseCtx->equipTargetSlot = SLOT_BOW;
            sEquipAnimTimer = 6;
            pauseCtx->equipAnimScale = 320;
            pauseCtx->equipAnimShrinkRate = 40;
            sEquipState++;
            Audio_PlaySfx(NA_SE_SY_SYNTH_MAGIC_ARROW);
        }
        return;
    }

    if (sEquipState == EQUIP_STATE_MAGIC_ARROW_MOVE_TO_BOW_SLOT) {
        bowItemVtx = &pauseCtx->itemVtx[SLOT_BOW * 4];
        offsetX = ABS_ALT(pauseCtx->equipAnimX - bowItemVtx->v.ob[0] * 10) / sEquipAnimTimer;
        offsetY = ABS_ALT(pauseCtx->equipAnimY - bowItemVtx->v.ob[1] * 10) / sEquipAnimTimer;
    }
    else {
        offsetX = ABS_ALT(pauseCtx->equipAnimX - sCButtonPosX[pauseCtx->equipTargetCBtn]) / sEquipAnimTimer;
        offsetY = ABS_ALT(pauseCtx->equipAnimY - sCButtonPosY[pauseCtx->equipTargetCBtn]) / sEquipAnimTimer;
    }

    if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipAnimAlpha < 254)) {
        pauseCtx->equipAnimAlpha += 14;
        if (pauseCtx->equipAnimAlpha > 255) {
            pauseCtx->equipAnimAlpha = 254;
        }
        sEquipMagicArrowSlotHoldTimer = 5;
        return;
    }

    if (sEquipMagicArrowSlotHoldTimer == 0) {
        pauseCtx->equipAnimScale -= pauseCtx->equipAnimShrinkRate / sEquipAnimTimer;
        pauseCtx->equipAnimShrinkRate -= pauseCtx->equipAnimShrinkRate / sEquipAnimTimer;

        // Update coordinates of item icon while being equipped
        if (sEquipState == EQUIP_STATE_MAGIC_ARROW_MOVE_TO_BOW_SLOT) {
            // target is the bow slot
            if (pauseCtx->equipAnimX >= (pauseCtx->itemVtx[SLOT_BOW * 4].v.ob[0] * 10)) {
                pauseCtx->equipAnimX -= offsetX;
            }
            else {
                pauseCtx->equipAnimX += offsetX;
            }

            if (pauseCtx->equipAnimY >= (pauseCtx->itemVtx[SLOT_BOW * 4].v.ob[1] * 10)) {
                pauseCtx->equipAnimY -= offsetY;
            }
            else {
                pauseCtx->equipAnimY += offsetY;
            }
        }
        else {
            // target is the c button
            if (pauseCtx->equipAnimX >= sCButtonPosX[pauseCtx->equipTargetCBtn]) {
                pauseCtx->equipAnimX -= offsetX;
            }
            else {
                pauseCtx->equipAnimX += offsetX;
            }

            if (pauseCtx->equipAnimY >= sCButtonPosY[pauseCtx->equipTargetCBtn]) {
                pauseCtx->equipAnimY -= offsetY;
            }
            else {
                pauseCtx->equipAnimY += offsetY;
            }
        }

        sEquipAnimTimer--;
        if (sEquipAnimTimer == 0) {
            if (sEquipState == EQUIP_STATE_MAGIC_ARROW_MOVE_TO_BOW_SLOT) {
                sEquipState++;
                sEquipMagicArrowBowSlotHoldTimer = 4;
                return;
            }

            // Equip item onto c buttons
            if (pauseCtx->equipTargetCBtn == PAUSE_EQUIP_C_LEFT) {
                // Swap if item is already equipped on CDown or CRight.
                if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) & 0xFF) != ITEM_NONE) {
                        if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                            (((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) & 0xFF) == ITEM_BOW) ||
                                (((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) & 0xFF) >= ITEM_BOW_FIRE) &&
                                    ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                            pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                            pauseCtx->equipTargetSlot = SLOT_BOW;
                        }
                        else {
                            BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT);
                            C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT);
                            Interface_LoadItemIcon(play, EQUIP_SLOT_C_DOWN);
                        }
                    }
                    else {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = ITEM_NONE;
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = SLOT_NONE;
                    }
                }
                else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) & 0xFF) != ITEM_NONE) {
                        if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                            (((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) & 0xFF) == ITEM_BOW) ||
                                (((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) & 0xFF) >= ITEM_BOW_FIRE) &&
                                    ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                            pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                            pauseCtx->equipTargetSlot = SLOT_BOW;
                        }
                        else {
                            BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT);
                            C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT);
                            Interface_LoadItemIcon(play, EQUIP_SLOT_C_RIGHT);
                        }
                    }
                    else {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = ITEM_NONE;
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = SLOT_NONE;
                    }
                }

                // Special case for magic arrows
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) == ITEM_BOW) ||
                        ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) >= ITEM_BOW_FIRE) &&
                            (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) <= ITEM_BOW_LIGHT))) {
                        pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                        pauseCtx->equipTargetSlot = SLOT_BOW;
                    }
                }
                else if (pauseCtx->equipTargetItem == ITEM_BOW) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) >= ITEM_BOW_FIRE) &&
                        (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) <= ITEM_BOW_LIGHT)) {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT);
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT);
                        Interface_LoadItemIcon(play, EQUIP_SLOT_C_DOWN);
                    }
                    else if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) >= ITEM_BOW_FIRE) &&
                        (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) <= ITEM_BOW_LIGHT)) {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT);
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT);
                        Interface_LoadItemIcon(play, EQUIP_SLOT_C_RIGHT);
                    }
                }

                // Equip item on CLeft
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = pauseCtx->equipTargetItem;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = pauseCtx->equipTargetSlot;
                Interface_LoadItemIconImpl(play, EQUIP_SLOT_C_LEFT);
            }
            else if (pauseCtx->equipTargetCBtn == PAUSE_EQUIP_C_DOWN) {
                // Swap if item is already equipped on CLeft or CRight.
                if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) & 0xFF) != ITEM_NONE) {
                        if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                            (((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) & 0xFF) == ITEM_BOW) ||
                                (((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) & 0xFF) >= ITEM_BOW_FIRE) &&
                                    ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                            pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                            pauseCtx->equipTargetSlot = SLOT_BOW;
                        }
                        else {
                            BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN);
                            C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN);
                            Interface_LoadItemIcon(play, EQUIP_SLOT_C_LEFT);
                        }
                    }
                    else {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = ITEM_NONE;
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = SLOT_NONE;
                    }
                }
                else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) & 0xFF) != ITEM_NONE) {
                        if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                            (((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) & 0xFF) == ITEM_BOW) ||
                                (((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) & 0xFF) >= ITEM_BOW_FIRE) &&
                                    ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                            pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                            pauseCtx->equipTargetSlot = SLOT_BOW;
                        }
                        else {
                            BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN);
                            C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN);
                            Interface_LoadItemIcon(play, EQUIP_SLOT_C_RIGHT);
                        }
                    }
                    else {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = ITEM_NONE;
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = SLOT_NONE;
                    }
                }

                // Special case for magic arrows
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) == ITEM_BOW) ||
                        ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) >= ITEM_BOW_FIRE) &&
                            (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) <= ITEM_BOW_LIGHT))) {
                        pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                        pauseCtx->equipTargetSlot = SLOT_BOW;
                    }
                }
                else if (pauseCtx->equipTargetItem == ITEM_BOW) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) >= ITEM_BOW_FIRE) &&
                        (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) <= ITEM_BOW_LIGHT)) {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN);
                        Interface_LoadItemIcon(play, EQUIP_SLOT_C_LEFT);
                    }
                    else if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) >= ITEM_BOW_FIRE) &&
                        (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) <= ITEM_BOW_LIGHT)) {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN);
                        Interface_LoadItemIcon(play, EQUIP_SLOT_C_RIGHT);
                    }
                }

                // Equip item on CDown
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = pauseCtx->equipTargetItem;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = pauseCtx->equipTargetSlot;
                Interface_LoadItemIconImpl(play, EQUIP_SLOT_C_DOWN);
            }
            else { // (pauseCtx->equipTargetCBtn == PAUSE_EQUIP_C_RIGHT)
                // Swap if item is already equipped on CLeft or CDown.
                if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) & 0xFF) != ITEM_NONE) {
                        if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                            (((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) & 0xFF) == ITEM_BOW) ||
                                (((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) & 0xFF) >= ITEM_BOW_FIRE) &&
                                    ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                            pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                            pauseCtx->equipTargetSlot = SLOT_BOW;
                        }
                        else {
                            BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT);
                            C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT);
                            Interface_LoadItemIcon(play, EQUIP_SLOT_C_LEFT);
                        }
                    }
                    else {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = ITEM_NONE;
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = SLOT_NONE;
                    }
                }
                else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) & 0xFF) != ITEM_NONE) {
                        if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                            (((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) & 0xFF) == ITEM_BOW) ||
                                (((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) & 0xFF) >= ITEM_BOW_FIRE) &&
                                    ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                            pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                            pauseCtx->equipTargetSlot = SLOT_BOW;
                        }
                        else {
                            BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT);
                            C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT);
                            Interface_LoadItemIcon(play, EQUIP_SLOT_C_DOWN);
                        }
                    }
                    else {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = ITEM_NONE;
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = SLOT_NONE;
                    }
                }

                // Special case for magic arrows
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) == ITEM_BOW) ||
                        ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) >= ITEM_BOW_FIRE) &&
                            (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) <= ITEM_BOW_LIGHT))) {
                        pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                        pauseCtx->equipTargetSlot = SLOT_BOW;
                    }
                }
                else if (pauseCtx->equipTargetItem == ITEM_BOW) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) >= ITEM_BOW_FIRE) &&
                        (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) <= ITEM_BOW_LIGHT)) {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT);
                        Interface_LoadItemIcon(play, EQUIP_SLOT_C_LEFT);
                    }
                    else if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) >= ITEM_BOW_FIRE) &&
                        (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) <= ITEM_BOW_LIGHT)) {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT);
                        Interface_LoadItemIcon(play, EQUIP_SLOT_C_DOWN);
                    }
                }

                // Equip item on CRight
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = pauseCtx->equipTargetItem;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = pauseCtx->equipTargetSlot;
                Interface_LoadItemIconImpl(play, EQUIP_SLOT_C_RIGHT);
            }

            // Reset params
            pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE;
            sEquipAnimTimer = 10;
            pauseCtx->equipAnimScale = 320;
            pauseCtx->equipAnimShrinkRate = 40;
        }
    }
    else {
        sEquipMagicArrowSlotHoldTimer--;
        if (sEquipMagicArrowSlotHoldTimer == 0) {
            pauseCtx->equipAnimAlpha = 255;
        }
    }
}

// @modern_layout Change position of the equip animation destination.
s16 sMaskCButtonPosX[] = { 350, 550, 1140 };
s16 sMaskCButtonPosY[] = { 950, 1100, 1100 };
extern s16 sMaskEquipState;
extern s16 sMaskEquipMagicArrowSlotHoldTimer;
extern s16 sMaskEquipAnimTimer;

RECOMP_PATCH void KaleidoScope_UpdateMaskEquip(PlayState* play) {
    static s16 sMaskEquipMagicArrowBowSlotHoldTimer = 0;
    PauseContext* pauseCtx = &play->pauseCtx;
    Vtx* bowItemVtx;
    u16 offsetX;
    u16 offsetY;

    // Grow glowing orb when equipping magic arrows
    if (sMaskEquipState == EQUIP_STATE_MAGIC_ARROW_GROW_ORB) {
        pauseCtx->equipAnimAlpha += 14;
        if (pauseCtx->equipAnimAlpha > 255) {
            pauseCtx->equipAnimAlpha = 254;
            sMaskEquipState++;
        }
        // Hover over magic arrow slot when the next state is reached
        sMaskEquipMagicArrowSlotHoldTimer = 5;
        return;
    }

    if (sMaskEquipState == EQUIP_STATE_MAGIC_ARROW_HOVER_OVER_BOW_SLOT) {
        sMaskEquipMagicArrowBowSlotHoldTimer--;

        if (sMaskEquipMagicArrowBowSlotHoldTimer == 0) {
            pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
            pauseCtx->equipTargetSlot = SLOT_BOW;
            sMaskEquipAnimTimer = 6;
            pauseCtx->equipAnimScale = 320;
            pauseCtx->equipAnimShrinkRate = 40;
            sMaskEquipState++;
            Audio_PlaySfx(NA_SE_SY_SYNTH_MAGIC_ARROW);
        }
        return;
    }

    if (sMaskEquipState == EQUIP_STATE_MAGIC_ARROW_MOVE_TO_BOW_SLOT) {
        //! Note: Copied from OoT when `SLOT_BOW` was still valued at 3.
        // Due to a shift, `SLOT_ARROW_ICE` now occupies slot 3 but this value was not updated
        // Block is never reached as you can not equip magic arrows from the mask page
        bowItemVtx = &pauseCtx->itemVtx[SLOT_ARROW_ICE * 4];
        offsetX = ABS_ALT(pauseCtx->equipAnimX - bowItemVtx->v.ob[0] * 10) / sMaskEquipAnimTimer;
        offsetY = ABS_ALT(pauseCtx->equipAnimY - bowItemVtx->v.ob[1] * 10) / sMaskEquipAnimTimer;
    }
    else {
        offsetX = ABS_ALT(pauseCtx->equipAnimX - sMaskCButtonPosX[pauseCtx->equipTargetCBtn]) / sMaskEquipAnimTimer;
        offsetY = ABS_ALT(pauseCtx->equipAnimY - sMaskCButtonPosY[pauseCtx->equipTargetCBtn]) / sMaskEquipAnimTimer;
    }

    if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipAnimAlpha < 254)) {
        pauseCtx->equipAnimAlpha += 14;
        if (pauseCtx->equipAnimAlpha > 255) {
            pauseCtx->equipAnimAlpha = 254;
        }
        sMaskEquipMagicArrowSlotHoldTimer = 5;
        return;
    }

    if (sMaskEquipMagicArrowSlotHoldTimer == 0) {
        pauseCtx->equipAnimScale -= pauseCtx->equipAnimShrinkRate / sMaskEquipAnimTimer;
        pauseCtx->equipAnimShrinkRate -= pauseCtx->equipAnimShrinkRate / sMaskEquipAnimTimer;

        // Update coordinates of item icon while being equipped
        if (sMaskEquipState == EQUIP_STATE_MAGIC_ARROW_MOVE_TO_BOW_SLOT) {
            // target is the bow slot
            if (pauseCtx->equipAnimX >= (pauseCtx->itemVtx[12].v.ob[0] * 10)) {
                pauseCtx->equipAnimX -= offsetX;
            }
            else {
                pauseCtx->equipAnimX += offsetX;
            }

            if (pauseCtx->equipAnimY >= (pauseCtx->itemVtx[12].v.ob[1] * 10)) {
                pauseCtx->equipAnimY -= offsetY;
            }
            else {
                pauseCtx->equipAnimY += offsetY;
            }
        }
        else {
            // target is the c button
            if (pauseCtx->equipAnimX >= sMaskCButtonPosX[pauseCtx->equipTargetCBtn]) {
                pauseCtx->equipAnimX -= offsetX;
            }
            else {
                pauseCtx->equipAnimX += offsetX;
            }

            if (pauseCtx->equipAnimY >= sMaskCButtonPosY[pauseCtx->equipTargetCBtn]) {
                pauseCtx->equipAnimY -= offsetY;
            }
            else {
                pauseCtx->equipAnimY += offsetY;
            }
        }

        sMaskEquipAnimTimer--;
        if (sMaskEquipAnimTimer == 0) {
            if (sMaskEquipState == EQUIP_STATE_MAGIC_ARROW_MOVE_TO_BOW_SLOT) {
                sMaskEquipState++;
                sMaskEquipMagicArrowBowSlotHoldTimer = 4;
                return;
            }

            // Equip mask onto c buttons
            if (pauseCtx->equipTargetCBtn == PAUSE_EQUIP_C_LEFT) {
                // Swap if mask is already equipped on CDown or CRight.
                if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) & 0xFF) != ITEM_NONE) {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT);
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT);
                        Interface_LoadItemIcon(play, EQUIP_SLOT_C_DOWN);
                    }
                    else {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = ITEM_NONE;
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = SLOT_NONE;
                    }
                }
                else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) & 0xFF) != ITEM_NONE) {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT);
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT);
                        Interface_LoadItemIcon(play, EQUIP_SLOT_C_RIGHT);
                    }
                    else {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = ITEM_NONE;
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = SLOT_NONE;
                    }
                }

                // Equip mask on CLeft
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = pauseCtx->equipTargetItem;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = pauseCtx->equipTargetSlot;
                Interface_LoadItemIconImpl(play, EQUIP_SLOT_C_LEFT);
            }
            else if (pauseCtx->equipTargetCBtn == PAUSE_EQUIP_C_DOWN) {
                // Swap if mask is already equipped on CLeft or CRight.
                if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) & 0xFF) != ITEM_NONE) {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN);
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN);
                        Interface_LoadItemIcon(play, EQUIP_SLOT_C_LEFT);
                    }
                    else {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = ITEM_NONE;
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = SLOT_NONE;
                    }
                }
                else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) & 0xFF) != ITEM_NONE) {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN);
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN);
                        Interface_LoadItemIcon(play, EQUIP_SLOT_C_RIGHT);
                    }
                    else {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = ITEM_NONE;
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = SLOT_NONE;
                    }
                }

                // Equip mask on CDown
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = pauseCtx->equipTargetItem;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = pauseCtx->equipTargetSlot;
                Interface_LoadItemIconImpl(play, EQUIP_SLOT_C_DOWN);
            }
            else { // (pauseCtx->equipTargetCBtn == PAUSE_EQUIP_C_RIGHT)
                // Swap if mask is already equipped on CLeft or CDown.
                if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) & 0xFF) != ITEM_NONE) {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT);
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT);
                        Interface_LoadItemIcon(play, EQUIP_SLOT_C_LEFT);
                    }
                    else {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = ITEM_NONE;
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = SLOT_NONE;
                    }
                }
                else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) & 0xFF) != ITEM_NONE) {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT);
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT);
                        Interface_LoadItemIcon(play, EQUIP_SLOT_C_DOWN);
                    }
                    else {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = ITEM_NONE;
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = SLOT_NONE;
                    }
                }

                // Equip mask on CRight
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = pauseCtx->equipTargetItem;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = pauseCtx->equipTargetSlot;
                Interface_LoadItemIconImpl(play, EQUIP_SLOT_C_RIGHT);
            }

            // Reset params
            pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE;
            sMaskEquipAnimTimer = 10;
            pauseCtx->equipAnimScale = 320;
            pauseCtx->equipAnimShrinkRate = 40;
        }
    }
    else {
        sMaskEquipMagicArrowSlotHoldTimer--;
        if (sMaskEquipMagicArrowSlotHoldTimer == 0) {
            pauseCtx->equipAnimAlpha = 255;
        }
    }
}