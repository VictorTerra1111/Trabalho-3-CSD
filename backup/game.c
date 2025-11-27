//300X218

#include "game_functions.h"
#include "game_sprites.h"
int ENEMYS = 50;
int ROUNDS = 1;
int PLAYER_SPEED = -3;
int SCORE_ATUAL = 0;
int SCORE = 0;
int ATTEMPT_COUNT = 1;
int i, j;
int VIDA = 3;
int VIDA_ATUAL = 3;
int VIDA_MAX = 8;


// grace frames to avoid immediate collision checks at startup
int START_GRACE = 3;
int SPEED_EN = 3;
// Controle de ciclos de tiro dos inimigos (50ms cada ciclo -> 20 ciclos = 1 segundo)
int ENEMY_BULLET_TIMER = 20;
int ENEMY_BULLET = 0;

// indica quando o tiro deve ser desativada
int BULLET_ACTIVE = 0;

// indica o tempo até poder atirar o próximo tiro
int RELOAD_TIMER = 0;

// indica quando o jogo acabou
int WIN = 0;
int GAME_OVER = 0;
int RESTART_REQUESTED = 0;

// Controle de movimento dos inimigos
int ENEMIE_DIRECTION = 1; // 1 = direita, -1 = esquerda

// indica quando os inimigos deve descer (quando colidem com a borda)
int MOVE_DOWN = 0;

// Velocidade do movimento sincronizado dos inimigos
int FRAME_COUNT = 0;

// vida dos escudos
int lsh1 = 5, lsh2 = 5, lsh3 = 5, lsh4 = 5;

// declara os inimigos
struct object_s EN1[5][10], EN2[5][10], EN3[5][10];

// declara o player, os shields(SH), e a bullet(BU)
struct object_s PL, SH1, SH2, SH3, SH4, BU;
/* Pool de balas dos inimigos */
#define MAX_ENEMY_BULLETS 6
struct object_s EBU[MAX_ENEMY_BULLETS];

int main()
{

	init_display();
	init_input();

	//===========================//
	// INICIALIZAÇÃO DOS OBJETOS //
	//===========================//

	// 1 fileira de 10 inimigos tipo EN3 (monster3)
	for (j = 0; j < 10; j++)
	{
		init_object(&EN3[0][j], &monster3_an1[0][0], &monster3_an2[0][0], 0, 11, 8, 10 + j * 25, 20, 0, 0, (ROUNDS + SPEED_EN), (ROUNDS + SPEED_EN));
	}

	// 2 fileiras de 10 inimigos tipo EN2 (monster2)
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 10; j++)
		{
			init_object(&EN2[i][j], &monster2_an1[0][0], &monster2_an2[0][0], 0, 11, 8, 10 + j * 25, 35 + i * 15, 0, 0, (ROUNDS + SPEED_EN), (ROUNDS + SPEED_EN));
		}
	}

	// 2 fileiras de 10 inimigos tipo EN1 (monster1)
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 10; j++)
		{
			init_object(&EN1[i][j], &monster1_an1[0][0], &monster1_an2[0][0], 0,
						8, 8, 10 + j * 25, 65 + i * 15, 0, 0, (ROUNDS + SPEED_EN), (ROUNDS + SPEED_EN));
		}
	}

	// Desenhar todos os inimigos inicializados e garantir alive=1 //
	for (j = 0; j < 10; j++)
	{
		if (EN3[0][j].alive)
			draw_object(&EN3[0][j], 1, -1);
	}
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 10; j++)
		{
			if (EN2[i][j].alive)
				draw_object(&EN2[i][j], 1, -1);
			if (EN1[i][j].alive)
				draw_object(&EN1[i][j], 1, -1);
		}
	}

	// player
	init_object(&PL, &player[0][0], 0, 0, 11, 8, 120, 180, 0, 0, PLAYER_SPEED, PLAYER_SPEED);

	// escudos (agora 24x13) — posicoes ajustadas para evitar overflow
	init_object(&SH1, &shieldG[0][0], 0, 0, 32, 15, 30, 145, 0, 0, 1, 1);
	init_object(&SH2, &shieldG[0][0], 0, 0, 32, 15, 90, 145, 0, 0, 1, 1);
	init_object(&SH3, &shieldG[0][0], 0, 0, 32, 15, 150, 145, 0, 0, 1, 1);
	init_object(&SH4, &shieldG[0][0], 0, 0, 32, 15, 210, 145, 0, 0, 1, 1);

	// bullet
	init_object(&BU, &bullet[0][0], 0, 0, 1, 2, PL.posx, PL.posy, 0, -1, 0, -10);

	init_display();

	/* Inicializa pool de balas inimigas e desativa-as */
	for (int k = 0; k < MAX_ENEMY_BULLETS; k++)
	{
		init_object(&EBU[k], &bullet[0][0], 0, 0, 1, 2, 0, VGA_HEIGHT, 0, 1, 0, -4);
		reset_bullet(&EBU[k]);
	}

	display_print("score: 0", 0, 0, 1, WHITE);
	display_print("3", 65, 200, 1, WHITE);


	// Print da vida
	
	//========================//
	// LOOP PRINCIPAL DO JOGO //
	//========================//
	move_object(&SH1);
	move_object(&SH2);
	move_object(&SH3);
	move_object(&SH4);

	while (!GAME_OVER && !WIN )
		{
			draw_sprite(50, 200, &life[0][0], 11, 8, -1);
			//=====================//
			// STATUS SUPERIOR//
			//=====================//
			if (VIDA_ATUAL != VIDA)
			{
				display_frectangle(65, 200, 20, 10, BLACK);
				char tmp[16];
				int_to_string(VIDA, tmp);
				display_print(tmp, 65, 200, 1, WHITE);
				VIDA_ATUAL = VIDA;
			}

			if (SCORE_ATUAL != SCORE)
			{
				/* limpa label+valor */
				display_frectangle(0, 0, 120, 10, BLACK);
				char SCORE_STR[32];
				char tmp[16];
				int_to_string(SCORE, tmp);
				/* monta "score: " + número */
				int p = 0;
				const char *lbl = "score: ";
				for (int k = 0; lbl[k]; k++)
					SCORE_STR[p++] = lbl[k];
				for (int k = 0; tmp[k]; k++)
					SCORE_STR[p++] = tmp[k];
				SCORE_STR[p] = '\0';
				display_print(SCORE_STR, 0, 0, 1, WHITE);
				SCORE_ATUAL = SCORE;
			}

			/* Evita colisões imediatas logo no startup: decrementar contador de graça
			   nos primeiros frames e pular as checagens enquanto START_GRACE > 0. */
			if (START_GRACE > 0)
			{
				START_GRACE--;
			}
			else
			{
				// Verificar colisão com todos os inimigos
				// EN1 (2 linhas)
				for (i = 0; i < 2 && !GAME_OVER; i++)
				{
					for (j = 0; j < 10; j++)
					{
						if (EN1[i][j].alive && detect_collision(&PL, &EN1[i][j]))
						{
							GAME_OVER = 1;
							break;
						}
					}
				}
				// EN2 (2 linhas)
				for (i = 0; i < 2 && !GAME_OVER; i++)
				{
					for (j = 0; j < 10; j++)
					{
						if (EN2[i][j].alive && detect_collision(&PL, &EN2[i][j]))
						{
							GAME_OVER = 1;
							break;
						}
					}
				}
				// EN3 (1 linha)
				for (j = 0; j < 10 && !GAME_OVER; j++)
				{
					if (EN3[0][j].alive && detect_collision(&PL, &EN3[0][j]))
					{
						GAME_OVER = 1;
						break;
					}
				}
			}

			// Movimento sincronizado dos inimigos
			FRAME_COUNT++;
			if (FRAME_COUNT >= 10)
			{
				FRAME_COUNT = 0;

				// Verificar se algum inimigo vivo bateu na parede
				int hit_wall = 0;

				// Verificar EN3
				for (j = 0; j < 10; j++)
				{
					if (!EN3[0][j].alive)
						continue;
					if ((EN3[0][j].posx + EN3[0][j].spriteszx >= VGA_WIDTH && ENEMIE_DIRECTION > 0) ||
						(EN3[0][j].posx <= 0 && ENEMIE_DIRECTION < 0))
					{
						hit_wall = 1;
						break;
					}
				}

				// Verificar EN2
				if (!hit_wall)
				{
					for (i = 0; i < 2; i++)
					{
						for (j = 0; j < 10; j++)
						{
							if (!EN2[i][j].alive)
								continue;
							if ((EN2[i][j].posx + EN2[i][j].spriteszx >= VGA_WIDTH && ENEMIE_DIRECTION > 0) ||
								(EN2[i][j].posx <= 0 && ENEMIE_DIRECTION < 0))
							{
								hit_wall = 1;
								break;
							}
						}
						if (hit_wall)
							break;
					}
				}

				// Verificar EN1
				if (!hit_wall)
				{
					for (i = 0; i < 2; i++)
					{
						for (j = 0; j < 10; j++)
						{
							if (!EN1[i][j].alive)
								continue;
							if ((EN1[i][j].posx + EN1[i][j].spriteszx >= VGA_WIDTH && ENEMIE_DIRECTION > 0) ||
								(EN1[i][j].posx <= 0 && ENEMIE_DIRECTION < 0))
							{
								hit_wall = 1;
								break;
							}
						}
						if (hit_wall)
							break;
					}
				}

				// Se bateu na parede, inverte direção e desce
				if (hit_wall)
				{
					ENEMIE_DIRECTION = -ENEMIE_DIRECTION;
					MOVE_DOWN = 1;
				}

				// Mover todos os inimigos (ignorar mortos)
				for (j = 0; j < 10; j++)
				{
					if (!EN3[0][j].alive)
						continue;
					draw_object(&EN3[0][j], 0, 0); // Apaga o inimigo
					EN3[0][j].posx += ENEMIE_DIRECTION * 2;
					if (MOVE_DOWN)
						EN3[0][j].posy += 8;
					draw_object(&EN3[0][j], 1, -1); // Redesenhar
				}

				for (i = 0; i < 2; i++)
				{
					for (j = 0; j < 10; j++)
					{
						if (EN2[i][j].alive)
						{
							draw_object(&EN2[i][j], 0, 0);
							EN2[i][j].posx += ENEMIE_DIRECTION * 2;
							if (MOVE_DOWN)
								EN2[i][j].posy += 8;
							draw_object(&EN2[i][j], 1, -1);
						}

						if (EN1[i][j].alive)
						{
							draw_object(&EN1[i][j], 0, 0);
							EN1[i][j].posx += ENEMIE_DIRECTION * 2;
							if (MOVE_DOWN)
								EN1[i][j].posy += 8;
							draw_object(&EN1[i][j], 1, -1);
						}
					}
				}

				MOVE_DOWN = 0;
			}

			// Após mover os inimigos, verificar novamente colisões (caso a descida os tenha alcançado)
			if (START_GRACE <= 0)
			{
				// EN1
				for (i = 0; i < 2 && !GAME_OVER; i++)
				{
					for (j = 0; j < 10; j++)
					{
						if (EN1[i][j].alive && detect_collision(&PL, &EN1[i][j]))
						{
							GAME_OVER = 1;
							break;
						}
					}
				}
				// EN2
				for (i = 0; i < 2 && !GAME_OVER; i++)
				{
					for (j = 0; j < 10; j++)
					{
						if (EN2[i][j].alive && detect_collision(&PL, &EN2[i][j]))
						{
							GAME_OVER = 1;
							break;
						}
					}
				}
				// EN3
				for (j = 0; j < 10 && !GAME_OVER; j++)
				{
					if (EN3[0][j].alive && detect_collision(&PL, &EN3[0][j]))
					{
						GAME_OVER = 1;
						break;
					}
				}
			}

			// DECLARACAO ESCUDOS
			move_object(&PL);
			
			// Garantir que o player não saia da tela após o movimento
			// `posx` é `unsigned`, então um valor negativo vira um grande unsigned (wrap).
			// Detectar wrap/underflow verificando se posx ultrapassa a largura da tela.
			if (PL.posx > VGA_WIDTH)
				PL.posx = 0;
			if (PL.posx + PL.spriteszx > VGA_WIDTH)
				PL.posx = (VGA_WIDTH - PL.spriteszx) >= 0 ? (VGA_WIDTH - PL.spriteszx) : 0;
			control_player(&PL);
			// Garantir que o player não saia da tela após controle por input
			if (PL.posx > VGA_WIDTH)
				PL.posx = 0;
			if (PL.posx + PL.spriteszx > VGA_WIDTH)
				PL.posx = (VGA_WIDTH - PL.spriteszx) >= 0 ? (VGA_WIDTH - PL.spriteszx) : 0;

			if (get_input() == KEY_UP)
			{
				if (!BULLET_ACTIVE && RELOAD_TIMER == 0)
				{
					BULLET_ACTIVE = 1;
					/* Centraliza o tiro na nave */
					BU.posx = PL.posx + (PL.spriteszx > BU.spriteszx ? (PL.spriteszx - BU.spriteszx) / 2 : 0);
					BU.posy = PL.posy - BU.spriteszy;
					BU.alive = 1;
					draw_object(&BU, 1, -1); // desenha inicialmente
				}
			}

			if (RELOAD_TIMER > 0)
				RELOAD_TIMER--;

			if (BULLET_ACTIVE)
			{
				int old_bu_posy = BU.posy;
				move_object(&BU); // usa dx/dy e speed para mover e redesenhar
				// Se atingir o topo da tela, reseta o tiro
				if (BU.posy + BU.spriteszy <= 0)
				{
					reset_bullet(&BU);
					BULLET_ACTIVE = 0;
				}
				else
				{
					/* colisão com inimigos (EN1,EN2,EN3) */
					for (i = 0; i < 2 && BULLET_ACTIVE; i++)
					{
						for (j = 0; j < 10 && BULLET_ACTIVE; j++)
						{
							if (detect_collision(&BU, &EN1[i][j]))
							{
								draw_object(&EN1[i][j], 0, 0); // apagar
								reset_bullet(&BU);
								EN1[i][j].alive = 0;
								BULLET_ACTIVE = 0;
								SCORE+= 10;
								ENEMYS --;
								if(ENEMYS == 0){
									WIN = 1;
								}
								break;
							}
							if (detect_collision(&BU, &EN2[i][j]))
							{
								draw_object(&EN2[i][j], 0, 0);
								EN2[i][j].alive = 0;
								reset_bullet(&BU);
								BULLET_ACTIVE = 0;
								SCORE+= 20;
								ENEMYS --;
								if(ENEMYS == 0){
									WIN = 1;
								}
								break;	
	
							}
							if (detect_collision(&BU, &EN3[0][j]))
							{
								EN3[0][j].alive = 0;
								draw_object(&EN3[0][j], 0, 0);
								reset_bullet(&BU);
								BULLET_ACTIVE = 0;
								SCORE+= 30;
								ENEMYS --;
								if(ENEMYS == 0){
									WIN = 1;
								}
								break;
							}
							/* colisão com escudos 
							*/

							if(detect_collision(&BU, &SH1)){
								lsh1--;
								switch (lsh1){
									case 4:
										draw_object(&SH1, 0,LGREEN );
										break;
									case 3:
										draw_object(&SH1, 0, YELLOW);
										break;
									case 2:
										draw_object(&SH1, 0, LYELLOW);
										break;
									case 1:
										draw_object(&SH1, 0, RED);
										break;
									case 0:
										reset_shield(&SH1);
										break;
								}
								reset_bullet(&BU);
								BULLET_ACTIVE = 0;
								continue;
							}

							if(detect_collision(&BU, &SH2)){
								lsh2--;
								switch (lsh2){
									case 4:
										draw_object(&SH2, 0,LGREEN );
										break;
									case 3:
										draw_object(&SH2, 0,YELLOW);
										break;
									case 2:
										draw_object(&SH2, 0, LYELLOW);
										break;
									case 1:
										
									draw_object(&SH2, 0, RED);
										break;
									case 0:
										reset_shield(&SH2);
										break;
								}
								reset_bullet(&BU);
								BULLET_ACTIVE = 0;
								continue;
							}

							if(detect_collision(&BU, &SH3) || detect_collision(&EBU[0], &SH3)){
								lsh3--;
								switch (lsh3){
									case 4:
										draw_object(&SH3, 0,LGREEN);
										break;
									case 3:
										draw_object(&SH3, 0,YELLOW);
										break;
									case 2:
										draw_object(&SH3, 0, LYELLOW);
										break;
									case 1:
										
									draw_object(&SH3, 0, RED);
										break;
									case 0:
										reset_shield(&SH3);
										break;
								}
								reset_bullet(&BU);
								BULLET_ACTIVE = 0;
								continue;
							}
							
							if(detect_collision(&BU, &SH4)){
								lsh4--;
								switch  (lsh4){
									case 4:
										draw_object(&SH4, 0,LGREEN );
										break;
									case 3:
										draw_object(&SH4, 0,YELLOW);
										break;
									case 2:
										draw_object(&SH4, 0, LYELLOW);
										break;
									case 1:
										
									draw_object(&SH4, 0, RED);
										break;
									case 0:
										reset_shield(&SH4);
										break;
								}
								
								reset_bullet(&BU);
								BULLET_ACTIVE = 0;
								continue;
							}
							
						}
					}
				}
			}

			// ===========================
			// Tiro dos inimigos (a cada 1 segundo)
			// ==========================
			/* Move e processa colisões das balas inimigas ativas */
			for (int kb = 0; kb < MAX_ENEMY_BULLETS; kb++)
			{
				if (!EBU[kb].alive)
					continue;
				move_object(&EBU[kb]);
				// saiu da tela //
				if (EBU[kb].posy + EBU[kb].spriteszy >= 190)
				{
					reset_bullet(&EBU[kb]);
					continue;
				}
				/* colisão com player */
				if (detect_collision(&EBU[kb], &PL))
				{
					reset_bullet(&EBU[kb]);
					/* Decrementar a vida real (`VIDA`) — `VIDA_ATUAL` guarda o último valor mostrado */
					if (VIDA > 1)
						VIDA--;
					else
					{
						GAME_OVER = 1;
						break;
					}
					continue;
				}
				/* colisão com escudos */
				if (detect_collision(&EBU[kb], &SH1) || detect_collision(&EBU[kb], &SH2) ||
					detect_collision(&EBU[kb], &SH3) || detect_collision(&EBU[kb], &SH4))
				{
					reset_bullet(&EBU[kb]);
					continue;
				}
			}

			ENEMY_BULLET--;
			if (ENEMY_BULLET <= 0)
			{
				// Chama a função tiro (usa pool interna) passando pool e matrizes de inimigos
				enemy_shoot(EBU, MAX_ENEMY_BULLETS, EN1, EN2, EN3);

				// Reinicia contagem
				ENEMY_BULLET = ENEMY_BULLET_TIMER;
			}




			delay_ms(50);
		}


		// ===========================
		// Se derrota
		// ==========================
		if(GAME_OVER){
			init_display();

		
			lsh1 = 5; lsh2 = 5; lsh3 = 5; lsh4 = 5;

			char SCORE_final[20];
			int_to_string(SCORE, SCORE_final);

			char ROUNDS_COUNT_str[20];
			int_to_string(ROUNDS, ROUNDS_COUNT_str);

			display_frectangle(0, 100, VGA_WIDTH, 40, BLACK);
			display_print("Game Over", 40, 90, 3, WHITE);
			display_print("SCORE", 120, 125, 1, WHITE);
			display_print(SCORE_final, 170, 125, 1, WHITE);
			display_print("ROUNDS:", 115, 140, 1, WHITE);
			display_print(ROUNDS_COUNT_str, 185, 140, 1, WHITE);

			while (!RESTART_REQUESTED)
			{
				display_print("Press UP_BUTTON key to restart", 35, 160, 1, WHITE);
				if (get_input() == KEY_UP)
				{
					RESTART_REQUESTED = 1;
				}
			}
			if (RESTART_REQUESTED)
			{
				// reseta variáveis de controle
				display_background(BLACK);
				ENEMYS = 50;
				ROUNDS = 1;
				ATTEMPT_COUNT++;
				PLAYER_SPEED = -3;
				SCORE_ATUAL = 0;
				SCORE = 0;
				VIDA = 3;
				VIDA_ATUAL = 3;
				START_GRACE = 3;
				ENEMY_BULLET_TIMER = 20;
				ENEMY_BULLET = 0;
				BULLET_ACTIVE = 0;
				RELOAD_TIMER = 0;
				GAME_OVER = 0;
				RESTART_REQUESTED = 0;
				ENEMIE_DIRECTION = 1;
				MOVE_DOWN = 0;
				FRAME_COUNT = 0;
			}

			main();
		}

		// ===========================
		// Se vitória
		// ==========================
		if(WIN){
			init_display();

			ROUNDS++;
			char ROUNDS_COUNT_str[20];
			int_to_string(ROUNDS, ROUNDS_COUNT_str);

			display_print("ROUND ", 50, 90, 3, WHITE);
			display_print(ROUNDS_COUNT_str, 180, 90, 3, WHITE);

			delay_ms(5000);
			display_background(BLACK);
				ENEMYS = 50;
				ATTEMPT_COUNT++;
				PLAYER_SPEED = -3;
				SCORE += SCORE_ATUAL;
				VIDA = VIDA_ATUAL ;
				if (VIDA < VIDA_MAX) VIDA++;
				START_GRACE = 3;
				ENEMY_BULLET_TIMER = 20;
				ENEMY_BULLET = 0;
				BULLET_ACTIVE = 0;
				RELOAD_TIMER = 0;
				GAME_OVER = 0;
				RESTART_REQUESTED = 0;
				ENEMIE_DIRECTION = 1;
				MOVE_DOWN = 0;
				FRAME_COUNT = 0;
				WIN = 0;

			// chama a main loop recursivamente
			main();
		}

	return 0;
}