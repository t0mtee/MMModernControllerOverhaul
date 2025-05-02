#include "modding.h"
#include "global.h"

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

// @modern_layout Only the A-button uses Interface_SetPerspectiveView. Hook into this, save the PlayState...
PlayState* bPlayState_a;

RECOMP_HOOK("Interface_SetPerspectiveView") void Interface_SetPerspectiveView_Init(PlayState* play, s32 topY, s32 bottomY, s32 leftX, s32 rightX) {
    bPlayState_a = play;
}

RECOMP_HOOK("View_SetViewport") void View_SetViewport_Init(View* view, Viewport* viewport) {
    InterfaceContext* interfaceCtx = &bPlayState_a->interfaceCtx;

    // @modern_layout If this function is being called for the A button, move the viewport by the correct values.
    if (&interfaceCtx->view == view && &interfaceCtx->viewport == viewport) {
        viewport->topY += 3;
        viewport->bottomY += 3;
        viewport->leftX += 45;
        viewport->rightX += 45;
    }
}

// @modern_layout Functionality for drawing the empty A-button icon
Gfx* bGfx_a;
TexturePtr bTexturePtr;

RECOMP_HOOK("Gfx_DrawTexQuadIA8") void Gfx_DrawTexQuadIA8_Init(Gfx* gfx, TexturePtr texture, s16 textureWidth, s16 textureHeight, u16 point) {
    bGfx_a = gfx;
    bTexturePtr = texture;
}

extern TexturePtr gButtonBackgroundTex[];
INCBIN(tAButtonEmpty, "./textures/a_icon.ia8.bin");

RECOMP_HOOK_RETURN("Gfx_DrawTexQuadIA8") void Gfx_DrawTexQuadIA8_Return() {
    // @modern_layout Draw the empty A button icon on top of the other one if it should be drawn...
    if ((&bPlayState_a->interfaceCtx) -> aButtonDoAction == DO_ACTION_NONE && recomp_get_config_u32("icon_addition") == 1 &&
    // @modern_layout and if the function is being called for the A button.
        bTexturePtr == gButtonBackgroundTex) {
        gDPLoadTextureBlock(bGfx_a++, tAButtonEmpty, G_IM_FMT_IA, G_IM_SIZ_8b, 32, 32, 0,
            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
            G_TX_NOLOD);

        gSP1Quadrangle(bGfx_a++, 0, 2, 3, 1, 0);
    }
}