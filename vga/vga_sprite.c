#include <hf-risc.h>
#include "vga_drv.h"
#include "game_sprites.h"

/* sprites and sprite drawing */
char monster1a[8][11] = {
	{0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0}, // topo da cabeça
	{0, 0, 2, 0, 0, 0, 0, 0, 2, 0, 0}, // parte superior + olhos
	{0, 2, 0, 2, 0, 0, 0, 2, 0, 2, 0}, // lados + olhos
	{0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0}, // bochechas
	{0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0}, // bochechas
	{0, 2, 0, 0, 2, 2, 2, 0, 0, 2, 0}, // sorriso
	{0, 0, 2, 2, 0, 0, 0, 2, 2, 0, 0}, // parte inferior da boca
	{0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0}  // base da cabeça
};

char monster1b[8][11] = {
	{0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0}, // topo da cabeça
	{0, 0, 2, 0, 0, 0, 0, 0, 2, 0, 0}, // parte superior + olhos
	{0, 2, 0, 2, 0, 0, 0, 2, 0, 2, 0}, // lados + olhos
	{0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0}, // bochechas
	{0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0}, // bochechas
	{0, 2, 0, 2, 2, 0, 2, 2, 0, 2, 0}, // sorriso
	{0, 0, 2, 0, 2, 2, 2, 0, 2, 0, 0}, // parte inferior da boca
	{0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0}  // base da cabeça
};

void draw_sprite(unsigned int x, unsigned int y, char *sprite,
	unsigned int sizex, unsigned int sizey, int color)
{
	unsigned int px, py;
	
	if (color < 0) {
		for (py = 0; py < sizey; py++)
			for (px = 0; px < sizex; px++)
				display_pixel(x + px, y + py, sprite[py * sizex + px]);
	} else {
		for (py = 0; py < sizey; py++)
			for (px = 0; px < sizex; px++)
				display_pixel(x + px, y + py, color & 0xf);
	}
	
}

/* sprite based objects */
struct object_s {
	char *sprite_frame[3];
	char spriteszx, spriteszy, sprites;
	int cursprite;
	unsigned int posx, posy;
	int dx, dy;
	int speedx, speedy;
	int speedxcnt, speedycnt;
};


void init_object(struct object_s *obj, char *spritea, char *spriteb,
	char *spritec, char spriteszx, char spriteszy, int posx, int posy, 
	int dx, int dy, int spx, int spy)
{
	obj->sprite_frame[0] = spritea;
	obj->sprite_frame[1] = spriteb;
	obj->sprite_frame[2] = spritec;
	obj->spriteszx = spriteszx;
	obj->spriteszy = spriteszy;
	obj->cursprite = 0;
	obj->posx = posx;
	obj->posy = posy;
	obj->dx = dx;
	obj->dy = dy;
	obj->speedx = spx;
	obj->speedy = spy;
	obj->speedxcnt = spx;
	obj->speedycnt = spy;
}

void draw_object(struct object_s *obj, char chgsprite, int color)
{
	if (chgsprite) {
		obj->cursprite++;
		if (obj->sprite_frame[obj->cursprite] == 0)
			obj->cursprite = 0;
	}
	
	draw_sprite(obj->posx, obj->posy, obj->sprite_frame[obj->cursprite],
		obj->spriteszx, obj->spriteszy, color);
}

void move_object(struct object_s *obj)
{
	struct object_s oldobj;
	
	memcpy(&oldobj, obj, sizeof(struct object_s));
	
	if (--obj->speedxcnt == 0) {
		obj->speedxcnt = obj->speedx;
		obj->posx = obj->posx + obj->dx;

		if (obj->posx + obj->spriteszx >= VGA_WIDTH || obj->posx <= 0) obj->dx = -obj->dx; 
	}
	if (--obj->speedycnt == 0) {
		obj->speedycnt = obj->speedy;
		obj->posy = obj->posy + obj->dy;

		if (obj->posy + obj->spriteszy >= VGA_HEIGHT || obj->posy <= 0) obj->dy = -obj->dy;
	}

	if ((obj->speedx == obj->speedxcnt) || (obj->speedy == obj->speedycnt)) {
		draw_object(&oldobj, 0, 0);
		draw_object(obj, 1, -1);
	}
}


/* display and input */
void init_display()
{
	display_background(BLACK);
}

enum {
	KEY_CENTER	= 0x01,
	KEY_UP		= 0x02,
	KEY_LEFT	= 0x04,
	KEY_RIGHT	= 0x08,
	KEY_DOWN	= 0x10
};

void init_input()
{
	/* configure GPIOB pins 8 .. 12 as inputs */
	GPIOB->DDR &= ~(MASK_P8 | MASK_P9 | MASK_P10 | MASK_P11 | MASK_P12);
}

int get_input()
{
	int keys = 0;
	
	if (GPIOB->IN & MASK_P8)	keys |= KEY_CENTER;
	if (GPIOB->IN & MASK_P9)	keys |= KEY_UP;
	if (GPIOB->IN & MASK_P10)	keys |= KEY_LEFT;
	if (GPIOB->IN & MASK_P11)	keys |= KEY_RIGHT;
	if (GPIOB->IN & MASK_P12)	keys |= KEY_DOWN;
		
	return keys;
}

int detect_collision(struct object_s *obj1, struct object_s *obj2)
{
	if (obj1->posx < obj2->posx + obj2->spriteszx &&
		obj1->posx + obj1->spriteszx > obj2->posx &&
		obj1->posy < obj2->posy + obj2->spriteszy &&
		obj1->posy + obj1->spriteszy > obj2->posy) return 1;

	return 0;
}

/* main game loop */
int main(void)
{
	struct object_s enemy1, enemy2, enemy3;

	init_display();
	init_input();

	init_object(&enemy1, monster1a[0], monster1b[0], 0, 11, 8, 30, 35, 1, 1, 3, 3);
	init_object(&enemy2, monster1a[0], monster1b[0], 0, 11, 8, 170, 160, -1, 0, 1, 1);
	init_object(&enemy3, monster1a[0], monster1b[0], 0, 11, 8, 15, 80, -1, 1, 5, 3);

	display_print("hey", 0, 180, 1, CYAN);

	while (1) {
		move_object(&enemy1);
		move_object(&enemy2);
		move_object(&enemy3);

		if (detect_collision(&enemy1, &enemy2)) display_print("collision: 1 and 2", 0, 180, 1, CYAN);
		if (detect_collision(&enemy1, &enemy3)) display_print("collision: 1 and 3", 0, 180, 1, CYAN);
		if (detect_collision(&enemy2, &enemy3)) display_print("collision: 2 and 3", 0, 180, 1, CYAN);
		
		// you can change the direction, speed, etc...
				
		// you can get input keys
		if (get_input() == KEY_CENTER) {
			enemy2.dx = 0;
		}
		if (get_input() == KEY_LEFT) {
			enemy2.dx = -1;
		}
		if (get_input() == KEY_RIGHT) {
			enemy2.dx = 1;
		}
		
		// game loop frame limiter
		delay_ms(50);
	}

	return 0;
}
