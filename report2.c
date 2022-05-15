typedef unsigned short hword;
typedef struct{
	hword x;
	hword y;
}point;

#define VRAM 0x06000000
#define RGB(r, g, b) ((b<<10) + (g<<5) + r)
#define KEY 0x04000130
#define KEY_A 0x0001
#define KEY_B 0x0002
#define KEY_DOWN 0x0080
#define KEY_UP 0x0040
#define KEY_LEFT 0x0020
#define KEY_RIGHT 0x0010
#define KEY_ALL 0x03ff
#include "8x8ascii.h"

hword tx, ty;
int count;
static hword cx, cy, bx, by;
int x;
int y;
point p;
unsigned char tmp;

hword WHITE = RGB(0x1f, 0x1f, 0x1f);
hword MAGENTA = RGB(0x1f, 0,0x1f);
hword BLACK = RGB(0, 0,0);
hword BLUE = RGB(0, 0, 0x1f);
hword RED = RGB(0x1f, 0, 0);
hword YELLOW = RGB(0x1f, 0x1f, 0);
int charExist[7][6];

void locate(hword, hword);
void print_ascii(unsigned char hword);
void print_ascii_white(unsigned char hword);
void prints(unsigned char *str);
void prints_white(unsigned char *str);
void cursor(point p, hword);
void drawPoint(hword, hword, hword);
void timer(hword);
void printChar(int);
hword mod(hword, hword);
hword divide(hword, hword);

int main(void){
	hword *ptr, *ptr2, key;
	int flagRight = 0, flagLeft = 0, flagUp = 0, flagDown = 0;

	ptr = (hword*)0x04000000;
	*ptr = 0x0f03;

	ptr2 = (hword*)KEY;
	*ptr2 = KEY_ALL;


	for (x = 0; x < 240; x++) {
		for (y = 0; y < 160; y++) {

			ptr = (hword*)VRAM + x + 240 * y;
			*ptr = WHITE;
		}
	}

	while (1) {
		key = ~*ptr2 & 0x03ff;

		switch (key) {
			case KEY_RIGHT:
				if (flagRight == 0) {
					flagRight = 1;
				}
				break;
			case KEY_LEFT:
				if (flagLeft == 0) {
					flagLeft = 1;
				}
				break;
			default:
				flagRight = 0;
				flagLeft = 0;
				break;
		}
	}
	return 0;
}
void locate(hword cx, hword cy){
		p.x = cx*8;		
		p.y = cy*8;
}
void drawPoint(hword x, hword y, hword color){
	hword *ptr;
	ptr = (hword*)VRAM + x + 240*y;
	*ptr = color;
}
void print_ascii(unsigned char ascii_num){

	int nx, ny;
	for(ny = 0; ny<8; ny++){
		tmp = 0x80;
		for(nx = 0; nx<8; nx++){
			if((char8x8[ascii_num][ny]&tmp) == tmp){
				drawPoint((p.x + nx), (p.y + ny), MAGENTA);
			}else{
				drawPoint((p.x + nx), (p.y + ny), BLACK);
			}
			tmp = tmp>>1;
		}

	}
}
void print_ascii_white(unsigned char ascii_num){

	int nx, ny;
	for(ny = 0; ny<8; ny++){
		tmp = 0x80;
		for(nx = 0; nx<8; nx++){
			if((char8x8[ascii_num][ny]&tmp) == tmp){
				drawPoint((p.x + nx), (p.y + ny), WHITE);
			}else{
				drawPoint((p.x + nx), (p.y + ny), BLACK);
			}
			tmp = tmp>>1;
		}

	}
}
void prints(unsigned char *str){
		print_ascii(*str);
}
void prints_white(unsigned char *str){
		print_ascii_white(*str);
		
}
void timer(hword time){
	int i,k;
	for(i=0; i<500; i++){
		for(k=0; k<time*100; k++){
		}
	}
}
void cursor(point p, hword color){
		int i;
		for(i = 0; i<8; i++){
			drawPoint(p.x+i, p.y+7, color);
		}
}
void printChar(int count){
		hword num;
		hword tmp;
		if(count<0){
			tmp = 255 + count;
		}else{
			tmp = count;
		}
		num = charExist[bx][by]+tmp;
		num = mod(num, 255);
		locate(bx,by);
		prints(" ");
		print_ascii(num);
		charExist[bx][by] = num;
}

hword mod(hword a, hword b){
	
	while(1){
		if(a >= b){
			a-= b;
		}else{
			break;
		}
	}
	return a;
}
hword divide(hword a, hword b){
	int tmp = 0;
	while(1){
		if(a >= b){
			a-= b;
			tmp++;
		}else{
			break;
		}
	}
	return tmp;
}