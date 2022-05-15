#include "gba1.h"
#include "8x8ascii.h"

#define KEY_IN (~(*(hword *) KEY_STATUS) & KEY_ALL)
#define gameMode (~(*(hword *) KEY_STATUS) & KEY_ALL)
#define CURRENT_TIME *((unsigned short *)0x0400010C)

hword* ptr, * ptr2;
unsigned char tmp;

point p;
int x, y;
hword cx, cy;

/* 関数のプロトタイプ宣言 */
void display_time(hword);
void set_timer(void);
void drawCircle(hword, hword, hword, hword);
void drawCircle2(hword, hword, hword, hword);
void drawPoint(hword, hword, hword);
void print_ascii(unsigned char);
void print_ascii_RGB(unsigned char, hword);
void prints(byte* str);
void prints_RGB(byte* str, hword);
void locate(hword, hword);
void locate4grid(hword, hword);
void drawLine(hword, hword, hword, hword, hword);
void printResult(hword);//print out the game results
void drawGrid(void);//draws grid for the game 
void playerExist(hword, hword);
int checkWinner(hword player);
void clearScreen(void);
void clearGame(void);
void exitScreen(void); 
void startScreen(void);
hword mod(hword, hword);
hword div(hword, hword);
hword switchPlayer(hword); // switch turn 

hword CYAN = BGR(0, 0x1f, 0x1f);
hword RED = BGR(0x1f, 0, 0);
hword GREEN = BGR(0, 0x1f, 0);
hword BLUE = BGR(0, 0, 0x1f);
hword WHITE = BGR(0x1f, 0x1f, 0x1f);
hword BLACK = BGR(0, 0, 0);
hword YELLOW = BGR(0x1f, 0x1f, 0);
hword MAZENTA = BGR(0x1f, 0, 0x1f);
hword ORANGE = BGR(0x1f, 0x1b, 0);
hword lightYELLOW = BGR(0x0a, 0x0a, 0);
hword lightGREEN = BGR(0, 0x1e, 0x0f);
hword medBLUE = BGR(0x0f, 0x0e, 0x1c);

int grid[7][6];

hword gx, gy; //coordinate for the connect4 grid (7,6)
hword mode; //set mode(1:MENU, 2:GAME, 3:RESULT)
hword color;
hword player;

int main(void) {

	volatile unsigned short time;
	unsigned short timer_on = 0;

	/* 画面初期化ルーチン */
	*((unsigned short*)IOBASE) = 0xF03;

	hword flagDown = 0;
	hword flagUp = 0;
	hword flagRight = 0;
	hword flagLeft = 0;
	hword flagA = 0;

	mode = 1;
	gx = 0;
	gy = 0;
	player = 1;

	while (1) {

		if (mode == 1) {
			startScreen();
			switch (gameMode) {
			case KEY_START:
				mode++;
				clearScreen();
				break;
			}
		}
		else if (mode == 2) {
			drawGrid();
			locate4grid(gx, gy);
			drawCircle2(gx, gy, lightYELLOW, player);
			switch (KEY_IN) {

			case KEY_DOWN:
				if (flagDown == 0) {
					flagDown = 1;
					if (grid[gx][gy] == 0) {
						drawCircle2(gx, gy, BLACK, player);
					}
					else {
						playerExist(gx, gy);
					}
					if (gy >= 5) {
						gy = 5;
					}
					else {
						gy++;
					}
					locate4grid(gx, gy);
					drawCircle2(gx, gy, lightYELLOW, player);
				}
				break;

			case KEY_UP:
				if (flagUp == 0) {
					flagUp = 1;
					if (grid[gx][gy] == 0) {
						drawCircle2(gx, gy, BLACK, player);
					}
					else {
						playerExist(gx, gy);
					}
					if (gy <= 0) {
						gy = 0;
					}
					else {
						gy--;
					}

					locate4grid(gx, gy);
					drawCircle2(gx, gy, lightYELLOW, player);
				}
				break;

			case KEY_LEFT:
				if (flagLeft == 0) {
					flagLeft = 1;
					if (grid[gx][gy] == 0) {
						drawCircle2(gx, gy, BLACK, player);
					}
					else {
						playerExist(gx, gy);
					}
					if (gx <= 0) {
						gx = 0;
					}
					else {
						gx--;
					}
					locate4grid(gx, gy);
					drawCircle2(gx, gy, lightYELLOW, player);
				}
				break;

			case KEY_RIGHT:
				if (flagRight == 0) {
					flagRight = 1;
					if (grid[gx][gy] == 0) {
						drawCircle2(gx, gy, BLACK, player);
					}
					else {
						playerExist(gx, gy);
					}
					if (gx >= 6) {
						gx = 6;
					}
					else {
						gx++;
					}
					locate4grid(gx, gy);
					drawCircle2(gx, gy, lightYELLOW, player);
				}
				break;
			case KEY_A:
				if (flagA == 0) {
					flagA = 1;
					if (player == 1) {
						color = RED;
					}
					else if (player == 2) {
						color = YELLOW;
					}
					if (grid[gx][gy] == 0) {
						drawCircle2(gx, gy, color, player);
						grid[gx][gy] = player;
						if (checkWinner(player) == 3) {
							mode = 3;
							break;
						}
						else if(checkWinner(player)==0) {
							if (player == 1) {
								player = 2;
							}
							else if (player == 2) {
								player = 1;
							}
						}
						
					}

				}
				break;
			default:
				flagDown = 0;
				flagUp = 0;
				flagRight = 0;
				flagLeft = 0;
				flagA = 0;
				break;
			}

		}
		else if (mode == 3) {
			break;
		}
	}
	printResult(player);
	while (1) {
		switch (KEY_IN) {
		case KEY_START:
			clearScreen();
			clearGame();
			main();
			break;
		case KEY_SELECT:
			exitScreen();
			return 0;
		}
	}
	return 0;

}


void display_time(hword val) {

	byte char_data[] = "0123456789";
	byte buf[6];
	hword tmp;
	int i;

	/* 入力値valの桁数(i+1)設定 (i=3の場合4桁表示)*/
	i = 3;

	/* 文字列の最後にNULLコード挿入 */
	buf[i + 1] = 0;

	/* 最下位桁の文字（10で割った余り）を挿入し，入力値を10で割る */
	for (; i >= 0; i--) {
		buf[i] = char_data[mod(val, 10)];
		val = div(val, 10);
	}

	/* 文字列全体を表示 */
	prints(buf);

	return;
}
void set_timer(void) {
	/* タイマ初期化ルーチン */
	*((unsigned short*)0x04000100) = (0xFFFF - 1780);
	*((unsigned short*)0x04000104) = (0xFFFF - 100);
	*((unsigned short*)0x04000108) = (0xFFFF - 10);
	*((unsigned short*)0x0400010C) = 0x0000;

	*((unsigned short*)0x04000102) = 0x0000; //timer deactivate
	*((unsigned short*)0x04000106) = 0x0084;
	*((unsigned short*)0x0400010A) = 0x00c4; //activate timer2 to interrupt
	*((unsigned short*)0x0400010E) = 0x0084;
}

void print_ascii(unsigned char ascii_num) {

	int nx, ny;
	for (ny = 0; ny < 8; ny++) {
		tmp = 0x80;
		for (nx = 0; nx < 8; nx++) {
			if ((char8x8[ascii_num][ny] & tmp) == tmp) {
				drawPoint((p.x + nx), (p.y + ny), WHITE);
			}
			else {
				drawPoint((p.x + nx), (p.y + ny), BLACK);
			}
			tmp = tmp >> 1;
		}

	}
}
void print_ascii_RGB(unsigned char ascii_num, hword color) {

	int nx, ny;
	for (ny = 0; ny < 8; ny++) {
		tmp = 0x80;
		for (nx = 0; nx < 8; nx++) {
			if ((char8x8[ascii_num][ny] & tmp) == tmp) {
				drawPoint((p.x + nx), (p.y + ny), color);
			}
			else {
				drawPoint((p.x + nx), (p.y + ny), BLACK);
			}
			tmp = tmp >> 1;
		}

	}
}
void drawPoint(hword x, hword y, hword color) {
	ptr = (hword*)VRAM + x + 240 * y;
	*ptr = color;
}
void locate(hword cx, hword cy) {
	p.x = cx * 8;
	p.y = cy * 8;
}
void locate4grid(hword cx, hword cy) {
	p.x = 22 * cx + 43;
	p.y = 22 * cy + 14;
}
void prints(byte* str) {

	while (*str) {
		print_ascii(*str++);
		p.x = p.x + CHAR_WIDTH;			/* 表示位置を1文字右へ */
	}
}
void prints_RGB(byte* str, hword color) {

	while (*str) {
		print_ascii_RGB(*str++, color);
		p.x = p.x + CHAR_WIDTH;			/* 表示位置を1文字右へ */
	}
}

void drawLine(hword xa, hword ya, hword xb, hword yb, hword color) {
	hword xc, yc;

	xc = (xa + xb) / 2;
	yc = (ya + yb) / 2;

	drawPoint(xc, yc, color);

	if (((xa - xc) >= 1) && ((xc - xb) >= 1)) {
		drawLine(xa, ya, xc, yc, color);
		drawLine(xc, yc, xb, yb, color);
	}
	else if (((ya - yc) >= 1) && ((yc - yb) >= 1)) {
		drawLine(xa, ya, xc, yc, color);
		drawLine(xc, yc, xb, yb, color);
	}
	else {
		return;
	}
}
hword div(hword dividened, hword divisor) {

	hword quotient = 0;			/* 商 */

	while (1) {
		if (dividened >= divisor) {
			dividened = dividened - divisor;
			quotient++;
		}
		else {
			break;
		}
	}
	return quotient;
}

hword mod(hword dividened, hword divisor) {

	hword quotient = 0;			/* 商 */

	while (1) {
		if (dividened >= divisor) {
			dividened = dividened - divisor;
			quotient++;
		}
		else {
			break;
		}
	}
	return dividened;
}
void drawCircle(hword x, hword y, hword color, hword player) {
	hword r = 9;
	hword startX, startY;
	hword* start;
	hword i, k;

	startX = x - r;
	startY = y - r;
	start = (hword*)VRAM + startX + 240 * startY;

	for (i = 0; i < 2 * r; i++) {
		for (k = 0; k < 2 * r; k++) {
			if (i * i + k * k + r * r < 2 * i * r + 2 * k * r) {
				ptr = start + i + 240 * k;
				*ptr = color;
			}
		}
	}
	grid[x][y] = player;
}
void drawCircle2(hword x, hword y, hword color, hword player) {
	hword r = 9;
	hword startX, startY;
	hword* start;
	hword i, k;

	startX = p.x + 2;
	startY = p.y + 2;
	start = (hword*)VRAM + startX + 240 * startY;

	for (i = 0; i < 2 * r; i++) {
		for (k = 0; k < 2 * r; k++) {
			if (i * i + k * k + r * r < 2 * i * r + 2 * k * r) {
				ptr = start + i + 240 * k;
				*ptr = color;
			}
		}
	}
}
void printResult(hword player) {
	clearScreen();
	switchPlayer(player);
	hword cx, cy;
	hword color;
	cx = 7, cy = 6;
	locate(cx, cy);
	if (player == 2) {
		prints("Player  has won!");
		locate(13, 6); prints_RGB("2", YELLOW);
		color = YELLOW;
	}
	else if (player == 1) {
		prints("Player  has WON!");
		locate(13, 6); prints_RGB("1", RED);
		color = RED;
	}
	locate(7, 3); prints_RGB("CONGRATULATION!!", color);

	locate(3, 11); prints("to CONTINUE, press");
	locate(22, 11); prints_RGB("START", CYAN);

	locate(3, 13); prints("to EXIT, press");
	locate(18, 13); prints_RGB("SELECT", MAZENTA);

	hword x, y;
	for (y = 4; y < 158; y++) {
		if (mod(y, 8) == 1 || mod(y, 8) == 2 || mod(y, 8) == 7 || mod(y, 8) == 0) {
			drawPoint(2, y, color);
		}
		if (mod(y, 8) == 2 || mod(y, 8) == 3 || mod(y, 8) == 6 || mod(y, 8) == 7) {
			drawPoint(3, y, color);
		}
		if (mod(y, 8) == 3 || mod(y, 8) == 4 || mod(y, 8) == 5 || mod(y, 8) == 6) {
			drawPoint(4, y, color);
		}
		if (mod(y, 8) == 0 || mod(y, 8) == 1) {
			drawPoint(1, y, color);
		}
		if (mod(y, 8) == 4 || mod(y, 8) == 5) {
			drawPoint(5, y, color);
		}
	}
	for (y = 6; y < 158; y++) {
		if (mod(y, 8) == 1 || mod(y, 8) == 2 || mod(y, 8) == 7 || mod(y, 8) == 0) {
			drawPoint(236, y, color);
		}
		if (mod(y, 8) == 2 || mod(y, 8) == 3 || mod(y, 8) == 6 || mod(y, 8) == 7) {
			drawPoint(237, y, color);
		}
		if (mod(y, 8) == 3 || mod(y, 8) == 4 || mod(y, 8) == 5 || mod(y, 8) == 6) {
			drawPoint(238, y, color);
		}
		if (mod(y, 8) == 0 || mod(y, 8) == 1) {
			drawPoint(235, y, color);
		}
		if (mod(y, 8) == 4 || mod(y, 8) == 5) {
			drawPoint(239, y, color);
		}
	}
	for (x = 6; x < 238; x++) {
		if (mod(x, 8) == 1 || mod(x, 8) == 2 || mod(x, 8) == 7 || mod(x, 8) == 0) {
			drawPoint(x, 2, color);
		}
		if (mod(x, 8) == 2 || mod(x, 8) == 3 || mod(x, 8) == 6 || mod(x, 8) == 7) {
			drawPoint(x, 3, color);
		}
		if (mod(x, 8) == 3 || mod(x, 8) == 4 || mod(x, 8) == 5 || mod(x, 8) == 6) {
			drawPoint(x, 4, color);
		}
		if (mod(x, 8) == 0 || mod(x, 8) == 1) {
			drawPoint(x, 1, color);
		}
		if (mod(x, 8) == 4 || mod(x, 8) == 5) {
			drawPoint(x, 5, color);
		}
	}
	for (x = 6; x < 240; x++) {
		if (mod(x, 8) == 1 || mod(x, 8) == 2 || mod(x, 8) == 7 || mod(x, 8) == 0) {
			drawPoint(x, 156, color);
		}
		if (mod(x, 8) == 2 || mod(x, 8) == 3 || mod(x, 8) == 6 || mod(x, 8) == 7) {
			drawPoint(x, 157, color);
		}
		if (mod(x, 8) == 3 || mod(x, 8) == 4 || mod(x, 8) == 5 || mod(x, 8) == 6) {
			drawPoint(x, 158, color);
		}
		if (mod(x, 8) == 0 || mod(x, 8) == 1) {
			drawPoint(x, 155, color);
		}
		if (mod(x, 8) == 4 || mod(x, 8) == 5) {
			drawPoint(x, 159, color);
		}
	}

}
void drawGrid(void) {
	hword x, y;
	for (y = 0; y < 8; y++) {
		x = 43;
		while (x <= LCD_WIDTH - 43) {
			drawPoint(x, 22 * y + 14, WHITE);
			x++;
		}
	}
	for (x = 0; x < 8; x++) {
		y = 14;
		while (y <= LCD_HEIGHT - 14) {
			drawPoint(22 * x + 43, y, WHITE);
			y++;
		}
	}
}
hword switchPlayer(hword player) {
	if (player == 1) {
		player = 2;
	}
	else if (player == 2) {
		player = 1;
	}
	return player;
}
void playerExist(hword x, hword y) {
	hword color;
	hword player;
	player = grid[x][y];
	if (player == 1) {
		color = RED;
	}
	else if (player == 2) {
		color = YELLOW;
	}
	drawCircle2(x, y, color, player);
}
int checkWinner(hword player) {
	for (y = 0; y < 6; y++) {
		for (x = 0; x < 7; x++) {
			//horizantal check
			if (grid[x][y] == player && grid[x + 1][y] == player
				&& grid[x + 2][y] == player && grid[x + 3][y] == player) {
				return 3;
			}

		}
	}
	for (y = 0; y < 6; y++) {
		for (x = 0; x < 7; x++) {
			//vertical check
			if (grid[x][y] == player && grid[x][y + 1] == player
				&& grid[x][y + 2] == player && grid[x][y + 3] == player) {
				return 3;
			}
		}
	}
	for (y = 0; y < 6; y++) {
		for (x = 0; x < 7; x++) {
			//leftdown diagonal check
			if (grid[x][y] == player && grid[x - 1][y + 1] == player
				&& grid[x - 2][y + 2] == player && grid[x - 3][y + 3] == player) {
				return 3;
			}
		}
	}
	for (y = 0; y < 6; y++) {
		for (x = 0; x < 7; x++) {
			//rightdown diagonal check
			if (grid[x][y] == player && grid[x - 1][y - 1] == player
				&& grid[x - 2][y - 2] == player && grid[x - 3][y - 3] == player) {
				return 3;
			}
		}
	}
	return 0;
}
void clearScreen(void) {
	hword i, k;
	for (i = 0; i < LCD_HEIGHT; i++) {
		for (k = 0; k < LCD_WIDTH; k++) {
			drawPoint(k, i, BLACK);
		}
	}
}
void clearGame(void) {
	hword x, y;
	for (x = 0; x < 6; x++) {
		for (y = 0; y < 7; y++) {
			grid[x][y] = 0;
		}
	}
}
void exitScreen(void) {
	clearScreen();
	locate(9, 7); prints("Thank You");
	locate(9, 9); prints("for Playing!!");
	hword x,y;
	for (y = 2; y < 15; y++) {
		locate(4, y);  print_ascii_RGB(186, medBLUE);
		locate(26, y); print_ascii_RGB(186, medBLUE);
	}
	for (y = 3; y < 14; y++) {
		locate(5, y);  print_ascii_RGB(178, lightGREEN);
		locate(25, y); print_ascii_RGB(178, lightGREEN);
	}
	for (y = 4; y < 13; y++) {
		locate(6, y);  print_ascii_RGB(176, CYAN);
		locate(24, y); print_ascii_RGB(176, CYAN);
	}
	for (y = 5; y < 12; y++) {
		locate(7, y);  print_ascii_RGB(177, MAZENTA);
		locate(23, y); print_ascii_RGB(177, MAZENTA);
	}
	for (x = 4; x < 26; x++) {
		locate(x, 1);  print_ascii_RGB(205, medBLUE);
		locate(x, 15); print_ascii_RGB(205, medBLUE);
	}
	for (x = 5; x < 26; x++) {
		locate(x, 2);  print_ascii_RGB(178, lightGREEN);
		locate(x, 14); print_ascii_RGB(178, lightGREEN);
	}
	for (x = 6; x < 25; x++) {
		locate(x, 3);  print_ascii_RGB(176, CYAN);
		locate(x, 13); print_ascii_RGB(176, CYAN);
	}
	for (x = 7; x < 24; x++) {
		locate(x, 4);  print_ascii_RGB(177, MAZENTA);
		locate(x, 12); print_ascii_RGB(177, MAZENTA);
	}
	locate(4, 1); print_ascii_RGB(201, medBLUE);
	locate(26,1); print_ascii_RGB(187, medBLUE);
	locate(4, 15); print_ascii_RGB(200, medBLUE);
	locate(26, 15); print_ascii_RGB(188, medBLUE);
	
	locate(6, 17); prints_RGB("Created by Sota Kataoka",CYAN); 
	locate(19, 18); prints_RGB("1116201021", CYAN);

	drawLine(233, 133, 45, 133, RED);
	drawLine(233, 153, 45, 153, RED);
	drawLine(233, 154, 233, 133, RED);
	drawLine(45, 153, 45, 133, RED);

	drawLine(235, 131, 43, 131, CYAN);
	drawLine(236, 155, 43, 155, CYAN);
	drawLine(235, 155, 235, 131, CYAN);
	drawLine(43, 155, 43, 131, CYAN);

	drawLine(237, 129, 41, 129, RED);
	drawLine(237, 157, 41, 157, RED);
	drawLine(237, 158, 237, 129, RED);
	drawLine(41, 157, 41, 129, RED);

}
void startScreen(void) {
	locate(2, 3);  prints("connect 4 dots vertically,");
	locate(2, 4);  prints("horizontally, or diagonally");
	locate(8, 6);  prints("to");
	locate(15, 6);  prints("a game");
	locate(11, 6);  prints_RGB("WIN", ORANGE);
	locate(2, 9);  prints("Player");
	locate(10, 9);  prints("will be");
	locate(22, 9);  prints("dots");
	locate(8, 9); prints_RGB("1", RED);
	locate(18, 9); prints_RGB("RED", RED);
	locate(2, 10);  prints("Player");
	locate(10, 10);  prints("will be");
	locate(25, 10);  prints("dots");
	locate(8, 10); prints_RGB("2", YELLOW);
	locate(18, 10); prints_RGB("YELLOW", YELLOW);
	locate(3, 13); prints("use");
	locate(13, 13); prints("to move cursor");
	locate(7, 13); prints_RGB("D-pad", medBLUE);
	locate(3, 15); prints("press");
	locate(11, 15); prints("to place a dot");
	locate(9, 15); prints_RGB("A", GREEN);
	locate(3, 17); prints("press");
	locate(15, 17); prints("to play game");
	locate(9, 17); prints_RGB("START", CYAN);

	locate(0, 0); print_ascii_RGB(218, MAZENTA);
	locate(0, 19); print_ascii_RGB(192, BLUE);
	locate(29, 0); print_ascii_RGB(191, BLUE);
	locate(29, 19); print_ascii_RGB(217, MAZENTA);
	for (x = 1; x < 29; x++) {
		if (mod(x, 2) == 0) {
			locate(x, 0);  print_ascii_RGB(196, MAZENTA);
			locate(x, 19); print_ascii_RGB(196, BLUE);
		}
		else if (mod(x, 2) == 1) {
			locate(x, 0);  print_ascii_RGB(196, BLUE);
			locate(x, 19); print_ascii_RGB(196, MAZENTA);
		}
		
	}
	for (y = 1; y < 19; y++) {
		if (mod(y, 2) == 0) {
			locate(0, y);  print_ascii_RGB(179, MAZENTA);
			locate(29, y); print_ascii_RGB(179, BLUE);
		}
		else if (mod(y, 2) == 1) {
			locate(0, y);  print_ascii_RGB(179, BLUE);
			locate(29, y); print_ascii_RGB(179, MAZENTA);
		}
	}
}