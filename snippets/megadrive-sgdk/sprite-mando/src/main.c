/* Sprite movido con la cruceta, en punto fijo. SGDK 2.11. */
#include <genesis.h>
#include "resources.h"

#define VELOCIDAD F16(1.5)

int main(bool hard)
{
    (void) hard;
    f16 x = F16(144), y = F16(96);

    VDP_drawText("SNIPPET SPRITE-MANDO", 10, 2);
    PAL_setPalette(PAL1, spr_robot.palette->data, CPU);
    SPR_init();
    Sprite *robot = SPR_addSprite(&spr_robot, F16_toInt(x), F16_toInt(y),
                                  TILE_ATTR(PAL1, TRUE, FALSE, FALSE));

    while (TRUE)
    {
        const u16 mando = JOY_readJoypad(JOY_1);
        if (mando & BUTTON_LEFT)  x -= VELOCIDAD;
        if (mando & BUTTON_RIGHT) x += VELOCIDAD;
        if (mando & BUTTON_UP)    y -= VELOCIDAD;
        if (mando & BUTTON_DOWN)  y += VELOCIDAD;
        SPR_setPosition(robot, F16_toInt(x), F16_toInt(y));
        SPR_update();
        SYS_doVBlankProcess();
    }
    return 0;
}
