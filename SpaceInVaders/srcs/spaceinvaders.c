#include <hf-risc.h>
#include "vga_drv.h"

// func display_print() -- Acho que vem do vga_drv.h
// 0, 0, 0, color		--  x, y, size, color    (0, 0) = top left corner


// ======================
// Variaveis de ambiente
// ======================

// Configuração de velocidade do player
// Valores positivos = frames por movimento (2=médio, 3=lento); Valor 1 = move 1 pixel por frame (rápido); Valores negativos = pixels por frame (-2=muito rápido, move 2 pixels por frame)
int PLAYER_SPEED = -2;


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
	
	// Speed negativo = move múltiplos pixels por frame
	if (obj->speedx < 0) {
		obj->posx = obj->posx + (obj->dx * (-obj->speedx));
		if (obj->posx + obj->spriteszx >= VGA_WIDTH || obj->posx <= 0) obj->dx = -obj->dx;
	} else if (--obj->speedxcnt == 0) {
		obj->speedxcnt = obj->speedx;
		obj->posx = obj->posx + obj->dx;
		if (obj->posx + obj->spriteszx >= VGA_WIDTH || obj->posx <= 0) obj->dx = -obj->dx; 
	}
	
	if (obj->speedy < 0) {
		obj->posy = obj->posy + (obj->dy * (-obj->speedy));
		if (obj->posy + obj->spriteszy >= VGA_HEIGHT || obj->posy <= 0) obj->dy = -obj->dy;
	} else if (--obj->speedycnt == 0) {
		obj->speedycnt = obj->speedy;
		obj->posy = obj->posy + obj->dy;
		if (obj->posy + obj->spriteszy >= VGA_HEIGHT || obj->posy <= 0) obj->dy = -obj->dy;
	}

	if ((obj->speedx == obj->speedxcnt) || (obj->speedy == obj->speedycnt) || (obj->speedx < 0) || (obj->speedy < 0)) {
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

void control_player(struct object_s *player){
	int keys = get_input();

	if (keys == KEY_LEFT) {
		player->dx = -1;
	}
	if (keys == KEY_RIGHT) {
		player->dx = 1;
	}
	if (keys == 0) {
		player->dx = 0;
	}
}

/* main game loop */
int main(void)
{
	int score = 0;
	struct object_s player, enemy1, enemy2, enemy3, bullet_obj;

	init_display();
	init_input();

	init_object(&enemy1, alien2a[0], alien2b[0], 0, 11, 8, 30, 35, 1, 1, 3, 3);
    
	init_object(&enemy2, alien_medium_a[0], alien_medium_b[0], 0, 11, 8, 40, 80, -1, 1, 5, 3);
	
	init_object(&enemy3, alien_strong_a[0], alien_strong_b[0], 0, 11, 8, 15, 80, -1, 1, 5, 3);
	
	init_object(&player, spaceship[0], spaceship[0], 0, 11, 8, 170, 160, -1, 0, PLAYER_SPEED, PLAYER_SPEED);
	
	init_object(&bullet_obj, bullet[0], bullet[0], 0, 3, 5, player.posx + 4, player.posy, 0, -1, -3, -3);
	
	

	while (1) {
		move_object(&enemy1);
		move_object(&enemy2);
		move_object(&enemy3);
		


		//if (detect_collision(&enemy1, &player)) display_print("collision: 1 and 2", 0, 180, 1, CYAN);
		//if (detect_collision(&enemy1, &enemy3)) display_print("collision: 1 and 3", 0, 180, 1, CYAN);
		//if (detect_collision(&player, &enemy3)) display_print("collision: 2 and 3", 0, 180, 1, CYAN);
		
		// you can change the direction, speed, etc...
				
		// you can get input keys
		control_player(&player);
	
		if (get_input() == KEY_UP) {
			//move_object(&bullet_obj);
		}

		/*if(score ++){
			display_frectangle(0, 0, 80, 10, BLACK);
		char buffer[20];
		sprintf(buffer, "%d", score);
		display_print(buffer, 0, 0, 1, RED);
		*/
				
		// game loop frame limiter
		delay_ms(50);
	}

	return 0;
}
