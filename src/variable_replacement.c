#include "modding.h"
#include "global.h"

RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));

Gfx to_compare[3];  // Texrects are 3 commands.

Gfx mSetCombineMode[1];
Gfx* mSetCombineMode_ptr = mSetCombineMode;

RECOMP_CALLBACK("*", recomp_after_play_init) void after_play_init() {

    // Build a dummy DisplayList to compare against to find the vanilla C-Up texrect command. Mimic the original
    // command so it's bytewise identical.
    Gfx* to_compare_ptr = to_compare;
    gSPTextureRectangle(to_compare_ptr++, 247 << 2, 18 << 2, (247 + 32) << 2, (18 + 12) << 2, G_TX_RENDERTILE, 0,
                        0, 1 << 10, 1 << 10);

    // Build a dummy DisplayList for if Interface_DrawBButtonIcons was called but nothing was drawn.
    gDPSetCombineMode(mSetCombineMode_ptr, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
}

extern s16 sBButtonDoActionXPositions[2];
extern s16 sBButtonDoActionYPositions[2];

extern s16 sBCButtonXPositions[4];
extern s16 sBCButtonYPositions[4];

extern s16 sMaskCButtonPosX[3];
extern s16 sMaskCButtonPosY[3];

extern s16 sCButtonPosX[3];
extern s16 sCButtonPosY[3];

extern s16 sAmmoDigitsXPositions[4];
extern s16 sAmmoDigitsYPositions[4];

#define C_UP(button)    sAmmoDigitsXPositions[button + 1] = 220;    /* Ammo Left */     \
                        sAmmoDigitsYPositions[button + 1] = 32;     /* Ammo Down */     \
                        sBCButtonXPositions[button + 1] = 219;      /* Button Left */   \
                        sBCButtonYPositions[button + 1] = 15;       /* Button Down */   \
                        sMaskCButtonPosX[button] = 550;             /* Mask Equip X */  \
                        sMaskCButtonPosY[button] = 1150;            /* Mask Equip Y */  \
                        sCButtonPosX[button] = 550;                 /* Item Equip X */  \
                        sCButtonPosY[button] = 1150;                /* Item Equip Y */  \

#define C_LEFT(button)  sAmmoDigitsXPositions[button + 1] = 195;    /* Ammo Left */     \
                        sAmmoDigitsYPositions[button + 1] = 52;     /* Ammo Down */     \
                        sBCButtonXPositions[button + 1] = 194;      /* Button Left */   \
                        sBCButtonYPositions[button + 1] = 35;       /* Button Down */   \
                        sMaskCButtonPosX[button] = 350;             /* Mask Equip X */  \
                        sMaskCButtonPosY[button] = 950;             /* Mask Equip Y */  \
                        sCButtonPosX[button] = 350;                 /* Item Equip X */  \
                        sCButtonPosY[button] = 950;                 /* Item Equip Y */  \

#define C_DOWN(button)  sAmmoDigitsXPositions[button + 1] = 220;    /* Ammo Left */     \
                        sAmmoDigitsYPositions[button + 1] = 72;     /* Ammo Down */     \
                        sBCButtonXPositions[button + 1] = 219;      /* Button Left */   \
                        sBCButtonYPositions[button + 1] = 55;       /* Button Down */   \
                        sMaskCButtonPosX[button] = 550;             /* Mask Equip X */  \
                        sMaskCButtonPosY[button] = 700;             /* Mask Equip Y */  \
                        sCButtonPosX[button] = 550;                 /* Item Equip X */  \
                        sCButtonPosY[button] = 700;                 /* Item Equip Y */  \

#define C_RIGHT(button) sAmmoDigitsXPositions[button + 1] = 247;    /* Ammo Left */     \
                        sAmmoDigitsYPositions[button + 1] = 52;     /* Ammo Down */     \
                        sBCButtonXPositions[button + 1] = 246;      /* Button Left */   \
                        sBCButtonYPositions[button + 1] = 35;       /* Button Down */   \
                        sMaskCButtonPosX[button] = 815;             /* Mask Equip X */  \
                        sMaskCButtonPosY[button] = 950;             /* Mask Equip Y */  \
                        sCButtonPosX[button] = 815;                 /* Item Equip X */  \
                        sCButtonPosY[button] = 950;                 /* Item Equip Y */  \

#define C_SHOULDER(button)  sAmmoDigitsXPositions[button + 1] = 272;                    /* Ammo Left */     \
                            sAmmoDigitsYPositions[button + 1] = 32 + vShoulderOffset;   /* Ammo Down */     \
                            sBCButtonXPositions[button + 1] = 271;                      /* Button Left */   \
                            sBCButtonYPositions[button + 1] = 15 + vShoulderOffset;     /* Button Down */   \
                            sMaskCButtonPosX[button] = 1140;                            /* Mask Equip X */  \
                            sMaskCButtonPosY[button] = 1150 - vShoulderEquipOffset;     /* Mask Equip Y */  \
                            sCButtonPosX[button] = 1140;                                /* Item Equip X */  \
                            sCButtonPosY[button] = 1150 - vShoulderEquipOffset;         /* Item Equip Y */  \

int vShoulderOffset;
extern s16 sItemIconTextureScales[4];

// Set here so that config changes have immediate effect.
RECOMP_CALLBACK("*", recomp_on_play_main) void on_play_main() {
    if (recomp_get_config_u32("attack_item_scale")) {
        // Make Attack button item icons similar size relative to button.
        sItemIconTextureScales[0] = 0x26C;
    } else {
        sItemIconTextureScales[0] = (s16)(1.074219f * (1 << 10)) >> 1;
    }

    // Calculate the offset that should be applied to the shoulder Y position based on user's config.
    vShoulderOffset = (recomp_get_config_u32("shoulder_position") - 1) * -3;
    int vShoulderEquipOffset = vShoulderOffset * 16;

    switch (recomp_get_config_u32("attack_button")) {
        case 0: // Up
            sAmmoDigitsXPositions[0] = 220;         // Ammo Left
            sAmmoDigitsYPositions[0] = 33;          // Ammo Down
            sBButtonDoActionXPositions[0] = 209;    // Action JPN X
            sBButtonDoActionXPositions[1] = 206;    // Action ENG X
            sBButtonDoActionYPositions[0] = 20;     // Action JPN Y
            sBButtonDoActionYPositions[1] = 19;     // Action ENG Y
            sBCButtonXPositions[0] = 218;           // Button Left
            sBCButtonYPositions[0] = 14;            // Button Down
            break;
        case 1: // Left
            sAmmoDigitsXPositions[0] = 195;         // Ammo Left
            sAmmoDigitsYPositions[0] = 53;          // Ammo Down
            sBButtonDoActionXPositions[0] = 184;    // Action JPN X
            sBButtonDoActionXPositions[1] = 181;    // Action ENG X
            sBButtonDoActionYPositions[0] = 40;     // Action JPN Y
            sBButtonDoActionYPositions[1] = 39;     // Action ENG Y
            sBCButtonXPositions[0] = 193;           // Button Left
            sBCButtonYPositions[0] = 34;            // Button Down
            break;
        case 2: // Down
            sAmmoDigitsXPositions[0] = 220;         // Ammo Left
            sAmmoDigitsYPositions[0] = 73;          // Ammo Down
            sBButtonDoActionXPositions[0] = 209;    // Action JPN X
            sBButtonDoActionXPositions[1] = 206;    // Action ENG X
            sBButtonDoActionYPositions[0] = 60;     // Action JPN Y
            sBButtonDoActionYPositions[1] = 59;     // Action ENG Y
            sBCButtonXPositions[0] = 218;           // Button Left
            sBCButtonYPositions[0] = 54;            // Button Down
            break;
        case 3: // Right
            sAmmoDigitsXPositions[0] = 245;         // Ammo Left
            sAmmoDigitsYPositions[0] = 53;          // Ammo Down
            sBButtonDoActionXPositions[0] = 236;    // Action JPN X
            sBButtonDoActionXPositions[1] = 233;    // Action ENG X
            sBButtonDoActionYPositions[0] = 40;     // Action JPN Y
            sBButtonDoActionYPositions[1] = 39;     // Action ENG Y
            sBCButtonXPositions[0] = 245;           // Button Left
            sBCButtonYPositions[0] = 34;            // Button Down
            break;
        case 4: // Shoulder
            sAmmoDigitsXPositions[0] = 272;                         // Ammo Left
            sAmmoDigitsYPositions[0] = 33 + vShoulderOffset;        // Ammo Down
            sBButtonDoActionXPositions[0] = 261;                    // Action JPN X
            sBButtonDoActionXPositions[1] = 258;                    // Action ENG X
            sBButtonDoActionYPositions[0] = 20 + vShoulderOffset;   // Action JPN Y
            sBButtonDoActionYPositions[1] = 19 + vShoulderOffset;   // Action ENG Y
            sBCButtonXPositions[0] = 270;                           // Button Left
            sBCButtonYPositions[0] = 14 + vShoulderOffset;          // Button Down
            break;
    }

    switch (recomp_get_config_u32("1_button")) {
        case 0: // Up
            C_UP(0)
            break;
        case 1: // Left
            C_LEFT(0)
            break;
        case 2: // Down
            C_DOWN(0)
            break;
        case 3: // Right
            C_RIGHT(0)
            break;
        case 4: // Shoulder
            C_SHOULDER(0)
            break;
    }
    
    switch (recomp_get_config_u32("2_button")) {
        case 0: // Up
            C_UP(1)
            break;
        case 1: // Left
            C_LEFT(1)
            break;
        case 2: // Down
            C_DOWN(1)
            break;
        case 3: // Right
            C_RIGHT(1)
            break;
        case 4: // Shoulder
            C_SHOULDER(1)
            break;
    }
    
    switch (recomp_get_config_u32("3_button")) {
        case 0: // Up
            C_UP(2)
            break;
        case 1: // Left
            C_LEFT(2)
            break;
        case 2: // Down
            C_DOWN(2)
            break;
        case 3: // Right
            C_RIGHT(2)
            break;
        case 4: // Shoulder
            C_SHOULDER(2)
            break;
    }
}

extern u8 gPlayerFormItemRestrictions[][114];
u8 bBadIndex;

RECOMP_HOOK("Interface_UpdateButtonsPart2") void Interface_UpdateButtonsPart2_Init(PlayState* play) {
    // Fix a vanilla bug. gPlayerFormItemRestrictions has 114 allocated positions, however the 255th position is requested
    // when assessing C-buttons with no items.
    // This leads to unpredictable behaviour, and Deku Link alone will have greyed out empty C-buttons.
    // To fix this without causing any potential problems, save the value at the bugged index, replace it to true, then revert it.
    bBadIndex = gPlayerFormItemRestrictions[PLAYER_FORM_DEKU][255];
    gPlayerFormItemRestrictions[PLAYER_FORM_DEKU][255] = true;

    // Keep buttons consistent by not disabling the Attack button when its empty.
    // Using FE as it is unused in the rest of the game.
    if (BUTTON_ITEM_EQUIP(CUR_FORM, EQUIP_SLOT_B) == ITEM_NONE) {
        BUTTON_ITEM_EQUIP(CUR_FORM, EQUIP_SLOT_B) = ITEM_FE;
    }
}

RECOMP_HOOK_RETURN("Interface_UpdateButtonsPart2") void Interface_UpdateButtonsPart2_Return() {
    // Revert prior changes.
    gPlayerFormItemRestrictions[PLAYER_FORM_DEKU][255] = bBadIndex;

    if (BUTTON_ITEM_EQUIP(CUR_FORM, EQUIP_SLOT_B) == ITEM_FE) {
        BUTTON_ITEM_EQUIP(CUR_FORM, EQUIP_SLOT_B) = ITEM_NONE;
    }
}