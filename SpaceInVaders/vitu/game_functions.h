#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <hf-risc.h>
#include "vga_drv.h"

#define MAX_ENEMY_BULLETS 6
/* Pequeno LCG local para seleção de coluna aleatória */
static unsigned int __rnd_state_gc = 0x12345678u;
static unsigned int rnd32_gc(void) {
	__rnd_state_gc = __rnd_state_gc * 1103515245u + 12345u;
	return __rnd_state_gc;
}

struct object_s {
    char *sprite_frame[3];
    char spriteszx, spriteszy, sprites;
    int cursprite;
    char alive;
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


void spawn_bullet(struct object_s *owner, struct object_s *b, int offsetx, int offsety, int dx, int dy, int spx, int spy);

void reset_bullet(struct object_s *b);


/* Faz um inimigo atirar usando um pool de bullets fornecido.
    ebuf: ponteiro para array de bullets
    max_eb: tamanho do pool
    EN1, EN2, EN3: matrizes de inimigos
*/
void enemy_shoot(struct object_s *ebuf, int max_eb, struct object_s EN1[][10], struct object_s EN2[][10], struct object_s EN3[][10]);

#endif
