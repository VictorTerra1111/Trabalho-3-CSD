#include <hf-risc.h>
#include "vga_drv.h"
#include "game_sprites.h"
#include "game_functions.h"

// func display_print() -- Acho que vem do vga_drv.h
// 0, 0, 0, color		--  x, y, size, color    (0, 0) = top left corner


// ======================
// Variaveis de ambiente
// ======================

// Configuração de velocidade do player
// Valores positivos = frames por movimento (2=médio, 3=lento); Valor 1 = move 1 pixel por frame (rápido); Valores negativos = pixels por frame (-2=muito rápido, move 2 pixels por frame)
int PLAYER_SPEED = -2;

int main()
{
	int score = 0;
	
	struct object_s enemy1;
	struct object_s enemy2;
	struct object_s enemy3;

	struct object_s p1, sh1, sh2, sh3, sh4, bullet;

	

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
	            8, 8, 120, 180, 0, 0, PLAYER_SPEED, PLAYER_SPEED);

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

		control_player(&p1);


		if (get_input() == KEY_UP) {
			//move_object(&bullet_obj);
		}

		//if (detect_collision(&enemy1, &enemy2)) display_print("collision: 1 and 2", 0, 180, 1, CYAN);
		//if (detect_collision(&enemy1, &enemy3)) display_print("collision: 1 and 3", 0, 180, 1, CYAN);
		//if (detect_collision(&enemy2, &enemy3)) display_print("collision: 2 and 3", 0, 180, 1, CYAN);
		
		/*if(score ++){
			display_frectangle(0, 0, 80, 10, BLACK);
		char buffer[20];
		sprintf(buffer, "%d", score);
		display_print(buffer, 0, 0, 1, RED);
		*/


		delay_ms(50);
	}

	return 0;
}
