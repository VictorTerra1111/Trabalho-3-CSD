#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <hf-risc.h>
#include "vga_drv.h"

struct object_s {
    char *sprite_frame[3];
    char spriteszx, spriteszy, sprites;
    int cursprite;
    unsigned int posx, posy;
    int dx, dy;
    int speedx, speedy;
    int speedxcnt, speedycnt;
};

enum {
    KEY_CENTER = 0x01,
    KEY_UP     = 0x02,
    KEY_LEFT   = 0x04,
    KEY_RIGHT  = 0x08,
    KEY_DOWN   = 0x10
};

void draw_sprite(unsigned int x, unsigned int y, char *sprite,
                 unsigned int sizex, unsigned int sizey, int color);

void init_object(struct object_s *obj, char *spritea, char *spriteb,
                 char *spritec, char spriteszx, char spriteszy,
                 int posx, int posy, int dx, int dy, int spx, int spy);

void draw_object(struct object_s *obj, char chgsprite, int color);

void move_object(struct object_s *obj);

void init_display(void);

void init_input(void);

int get_input(void);

int detect_collision(struct object_s *obj1, struct object_s *obj2);

void control_player(struct object_s *player);

void int_to_string(int value, char *str);
#endif
