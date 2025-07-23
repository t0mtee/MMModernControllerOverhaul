#include "interface_helper.h"
#include "libc64/sprintf.h"

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

// Function is used to determine what the correct glyph texture is.
extern TexturePtr gButtonBackgroundTex[];

INCBIN(tBGlyph, "./textures/ia8/b_empty_icon.ia8.bin");
INCBIN(tAGlyph, "./textures/ia8/a_empty_icon.ia8.bin");

TexturePtr Mod_GlyphTexture(const char button[]) {
    s32 config = recomp_get_config_u32(button);
    
    // If glyphs aren't fully disabled.
    if (recomp_get_config_u32("glyphs") != 0) {
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

extern s16 sMaskCButtonPosX[3];
extern s16 sMaskCButtonPosY[3];
extern s16 sCButtonPosX[3];
extern s16 sCButtonPosY[3];
extern s16 sBButtonDoActionXPositions[2];
extern s16 sBButtonDoActionYPositions[2];

AMMO_POSITIONS_TENS_X_DECLARE
AMMO_POSITIONS_TENS_Y_DECLARE
AMMO_POSITIONS_ONES_X_DECLARE
AMMO_POSITIONS_ONES_Y_DECLARE
BUTTON_POSITIONS_X_DECLARE
BUTTON_POSITIONS_Y_DECLARE
C_GLYPH_TEXTURES_DECLARE
C_GLYPH_POSITIONS_X_DECLARE
C_GLYPH_POSITIONS_Y_DECLARE
START_BUTTON_ENABLED_DECLARE
START_LABEL_ENABLED_DECLARE
C_UP_BUTTON_POSITION_X_DECLARE
C_UP_LABEL_POSITION_X_DECLARE
B_BUTTON_DRAWN_DECLARE
ITEM_ICON_POSITIONS_X_DECLARE
ITEM_ICON_POSITIONS_Y_DECLARE

RECOMP_CALLBACK("*", recomp_after_play_init) void after_play_init() {
    AMMO_POSITIONS_TENS_X_REGISTER
    AMMO_POSITIONS_TENS_Y_REGISTER
    AMMO_POSITIONS_ONES_X_REGISTER
    AMMO_POSITIONS_ONES_Y_REGISTER
    BUTTON_POSITIONS_X_REGISTER
    BUTTON_POSITIONS_Y_REGISTER
    C_GLYPH_TEXTURES_REGISTER
    C_GLYPH_POSITIONS_X_REGISTER
    C_GLYPH_POSITIONS_Y_REGISTER
    START_BUTTON_ENABLED_REGISTER
    START_LABEL_ENABLED_REGISTER
    C_UP_BUTTON_POSITION_X_REGISTER
    C_UP_LABEL_POSITION_X_REGISTER
    B_BUTTON_DRAWN_REGISTER
    ITEM_ICON_POSITIONS_X_REGISTER
    ITEM_ICON_POSITIONS_Y_REGISTER
    
    (*pStartButtonEnabled) = false;
    (*pStartLabelEnabled) = false;

    (*pCUpButtonPositionX) = 251;
    (*pCUpLabelPositionX) = 965;
}

#define C_UP(button)    (*pAmmoPositionsTensX)[button + 1] = 220;    /* Ammo Left */     \
                        (*pAmmoPositionsTensY)[button + 1] = 32;     /* Ammo Down */     \
                        (*pButtonPositionsX)[button + 1] = 219;      /* Button Left */   \
                        (*pButtonPositionsY)[button + 1] = 15;       /* Button Down */   \
                        sMaskCButtonPosX[button] = 550;             /* Mask Equip X */  \
                        sMaskCButtonPosY[button] = 1150;            /* Mask Equip Y */  \
                        sCButtonPosX[button] = 550;                 /* Item Equip X */  \
                        sCButtonPosY[button] = 1150;                /* Item Equip Y */  \

#define C_LEFT(button)  (*pAmmoPositionsTensX)[button + 1] = 195;    /* Ammo Left */     \
                        (*pAmmoPositionsTensY)[button + 1] = 52;     /* Ammo Down */     \
                        (*pButtonPositionsX)[button + 1] = 194;      /* Button Left */   \
                        (*pButtonPositionsY)[button + 1] = 35;       /* Button Down */   \
                        sMaskCButtonPosX[button] = 350;             /* Mask Equip X */  \
                        sMaskCButtonPosY[button] = 950;             /* Mask Equip Y */  \
                        sCButtonPosX[button] = 350;                 /* Item Equip X */  \
                        sCButtonPosY[button] = 950;                 /* Item Equip Y */  \

#define C_DOWN(button)  (*pAmmoPositionsTensX)[button + 1] = 220;    /* Ammo Left */     \
                        (*pAmmoPositionsTensY)[button + 1] = 72;     /* Ammo Down */     \
                        (*pButtonPositionsX)[button + 1] = 219;      /* Button Left */   \
                        (*pButtonPositionsY)[button + 1] = 55;       /* Button Down */   \
                        sMaskCButtonPosX[button] = 550;             /* Mask Equip X */  \
                        sMaskCButtonPosY[button] = 700;             /* Mask Equip Y */  \
                        sCButtonPosX[button] = 550;                 /* Item Equip X */  \
                        sCButtonPosY[button] = 700;                 /* Item Equip Y */  \

#define C_RIGHT(button) (*pAmmoPositionsTensX)[button + 1] = 247;    /* Ammo Left */     \
                        (*pAmmoPositionsTensY)[button + 1] = 52;     /* Ammo Down */     \
                        (*pButtonPositionsX)[button + 1] = 246;      /* Button Left */   \
                        (*pButtonPositionsY)[button + 1] = 35;       /* Button Down */   \
                        sMaskCButtonPosX[button] = 815;             /* Mask Equip X */  \
                        sMaskCButtonPosY[button] = 950;             /* Mask Equip Y */  \
                        sCButtonPosX[button] = 815;                 /* Item Equip X */  \
                        sCButtonPosY[button] = 950;                 /* Item Equip Y */  \

#define C_SHOULDER(button)  (*pAmmoPositionsTensX)[button + 1] = 272;                    /* Ammo Left */     \
                            (*pAmmoPositionsTensY)[button + 1] = 32 + vShoulderOffset;   /* Ammo Down */     \
                            (*pButtonPositionsX)[button + 1] = 271;                      /* Button Left */   \
                            (*pButtonPositionsY)[button + 1] = 15 + vShoulderOffset;     /* Button Down */   \
                            sMaskCButtonPosX[button] = 1140;                            /* Mask Equip X */  \
                            sMaskCButtonPosY[button] = 1150 - vShoulderEquipOffset;     /* Mask Equip Y */  \
                            sCButtonPosX[button] = 1140;                                /* Item Equip X */  \
                            sCButtonPosY[button] = 1150 - vShoulderEquipOffset;         /* Item Equip Y */  \

const char BUTTON_CONFIGS[3][9] = {"1_button", "2_button", "3_button"};
int vShoulderOffset;

// Set here so that config changes have immediate effect.
RECOMP_CALLBACK("*", recomp_on_play_main) void on_play_main() {
    // Calculate the offset that should be applied to the shoulder Y position based on user's config.
    vShoulderOffset = (recomp_get_config_u32("shoulder_position") - 1) * -3;
    int vShoulderEquipOffset = vShoulderOffset * 16;
    
    switch (recomp_get_config_u32("attack_button")) {
        case 0: // Up
            (*pAmmoPositionsTensX)[0] = 220;         // Ammo Left
            (*pAmmoPositionsTensY)[0] = 33;          // Ammo Down
            sBButtonDoActionXPositions[0] = 209;    // Action JPN X
            sBButtonDoActionXPositions[1] = 206;    // Action ENG X
            sBButtonDoActionYPositions[0] = 20;     // Action JPN Y
            sBButtonDoActionYPositions[1] = 19;     // Action ENG Y
            (*pButtonPositionsX)[0] = 218;           // Button Left
            (*pButtonPositionsY)[0] = 14;            // Button Down
            break;
        case 1: // Left
            (*pAmmoPositionsTensX)[0] = 195;         // Ammo Left
            (*pAmmoPositionsTensY)[0] = 53;          // Ammo Down
            sBButtonDoActionXPositions[0] = 184;    // Action JPN X
            sBButtonDoActionXPositions[1] = 181;    // Action ENG X
            sBButtonDoActionYPositions[0] = 40;     // Action JPN Y
            sBButtonDoActionYPositions[1] = 39;     // Action ENG Y
            (*pButtonPositionsX)[0] = 193;           // Button Left
            (*pButtonPositionsY)[0] = 34;            // Button Down
            break;
        case 2: // Down
            (*pAmmoPositionsTensX)[0] = 220;         // Ammo Left
            (*pAmmoPositionsTensY)[0] = 73;          // Ammo Down
            sBButtonDoActionXPositions[0] = 209;    // Action JPN X
            sBButtonDoActionXPositions[1] = 206;    // Action ENG X
            sBButtonDoActionYPositions[0] = 60;     // Action JPN Y
            sBButtonDoActionYPositions[1] = 59;     // Action ENG Y
            (*pButtonPositionsX)[0] = 218;           // Button Left
            (*pButtonPositionsY)[0] = 54;            // Button Down
            break;
        case 3: // Right
            (*pAmmoPositionsTensX)[0] = 245;         // Ammo Left
            (*pAmmoPositionsTensY)[0] = 53;          // Ammo Down
            sBButtonDoActionXPositions[0] = 236;    // Action JPN X
            sBButtonDoActionXPositions[1] = 233;    // Action ENG X
            sBButtonDoActionYPositions[0] = 40;     // Action JPN Y
            sBButtonDoActionYPositions[1] = 39;     // Action ENG Y
            (*pButtonPositionsX)[0] = 245;           // Button Left
            (*pButtonPositionsY)[0] = 34;            // Button Down
            break;
        case 4: // Shoulder
            (*pAmmoPositionsTensX)[0] = 272;                         // Ammo Left
            (*pAmmoPositionsTensY)[0] = 33 + vShoulderOffset;        // Ammo Down
            sBButtonDoActionXPositions[0] = 261;                    // Action JPN X
            sBButtonDoActionXPositions[1] = 258;                    // Action ENG X
            sBButtonDoActionYPositions[0] = 20 + vShoulderOffset;   // Action JPN Y
            sBButtonDoActionYPositions[1] = 19 + vShoulderOffset;   // Action ENG Y
            (*pButtonPositionsX)[0] = 270;                           // Button Left
            (*pButtonPositionsY)[0] = 14 + vShoulderOffset;          // Button Down
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

    for (int index = 0; index <= 3; index++ ) {
        // Set ammo one positions to be next to ammo tens
        (*pAmmoPositionsOnesX)[index] = (*pAmmoPositionsTensX)[index] + 6;
        (*pAmmoPositionsOnesY)[index] = (*pAmmoPositionsTensY)[index];
        
        (*pItemIconPositionsX)[index] = (*pButtonPositionsX)[index] << 2;
        (*pItemIconPositionsY)[index] = (*pButtonPositionsY)[index] << 2;
        
        // C-button only shenanigans
        if (index != 0) {
            int cIndex = index - 1;
            
            (*pCGlyphTextures)[cIndex] = Mod_GlyphTexture(BUTTON_CONFIGS[cIndex]);
            
            (*pCGlyphPositionsX)[cIndex] = (*pButtonPositionsX)[index];
            (*pCGlyphPositionsY)[cIndex] = (*pButtonPositionsY)[index];
        }
    }
}

RECOMP_HOOK("Interface_DrawCButtonIcons") void Interface_DrawCButtonIcons_Init(PlayState* play) {
    // Makes sure the Action button's offset is always zero - it doesn't need to be changed with this mod.
    R_A_BTN_Y_OFFSET = 0;

    if (*pBButtonDrawn == false && recomp_get_config_u32("glyphs") == 2) {
        OPEN_DISPS(play->state.gfxCtx);
        
        // Draw Attack button glyph.
        gDPPipeSync(OVERLAY_DISP++);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 100, 255, 120, (&play->interfaceCtx)->bAlpha);
        OVERLAY_DISP = Gfx_DrawTexRectIA8(OVERLAY_DISP, Mod_GlyphTexture("attack_button"),
            0x20, 0x20, (*pButtonPositionsX)[0], (*pButtonPositionsY)[0], 0x1D,
            0x1D, ((s32)(1.1230469f * (1 << 10)) >> 1) * 2, ((s32)(1.1230469f * (1 << 10)) >> 1) * 2);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}