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


// @modern_layout Draw empty B button icon if it should be drawn
INCBIN(bButtonEmpty, "./textures/b_empty_icon.ia8.bin");
bool bButtonEmptyDrawn = false;

RECOMP_HOOK("Interface_DrawBButtonIcons") void Interface_DrawBButtonIcons_Init(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    OPEN_DISPS(play->state.gfxCtx);

    // @modern_layout If empty B button icon should be drawn, draw it.
    // @modern_layout ITEM_FD because during the first 3-day cycle, Deku Link's B-button is assinged to ITEM_FD instead of ITEM_NONE for some reason
    if ((BUTTON_ITEM_EQUIP(CUR_FORM, EQUIP_SLOT_B) >= ITEM_FD || interfaceCtx->bButtonDoAction == DO_ACTION_NONE) &&
        (&play->pauseCtx)->state == PAUSE_STATE_OFF) {
            bButtonEmptyDrawn = true;
            // @modern_layout Only draw if option enabled
            if (recomp_get_config_u32("icon_addition") == 1) {
                gDPPipeSync(OVERLAY_DISP++);
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 100, 255, 120, interfaceCtx->bAlpha);
                OVERLAY_DISP = Gfx_DrawTexRectIA8(OVERLAY_DISP, bButtonEmpty,
                    0x20, 0x20, 218, 55, 0x1D,
                    0x1D, ((s32)(1.1230469f * (1 << 10)) >> 1) * 2, ((s32)(1.1230469f * (1 << 10)) >> 1) * 2);
            }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

// @modern_layout To keep the buttons consistent, if there is no item in the B-button, keep the alpha at max.
PlayState* play_save;

RECOMP_HOOK("Interface_UpdateButtonAlphasByStatus") void Interface_UpdateButtonAlphasByStatus_Init(PlayState* play, s16 risingAlpha) {
    play_save = play;
}

RECOMP_HOOK_RETURN("Interface_UpdateButtonAlphasByStatus") void Interface_UpdateButtonAlphasByStatus_Return() {
    // @modern_layout We know if there is no item in B-button because otherwise we wouldn't have drawn the empty B-button icon
    if (bButtonEmptyDrawn == true) {
        (&play_save->interfaceCtx)->bAlpha = 255;
        bButtonEmptyDrawn = false;
    }
}