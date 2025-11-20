#include "game_functions.h"
#include "game_sprites.h"

// func display_print() -- Acho que vem do vga_drv.h
// 0, 0, 0, color		--  x, y, size, color    (0, 0) = top left corner


// ======================
// Variaveis de ambiente
// ======================

// Configuração de velocidade do player
// Valores positivos = frames por movimento (2=médio, 3=lento); Valor 1 = move 1 pixel por frame (rápido); Valores negativos = pixels por frame (-2=muito rápido, move 2 pixels por frame)
int PLAYER_SPEED = -2;

int main(void)
{
	int score = 0;
	int i, j;
	
	struct object_s enemies[5][10];  // 5 fileiras de 10 inimigos
	struct object_s p1, sh1, sh2, sh3, sh4, bullet;
	struct object_s nave_mae;

	init_display();
	init_input();

	// criar 5 fileiras de 10 enemies (alien2a)
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 10; j++) {
			init_object(&enemies[i][j], &alien_medium_a[0][0], &alien_medium_b[0][0], 0, 
			            11, 8, 10 + j * 25, 20 + i * 15, 1, 0, 3, 3);
		}
	}
	
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 10; j++) {
			init_object(&enemies[i][j], &alien_strong_a[0][0], &alien_strong_b[0][0], 0, 
			            11, 8, 10 + j * 25, 20 + i * 15, 1, 0, 3, 3);
		}
	}

	// player
	init_object(&p1, &spaceship[0][0], 0, 0, 11, 8, 120, 180, 0, 0, PLAYER_SPEED, PLAYER_SPEED);
	
	// all four shields 
	init_object(&sh1, &shield[0][0], 0, 0, 20, 10,  40, 150, 0, 0, 1, 1);

	init_object(&sh2, &shield[0][0], 0, 0, 20, 10, 100, 150, 0, 0, 1, 1);

	init_object(&sh3, &shield[0][0], 0, 0, 20, 10, 160, 150, 0, 0, 1, 1);

	init_object(&sh4, &shield[0][0], 0, 0, 20, 10, 220, 150, 0, 0, 1, 1);

	init_object(&nave_mae, &big_guy[0][0], 0, 0, 30, 10, 0, 10, 1, 0, 5, 1);


	display_print("ROUND 1", 0, 0, 1, RED);

	while (1) {
		// mover todos os inimigos
		for (i = 0; i < 2; i++) {
			for (j = 0; j < 10; j++) {
				move_object(&enemies[i][j]);
			}
		}
	
	
		
		move_object(&sh1);
		move_object(&sh2);
		move_object(&sh3);
		move_object(&sh4);
		move_object(&nave_mae);
		move_object(&p1);
		

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
