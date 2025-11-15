#include <hf-risc.h>
#include "vga_drv.h"
#include "game_sprites.h"
#include "game_functions.h"

int main(void)
{
	struct object_s enemy1;
	struct object_s enemy2;
	struct object_s enemy3;

	struct object_s p1, sh1, sh2, sh3, sh4;

	init_display();
	init_input();

	// enemies
	init_object(&enemy1, &monster1_an1[0][0], &monster1_an2[0][0], 0,
	            8, 8, 30, 35, 1, 1, 3, 3);

	init_object(&enemy2, &monster2_an1[0][0], &monster2_an2[0][0], 0,
	            8, 8, 170, 160, -1, 0, 1, 1);

	init_object(&enemy3, &monster3_an1[0][0], &monster3_an2[0][0], 0,
	            8, 8, 15, 80, -1, 1, 5, 3);

	// player
	init_object(&p1, &player[0][0], 0, 0,
	            8, 8, 120, 180, 0, 0, 1, 1);

	// all four shields 
	init_object(&sh1, &shield[0][0], 0, 0,
	            20, 10, 40, 150, 0, 0, 1, 1);

	init_object(&sh2, &shield[0][0], 0, 0,
	            20, 10, 100, 150, 0, 0, 1, 1);

	init_object(&sh3, &shield[0][0], 0, 0,
	            20, 10, 160, 150, 0, 0, 1, 1);

	init_object(&sh4, &shield[0][0], 0, 0,
	            20, 10, 220, 150, 0, 0, 1, 1);

	display_print("ROUND 1", 0, 180, 1, CYAN);

	while (1) {
		move_object(&enemy1);
		move_object(&enemy2);
		move_object(&enemy3);

		if (detect_collision(&enemy1, &enemy2)) display_print("collision: 1 and 2", 0, 180, 1, CYAN);
		if (detect_collision(&enemy1, &enemy3)) display_print("collision: 1 and 3", 0, 180, 1, CYAN);
		if (detect_collision(&enemy2, &enemy3)) display_print("collision: 2 and 3", 0, 180, 1, CYAN);
		
		if (get_input() == KEY_CENTER) {
			enemy2.dx = 0;
		}
		if (get_input() == KEY_LEFT) {
			enemy2.dx = -1;
		}
		if (get_input() == KEY_RIGHT) {
			enemy2.dx = 1;
		}
		
		delay_ms(50);
	}

	return 0;
}
