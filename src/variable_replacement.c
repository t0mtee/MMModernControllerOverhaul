#include "modding.h"
#include "global.h"

// @modern_layout Change all variables that can be easily changed. Hooked if variables aren't exposed during the init function, callbacks if they are.
extern s16 sMaskCButtonPosX[3];
extern s16 sMaskCButtonPosY[3];

RECOMP_HOOK("KaleidoScope_UpdateMaskEquip") void KaleidoScope_UpdateMaskEquip_Init(PlayState* play) {
    // @modern_layout Change position of the equip animation destination.
    sMaskCButtonPosX[0] = 350;
    sMaskCButtonPosX[1] = 550;

    sMaskCButtonPosY[0] = 950;
    sMaskCButtonPosY[1] = 1110;
}

extern s16 sCButtonPosX[3];
extern s16 sCButtonPosY[3];

RECOMP_HOOK("KaleidoScope_UpdateItemEquip") void KaleidoScope_UpdateItemEquip_Init(PlayState* play) {
    // @modern_layout Change equip animation destination
    sCButtonPosX[0] = 350;
    sCButtonPosX[1] = 550;

    sCButtonPosY[0] = 950;
    sCButtonPosY[0] = 950;
    sCButtonPosY[1] = 1100;
}

extern s16 D_801BF9C4[2];
extern s16 D_801BF9C8[2];

extern s16 D_801BFB04[4];
extern s16 D_801BFB0C[4];

extern s16 D_801BF9D4[4];
extern s16 D_801BF9DC[4];

extern s16 D_801BF9BC[4];

RECOMP_CALLBACK("*", recomp_after_play_init) void after_play_init() {
    // @modern_layout Move B button icons
    D_801BF9C4[0] = 209;
    D_801BF9C4[1] = 206;

    D_801BF9C8[0] = 61;
    D_801BF9C8[1] = 60;

    // @modern_layout Move ammo UI
    // Left distance
    D_801BFB04[0] = 220;    // EQUIP_SLOT_B
    D_801BFB04[1] = 195;    // EQUIP_SLOT_C_LEFT
    D_801BFB04[2] = 220;    // EQUIP_SLOT_C_DOWN

    // Down distance
    D_801BFB0C[0] = 74;     // EQUIP_SLOT_B
    D_801BFB0C[1] = 52;     // EQUIP_SLOT_C_LEFT
    D_801BFB0C[2] = 32;     // EQUIP_SLOT_C_DOWN

    // @modern_layout Move item buttons
    // Left distance
    D_801BF9D4[0] = 218;    // EQUIP_SLOT_B
    D_801BF9D4[1] = 194;    // EQUIP_SLOT_C_LEFT
    D_801BF9D4[2] = 219;    // EQUIP_SLOT_C_DOWN

    // Down distance
    D_801BF9DC[0] = 55;     // EQUIP_SLOT_B
    D_801BF9DC[1] = 35;     // EQUIP_SLOT_C_LEFT
    D_801BF9DC[2] = 15;     // EQUIP_SLOT_C_DOWN

    // @modern_layout Make B-button item icons similar size relative to button
    D_801BF9BC[0] = 0x26C;
}

RECOMP_HOOK("Interface_DrawItemIconTexture") void Interface_DrawItemIconTexture_Init(PlayState* play, TexturePtr texture, s16 button) {
    // @modern_layout Make B-button item icons line up better with C-down
    // Left distance
    D_801BF9D4[0] = 218;    // EQUIP_SLOT_B

    // Down distance
    D_801BF9DC[0] = 55;     // EQUIP_SLOT_B
}

RECOMP_HOOK_RETURN("Interface_DrawItemIconTexture") void Interface_DrawItemIconTexture_Return() {
    // @modern_layout Revert changes so the button itself draws correctly
    // Left distance
    D_801BF9D4[0] = 218;    // EQUIP_SLOT_B

    // Down distance
    D_801BF9DC[0] = 55;     // EQUIP_SLOT_B
}

// @modern_layout Fix a vanilla bug. gPlayerFormItemRestrictions has 114 allocated positions, however the 255th position is requested
// @modern_layout when assessing C-buttons with no items.
// @modern_layout This leads to unpredictable behaviour, and Deku Link alone will have greyed out empty C-buttons.
// @modern_layout To fix this without causing any potential problems, save the value at the bugged index, replace it to true, then revert it.
u8 gPlayerFormItemRestrictions_save;
extern u8 gPlayerFormItemRestrictions[][114];

RECOMP_HOOK("Interface_UpdateButtonsPart2") void Interface_UpdateButtonsPart2_Init(PlayState* play) {
    gPlayerFormItemRestrictions_save = gPlayerFormItemRestrictions[PLAYER_FORM_DEKU][255];
    gPlayerFormItemRestrictions[PLAYER_FORM_DEKU][255] = true;
}

RECOMP_HOOK_RETURN("Interface_UpdateButtonsPart2") void Interface_UpdateButtonsPart2_Return() {
    gPlayerFormItemRestrictions[PLAYER_FORM_DEKU][255] = gPlayerFormItemRestrictions_save;
}