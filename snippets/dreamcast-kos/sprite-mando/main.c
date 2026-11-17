/* Sprite con transparencia en el PVR, movido con la cruceta. KOS de DreamSDK R4. */
#include <kos.h>
#include "sprite_tex.h"

#define ESCALA 3.0f
#define VELOCIDAD 2.0f

KOS_INIT_FLAGS(INIT_DEFAULT);

static void vertice(pvr_vertex_t *v, uint32_t flags, float x, float y, float u, float t)
{
    v->flags = flags;
    v->x = x; v->y = y; v->z = 1.0f;
    v->u = u; v->v = t;
    v->argb = 0xFFFFFFFF;
    v->oargb = 0;
    pvr_prim(v, sizeof(*v));
}

int main(int argc, char **argv)
{
    (void) argc; (void) argv;
    pvr_init_defaults();
    pvr_set_bg_color(0.1f, 0.1f, 0.3f);

    pvr_ptr_t vram = pvr_mem_malloc(sizeof(robot));
    pvr_txr_load((void *) robot, vram, sizeof(robot));

    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB4444 | PVR_TXRFMT_NONTWIDDLED,
                     ROBOT_W, ROBOT_H, vram, PVR_FILTER_NONE);
    pvr_poly_compile(&hdr, &cxt);

    const float w = ROBOT_W * ESCALA, h = ROBOT_H * ESCALA;
    float x = 320.0f - w / 2, y = 240.0f - h / 2;

    for (;;)
    {
        maple_device_t *dev = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
        if (dev)
        {
            cont_state_t *mando = (cont_state_t *) maple_dev_status(dev);
            if (mando)
            {
                if (mando->buttons & CONT_DPAD_LEFT)  x -= VELOCIDAD;
                if (mando->buttons & CONT_DPAD_RIGHT) x += VELOCIDAD;
                if (mando->buttons & CONT_DPAD_UP)    y -= VELOCIDAD;
                if (mando->buttons & CONT_DPAD_DOWN)  y += VELOCIDAD;
            }
        }

        pvr_wait_ready();
        pvr_scene_begin();
        pvr_list_begin(PVR_LIST_TR_POLY);
        pvr_prim(&hdr, sizeof(hdr));
        pvr_vertex_t v;
        vertice(&v, PVR_CMD_VERTEX, x, y + h, 0.0f, 1.0f);
        vertice(&v, PVR_CMD_VERTEX, x, y, 0.0f, 0.0f);
        vertice(&v, PVR_CMD_VERTEX, x + w, y + h, 1.0f, 1.0f);
        vertice(&v, PVR_CMD_VERTEX_EOL, x + w, y, 1.0f, 0.0f);
        pvr_list_finish();
        pvr_scene_finish();
    }
    return 0;
}
