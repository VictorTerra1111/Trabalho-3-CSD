#include "game_functions.h"
#include "game_sprites.h"

int PLAYER_SPEED = -2;

int main()
{
	int score = 0;
	int i, j;
	int vida_atual = 3;
	int vida_max = 8;

	int game_over = 0;
	
	// Controle de movimento dos inimigos
	int enemies_direction = 1;  // 1 = direita, -1 = esquerda
	int move_down = 0;
	int frame_count = 0;

	struct object_s en1[5][10], en2[5][10], en3[5][10];
	struct object_s pl, sh1, sh2, sh3, sh4;
	struct object_s nave_mae;
	struct object_s bu;

	init_display();
	init_input();

	// 1 fileira de 10 inimigos tipo en3 (monster3)
	for (j = 0; j < 10; j++) {
		init_object(&en3[0][j], &monster3_an1[0][0], &monster3_an2[0][0], 0, 11, 8, 10 + j * 25, 20, 0, 0, 3, 3);
	}

	// 2 fileiras de 10 inimigos tipo en2 (monster2)
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 10; j++) {
			init_object(&en2[i][j], &monster2_an1[0][0], &monster2_an2[0][0], 0, 11, 8, 10 + j * 25, 35 + i * 15, 0, 0, 3, 3);
		}
	}

	// 2 fileiras de 10 inimigos tipo en1 (monster1)
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 10; j++) {
			init_object(&en1[i][j], &monster1_an1[0][0], &monster1_an2[0][0], 0, 
			            8, 8, 10 + j * 25, 65 + i * 15, 0, 0, 3, 3);
		}
	}

	// player
	init_object(&pl, &player[0][0], 0, 0, 11, 8, 120, 180, 0, 0, PLAYER_SPEED, PLAYER_SPEED);
	
	// all four shields 
	init_object(&sh1, &shield[0][0], 0, 0, 20, 10,  40, 150, 0, 0, 1, 1);

	init_object(&sh2, &shield[0][0], 0, 0, 20, 10, 100, 150, 0, 0, 1, 1);

	init_object(&sh3, &shield[0][0], 0, 0, 20, 10, 160, 150, 0, 0, 1, 1);

	init_object(&sh4, &shield[0][0], 0, 0, 20, 10, 220, 150, 0, 0, 1, 1);

	init_object(&nave_mae, &mothership[0][0], 0, 0, 30, 10, 0, 10, 1, 0, 5, 1);

	init_object(&bu, &bullet[0][0], 0, 0, 1, 2, 0, 0, 0, 0, 10, 10);

	display_print("score ", 0, 0, 1, RED);

	while (!game_over) {
		if (score++) {
			display_frectangle(40, 0, 80, 10, BLACK);
			char score_str[20];
			int_to_string(score, score_str);
			display_print(score_str, 40, 0, 1, RED);	
		}	


		// Verificar colisão com todos os inimigos
		for (i = 0; i < 2; i++) {
			for (j = 0; j < 10; j++) {
				if (detect_collision(&pl, &en1[i][j]) || detect_collision(&pl, &en2[i][j]) || detect_collision(&pl, &en3[0][j])) {
					game_over = 1;
					break;
				}
			}
			if (game_over) break;
		}

		if(detect_collision(&pl, &bu)){
			if(vida_atual > 1){
				vida_atual = vida_atual - 1;
			}
			else{
				game_over = 1;
				break;
			}
		}

		// Movimento sincronizado dos inimigos
		frame_count++;
		if (frame_count >= 10) {  // Velocidade do movimento (ajuste conforme necessário)
			frame_count = 0;
			
			// Verificar se algum inimigo bateu na parede
			int hit_wall = 0;
			
			// Verificar en3
			for (j = 0; j < 10; j++) {
				if ((en3[0][j].posx + en3[0][j].spriteszx >= VGA_WIDTH && enemies_direction > 0) ||
				    (en3[0][j].posx <= 0 && enemies_direction < 0)) {
					hit_wall = 1;
					break;
				}
			}
			
			// Verificar en2
			if (!hit_wall) {
				for (i = 0; i < 2; i++) {
					for (j = 0; j < 10; j++) {
						if ((en2[i][j].posx + en2[i][j].spriteszx >= VGA_WIDTH && enemies_direction > 0) ||
						    (en2[i][j].posx <= 0 && enemies_direction < 0)) {
							hit_wall = 1;
							break;
						}
					}
					if (hit_wall) break;
				}
			}
			
			// Verificar en1
			if (!hit_wall) {
				for (i = 0; i < 2; i++) {
					for (j = 0; j < 10; j++) {
						if ((en1[i][j].posx + en1[i][j].spriteszx >= VGA_WIDTH && enemies_direction > 0) ||
						    (en1[i][j].posx <= 0 && enemies_direction < 0)) {
							hit_wall = 1;
							break;
						}
					}
					if (hit_wall) break;
				}
			}
			
			// Se bateu na parede, inverte direção e desce
			if (hit_wall) {
				enemies_direction = -enemies_direction;
				move_down = 1;
			}
			
			// Mover todos os inimigos
			for (j = 0; j < 10; j++) {
				draw_object(&en3[0][j], 0, 0);  // Apagar
				en3[0][j].posx += enemies_direction * 2;
				if (move_down) en3[0][j].posy += 8;
				draw_object(&en3[0][j], 1, -1);  // Redesenhar
			}
			
			for (i = 0; i < 2; i++) {
				for (j = 0; j < 10; j++) {
					draw_object(&en2[i][j], 0, 0);
					en2[i][j].posx += enemies_direction * 2;
					if (move_down) en2[i][j].posy += 8;
					draw_object(&en2[i][j], 1, -1);
					
					draw_object(&en1[i][j], 0, 0);
					en1[i][j].posx += enemies_direction * 2;
					if (move_down) en1[i][j].posy += 8;
					draw_object(&en1[i][j], 1, -1);
				}
			}
			
			move_down = 0;
		}
		
		move_object(&sh1);
		move_object(&sh2);
		move_object(&sh3);
		move_object(&sh4);
		move_object(&nave_mae);
		move_object(&pl);
		

		control_player(&pl);
		
		if (get_input() == KEY_UP) {
			//move_object(&bullet_obj);
		}

		//if (detect_collision(&enemy1, &enemy2)) display_print("collision: 1 and 2", 0, 180, 1, CYAN);
		//if (detect_collision(&enemy1, &enemy3)) display_print("collision: 1 and 3", 0, 180, 1, CYAN);
		//if (detect_collision(&enemy2, &enemy3)) display_print("collision: 2 and 3", 0, 180, 1, CYAN);


		delay_ms(50);
	}

	init_display();

	char score_final[20];
	int_to_string(score, score_final);

	display_frectangle(300, 218, 80, 10, BLACK);
	display_print("Game over", 150, 109, 3, WHITE);
	display_print("SCORE", 150, 102, 1, WHITE);
	display_print(score_final, 5, 7, 1, WHITE);
	
	
	return 0;
}
