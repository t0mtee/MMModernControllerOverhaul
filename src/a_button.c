#include "modding.h"
#include "global.h"

RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));

// Only the Action button uses Interface_SetPerspectiveView. Hook into this, save the PlayState.
extern PlayState* bPlayState;
bool vSPVCount = true;

RECOMP_HOOK("Interface_SetPerspectiveView") void Interface_SetPerspectiveView_Init(PlayState* play, s32 topY, s32 bottomY, s32 leftX, s32 rightX) {
    // After every second function call with a leftX of 190, we want to draw the Action button glyph.
    if (leftX == 190) {
        vSPVCount = !vSPVCount;
    }
}

extern int vShoulderOffset;

RECOMP_HOOK("View_SetViewport") void View_SetViewport_Init(View* view, Viewport* viewport) {
    InterfaceContext* interfaceCtx = &bPlayState->interfaceCtx;

    // If this function is being called for the Action button, move the viewport by the correct values.
    if (&interfaceCtx->view == view && &interfaceCtx->viewport == viewport) {
        switch (recomp_get_config_u32("action_button")) {
            case 0: // Up
                viewport->topY -= 17;
                viewport->bottomY -= 17;
                viewport->leftX += 18;
                viewport->rightX += 18;
                break;
            case 1: // Left
                viewport->topY += 3;
                viewport->bottomY += 3;
                viewport->leftX -= 7;
                viewport->rightX -= 7;
                break;
            case 2: // Down
                viewport->topY += 23;
                viewport->bottomY += 23;
                viewport->leftX += 18;
                viewport->rightX += 18;
                break;
            case 3: // Right
                viewport->topY += 3;
                viewport->bottomY += 3;
                viewport->leftX += 45;
                viewport->rightX += 45;
                break;
            case 4: // Shoulder
                viewport->topY -= 17 - vShoulderOffset;
                viewport->bottomY -= 17 - vShoulderOffset;
                viewport->leftX += 70;
                viewport->rightX += 70;
                break;
        }
    }
}

// Draw Action button glyph.
extern TexturePtr Mod_GlyphTexture(char button[]);

RECOMP_HOOK_RETURN("Interface_SetPerspectiveView") void Interface_SetPerspectiveView_Return() {
    if (vSPVCount == true) {
        InterfaceContext* interfaceCtx = &bPlayState->interfaceCtx;

        if (interfaceCtx->aButtonDoAction == DO_ACTION_NONE && recomp_get_config_u32("glyphs") == 2) {
             OPEN_DISPS(bPlayState->state.gfxCtx);

            gSPSetGeometryMode(OVERLAY_DISP++, G_CULL_BACK);
            gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                            ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
            gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 0);

            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 100, 200, 255, interfaceCtx->aAlpha);
            OVERLAY_DISP = Gfx_DrawTexQuadIA8(OVERLAY_DISP, Mod_GlyphTexture("action_button"), 32, 32, 0);

            CLOSE_DISPS(bPlayState->state.gfxCtx);
        }
    }
}