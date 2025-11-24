#include "game_functions.h"
#include "game_sprites.h"

int PLAYER_SPEED = -2;

int main()
{
	int score_atual=0;
	int score = 0;
	int i, j;
	int vida_atual = 3;
	int vida_max = 8;

	int bullet_active = 0;
	int reload_timer = 0; 

	int game_over = 0;
	
	// Controle de movimento dos inimigos
	int enemies_direction = 1;  // 1 = direita, -1 = esquerda
	int move_down = 0;
	int frame_count = 0;

	struct object_s en1[5][10], en2[5][10], en3[5][10];
	struct object_s pl, sh1, sh2, sh3, sh4, bu;

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

	/* Desenhar todos os inimigos inicializados e garantir alive=1 */
	for (j = 0; j < 10; j++) {
		if (en3[0][j].alive) draw_object(&en3[0][j], 1, -1);
	}
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 10; j++) {
			if (en2[i][j].alive) draw_object(&en2[i][j], 1, -1);
			if (en1[i][j].alive) draw_object(&en1[i][j], 1, -1);
		}
	}

	// player
	init_object(&pl, &player[0][0], 0, 0, 11, 8, 120, 180, 0, 0, PLAYER_SPEED, PLAYER_SPEED);
	
	// escudos
	init_object(&sh1, &shield[0][0], 0, 0, 20, 10,  40, 150, 0, 0, 1, 1);

	init_object(&sh2, &shield[0][0], 0, 0, 20, 10, 100, 150, 0, 0, 1, 1);

	init_object(&sh3, &shield[0][0], 0, 0, 20, 10, 160, 150, 0, 0, 1, 1);

	init_object(&sh4, &shield[0][0], 0, 0, 20, 10, 220, 150, 0, 0, 1, 1);

	
	/* bullet: pos off-screen, dy = -1 (up), speedy = -2 (moves 2 px/frame) */
	init_object(&bu, &bullet[0][0], 0, 0, 1, 2, pl.posx, pl.posy, 0, -1, 0, -10);

	display_print("score:  ", 0, 0, 1, WHITE);


	while (!game_over) {
		
		if (score_atual != score) {
			display_frectangle(40, 0, 80, 10, BLACK);
			char score_str[20];
			int_to_string(score, score_str);
			display_print(score_str, 40, 0, 1, WHITE);
			score_atual = score;
		}


		// Verificar colisão com todos os inimigos (checar por grupo explicitamente)
		// en1 (2 linhas)
		for (i = 0; i < 2 && !game_over; i++) {
			for (j = 0; j < 10; j++) {
				if (en1[i][j].alive && detect_collision(&pl, &en1[i][j])) {
					game_over = 1;
					break;
				}
			}
		}
		// en2 (2 linhas)
		for (i = 0; i < 2 && !game_over; i++) {
			for (j = 0; j < 10; j++) {
				if (en2[i][j].alive && detect_collision(&pl, &en2[i][j])) {
					game_over = 1;
					break;
				}
			}
		}
		// en3 (apenas a linha 0 foi inicializada)
		for (j = 0; j < 10 && !game_over; j++) {
			if (en3[0][j].alive && detect_collision(&pl, &en3[0][j])) {
				game_over = 1;
				break;
			}
		}
/*
		if(detect_collision(&pl, &bu)){
			if(vida_atual > 1){
				vida_atual = vida_atual - 1;
			}
			else{
				game_over = 1;
				break;
			}
		}
*/
		// Movimento sincronizado dos inimigos -> CORRETO
		frame_count++;
		if (frame_count >= 10) {  // Velocidade do movimento (ajuste conforme necessário)
			frame_count = 0;
			
			// Verificar se algum inimigo vivo bateu na parede
			int hit_wall = 0;

			// Verificar en3
			for (j = 0; j < 10; j++) {
				if (!en3[0][j].alive) continue;
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
						if (!en2[i][j].alive) continue;
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
						if (!en1[i][j].alive) continue;
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
			
			// Mover todos os inimigos (ignorar mortos)
			for (j = 0; j < 10; j++) {
				if (!en3[0][j].alive) continue;
				draw_object(&en3[0][j], 0, 0);  // Apagar
				en3[0][j].posx += enemies_direction * 2;
				if (move_down) en3[0][j].posy += 8;
				draw_object(&en3[0][j], 1, -1);  // Redesenhar
			}

			for (i = 0; i < 2; i++) {
				for (j = 0; j < 10; j++) {
					if (en2[i][j].alive) {
						draw_object(&en2[i][j], 0, 0);
						en2[i][j].posx += enemies_direction * 2;
						if (move_down) en2[i][j].posy += 8;
						draw_object(&en2[i][j], 1, -1);
					}

					if (en1[i][j].alive) {
						draw_object(&en1[i][j], 0, 0);
						en1[i][j].posx += enemies_direction * 2;
						if (move_down) en1[i][j].posy += 8;
						draw_object(&en1[i][j], 1, -1);
					}
				}
			}
			
			move_down = 0;
		}

		// Após mover os inimigos, verificar novamente colisões (caso a descida os tenha alcançado)
		// en1
		for (i = 0; i < 2 && !game_over; i++) {
			for (j = 0; j < 10; j++) {
				if (en1[i][j].alive && detect_collision(&pl, &en1[i][j])) {
					game_over = 1;
					break;
				}
			}
		}
		// en2
		for (i = 0; i < 2 && !game_over; i++) {
			for (j = 0; j < 10; j++) {
				if (en2[i][j].alive && detect_collision(&pl, &en2[i][j])) {
					game_over = 1;
					break;
				}
			}
		}
		// en3
		for (j = 0; j < 10 && !game_over; j++) {
			if (en3[0][j].alive && detect_collision(&pl, &en3[0][j])) {
				game_over = 1;
				break;
			}
		}
		// MOVIMENTACAO DOS INIMIGOS ESTA CORRETA ATE AQUI

		move_object(&sh1);
		move_object(&sh2);
		move_object(&sh3);
		move_object(&sh4);
		move_object(&pl);
		control_player(&pl);
		// DECLARACAO ESCUDOS

		
		if (get_input() == KEY_UP) {
			if (!bullet_active && reload_timer == 0) {
				bullet_active = 1;
				/* Centraliza o tiro na nave */
				bu.posx = pl.posx + (pl.spriteszx > bu.spriteszx ? (pl.spriteszx - bu.spriteszx) / 2 : 0);
				bu.posy = pl.posy - bu.spriteszy;
				bu.alive = 1;
				draw_object(&bu, 1, -1); // desenha inicialmente
    		}
		}

		if (reload_timer > 0) reload_timer--;
		

		if (bullet_active) {
			int old_bu_posy = bu.posy;
			move_object(&bu); // usa dx/dy e speed para mover e redesenhar
			if (bu.posy + bu.spriteszy <= 0) {
				reset_bullet(&bu);
				bullet_active = 0;
			} else {
				/* colisão com inimigos (en1,en2,en3) */
				for (i = 0; i < 2 && bullet_active; i++) {
					for (j = 0; j < 10 && bullet_active; j++) {
						if (detect_collision(&bu, &en1[i][j])) {
							draw_object(&en1[i][j], 0, 0); // apagar
							reset_bullet(&bu);
							en1[i][j].alive = 0;
							bullet_active = 0;
							score++;
							break;
						}
						if (detect_collision(&bu, &en2[i][j])) {
							draw_object(&en2[i][j], 0, 0);
							en2[i][j].alive = 0;
							reset_bullet(&bu);
							bullet_active = 0;
							//bu.posy = VGA_HEIGHT + 3;
							score++;
							break;
						}
					}
				}
				for (j = 0; j < 10 && bullet_active; j++) {
					if (detect_collision(&bu, &en3[0][j])) {
						en3[0][j].alive = 0;
						draw_object(&en3[0][j], 0, 0);
						reset_bullet(&bu);
						bullet_active = 0;
						//bu.posy = VGA_HEIGHT;
						score++;
						break;
					}
				}
			}
		}
		

		delay_ms(50);

	}

	init_display();

	char score_final[20];
	int_to_string(score, score_final);

	display_frectangle(0, 100, VGA_WIDTH, 40, BLACK);
	display_print("Game Over", 30, 90, 3, WHITE);
	display_print("SCORE", 90, 125, 1, WHITE);
	display_print(score_final, 150, 125, 1, WHITE);
	
	
	return 0;
}
