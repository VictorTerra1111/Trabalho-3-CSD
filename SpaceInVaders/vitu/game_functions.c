#include "game_functions.h"
#include "game_sprites.h"

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
	obj->alive = 1;
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
	if (!obj1->alive || !obj2->alive) return 0;
	/* Considerar também o caso em que as bordas tocam exatamente
	   (encostar deve contar como colisão). Trocar < e > por <= e >=. */
	if (obj1->posx <= obj2->posx + obj2->spriteszx &&
		obj1->posx + obj1->spriteszx >= obj2->posx &&
		obj1->posy <= obj2->posy + obj2->spriteszy &&
		obj1->posy + obj1->spriteszy >= obj2->posy) return 1;

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

	void int_to_string(int num, char *str) {
    int i = 0;
    int is_negative = 0;
    
    // Tratar zero
    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }
    
    // Tratar negativos
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }
    
    // Converter dígitos (ordem reversa)
    while (num != 0) {
        str[i++] = (num % 10) + '0';
        num = num / 10;
    }
    
    // Adicionar sinal negativo
    if (is_negative) {
        str[i++] = '-';
    }
    
    str[i] = '\0';
    
    // Inverter string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }

	}

void reset_bullet(struct object_s *b) {
    draw_object(b, 0, 0);     // apaga onde está
    b->alive = 0;
    b->posy = VGA_HEIGHT;
}

void spawn_bullet(struct object_s *owner, struct object_s *b, int offsetx, int offsety, int dx, int dy, int spx, int spy) {
	/* Ativa e posiciona o bullet `b` relativo ao `owner`.
	   Presume que `b->sprite_frame` e dimensões já foram configuradas
	   (por exemplo com `init_object`). */
	/* apagar qualquer rastro anterior */
	draw_object(b, 0, 0);

	b->posx = owner->posx + offsetx;
	b->posy = owner->posy + offsety;
	b->dx = dx;
	b->dy = dy;
	b->speedx = spx;
	b->speedy = spy;
	b->speedxcnt = spx;
	b->speedycnt = spy;
	b->alive = 1;

	draw_object(b, 1, -1);
}

