#include <hf-risc.h>

#define SW_AXI_BASE			0xe4a90000

#define SW_AXI_STATUS			(*(volatile uint32_t *)(SW_AXI_BASE + 0x010))
#define SW_AXI_SDATA			(*(volatile uint32_t *)(SW_AXI_BASE + 0x020))

#define SW_AXI_STREADY			(1 << 0)
#define SW_AXI_STVALID			(1 << 1)


uint8_t sw_axi()
{
	uint8_t data;
	
	data = SW_AXI_SDATA;
	while (!(SW_AXI_STATUS & SW_AXI_STVALID));
	data = SW_AXI_SDATA;
	
	return data;
}

int main(){

	int key_active = 0; 
	int released = 0;

	unsigned char old_code = 0;
	
	printf("Iniciando leitura!\n");
	
	while (1) {
		unsigned char code = sw_axi();

		if (code == 0xF0) {
			released = 1;
			key_active = 0;
			old_code = 0; 
			continue;
		}

		if ((key_active && !released )|| code == 0xE0) {
			continue;
		}

		old_code = code;

		if (!released) {
			char ch = 0;

			switch (code) {
				case 0x16: ch = '1'; break;
				case 0x1E: ch = '2'; break;
				case 0x26: ch = '3'; break;
				case 0x25: ch = '4'; break;
				case 0x2E: ch = '5'; break;
				case 0x36: ch = '6'; break;
				case 0x3D: ch = '7'; break;
				case 0x3E: ch = '8'; break;
				case 0x46: ch = '9'; break;
				case 0x45: ch = '0'; break;
	
				case 0x15: ch = 'q'; break; case 0x1D: ch = 'w'; break; case 0x24: ch = 'e'; break; case 0x2D: ch = 'r'; break; case 0x2C: ch = 't'; break;
				case 0x35: ch = 'y'; break; case 0x3C: ch = 'u'; break; case 0x43: ch = 'i'; break; case 0x44: ch = 'o'; break; case 0x4D: ch = 'p'; break;
				case 0x1C: ch = 'a'; break; case 0x1B: ch = 's'; break; case 0x23: ch = 'd'; break; case 0x2B: ch = 'f'; break; case 0x34: ch = 'g'; break;
				case 0x33: ch = 'h'; break; case 0x3B: ch = 'j'; break; case 0x42: ch = 'k'; break; case 0x4B: ch = 'l'; break;
				case 0x1A: ch = 'z'; break; case 0x22: ch = 'x'; break; case 0x21: ch = 'c'; break; case 0x2A: ch = 'v'; break; case 0x32: ch = 'b'; break;
				case 0x31: ch = 'n'; break; case 0x3A: ch = 'm'; break;

				default: ch = 0; break;
			}

			if (ch) {
				putchar(ch);
				key_active = 1; 
			}
		}

		released = 0;
	}

}
