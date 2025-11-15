#include <hf-risc.h>
#include "vga_drv.h"
#include "game_sprites.h"
#include "game_functions.h"



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
