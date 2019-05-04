#include <stdio.h>
#include <Windows.h>
#include <stdbool.h>
#include <conio.h>
#include <strsafe.h>
#include <cwchar>
#include <utilapiset.h>

typedef struct{
	int x;
	int y;
	int size;
	int velX;
	int velY;
	float angle;
	CHAR_INFO chFill;
	int pX;	
	int pY;	
}Ball;

typedef struct {	
	int x;
	int y;
	int height;
	int width;
	bool visible;
}Box;

#define WIN32_LEAN_AND_MEAN

#define BLOCKS_ON_TOP 7
#define offset 1

#define BOX 219
#define space 32
#define star 42

#define WINDOW_HEIGHT 50
#define WINDOW_WIDTH 110
#define BOX_WIDTH 14
#define BOX_HEIGHT 2
#define PLATFORM_WIDTH 20
#define PLATFORM_HEIGHT 1

#define VEL_X 1
#define VEL_X_FAST 2
#define VEL_Y 1
#define VEL_Y_FAST 2

#define P_SPEED 10

#define STD_BALL_SIZE 1
#define BIG_BALL_SIZE 3

#define SHRILL 1500

int HEIGHT, WIDTH, listLength;

void drawCharacterSquare(int size, int x, int y, char c);

void drawRect(int length, int width, int x, int y);

void drawCharacterRect(int height, int width, int x, int y, char c);

void drawSquare(int , int , int);

void clearScreen();

inline bool intersects(int, int, Box*);

void fillScreen(char);

void delay(int);

void setCursorPos(int , int );

Box *boxes, platform;

Ball ball;

int LOCX = 0, LOCY = 0;

char keyInput;

HANDLE std_out, std_in;         		          // Get standard output, input
CONSOLE_CURSOR_INFO cursorInfo;                          // Get Cursor Info
CONSOLE_SCREEN_BUFFER_INFO bufferInfo;			// Get current buffer's information	

int main(){

	
	//Do some basic setup for accessing the console and get console handles for input and output
	std_out =  GetStdHandle(STD_OUTPUT_HANDLE);
	std_in = GetStdHandle(STD_INPUT_HANDLE);

	//Input events
	INPUT_RECORD inputEvent;
	DWORD Event;

	if (!(SetConsoleMode(std_in, ENABLE_EXTENDED_FLAGS))){ 
		printf("Problem");
		exit(0);
	}
	
	if (!(SetConsoleMode(std_in, ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT))){ 
		printf("Problem with inputs");
		exit(0);
	}
	

	GetConsoleCursorInfo(std_out, &cursorInfo);             // Get cursorinfo from output
 	cursorInfo.bVisible = false;			       // Set flag visible.
 	SetConsoleCursorInfo(std_out, &cursorInfo);           // Apply changes		

	//Set console size
	SMALL_RECT myNewSize;

	myNewSize.Left = 0;
	myNewSize.Top = 0;
	myNewSize.Right = WINDOW_WIDTH;
	myNewSize.Bottom = WINDOW_HEIGHT;

	SetConsoleWindowInfo(std_out, true, &myNewSize);

	//Set display mode
	SetConsoleDisplayMode(std_out, CONSOLE_WINDOWED_MODE , NULL);
	
	//Set Title
	TCHAR title[MAX_PATH];
	TCHAR oldTitle[MAX_PATH];

	GetConsoleTitle(oldTitle, MAX_PATH);
	StringCchPrintf(title, MAX_PATH, TEXT("BRICK CHUNGUS"), oldTitle);
	SetConsoleTitle(title);
	
	//CONSOLE_FONT
	CONSOLE_FONT_INFOEX currentFont;
	currentFont.cbSize = sizeof(currentFont);
	currentFont.nFont = 0;
	currentFont.dwFontSize.X = 0;
	currentFont.dwFontSize.Y = 15;
	currentFont.FontFamily = FF_DONTCARE;
	currentFont.FontWeight = 1000;
	std::wcscpy(currentFont.FaceName, L"Consolas");
	SetCurrentConsoleFontEx(std_out, true, &currentFont);		
	
	//colours, using system call, can be faster using low level functions, see end of the file for ex.
	system("color fa");

	//useful variables for iteration in loops
	int i = 0;
	int j = 0;	
		
	// See the console buffer size.
 	GetConsoleScreenBufferInfo(std_out, &bufferInfo);
	HEIGHT = bufferInfo.dwSize.Y;
	WIDTH =  bufferInfo.dwSize.X;

	//Summmation of consecutive integers formula
	// 1 + 2 + 3 + 4 +5 + ... + BLOCKS_ON_TOP = SUM
	
	listLength = (BLOCKS_ON_TOP*(BLOCKS_ON_TOP + 1))/2;
	
	//Keep track of number of visible tiles
	int visibles = listLength;

	LOCX = (-((BOX_WIDTH + offset) * BLOCKS_ON_TOP) + WINDOW_WIDTH)/2;
	LOCY = 5;

	boxes = (Box*) malloc(sizeof(Box)*listLength);
	ball.x = WINDOW_WIDTH/2 - 30;
	ball.y = WINDOW_HEIGHT/2 + 5;		
	ball.velX = -VEL_X;
	ball.velY = -VEL_Y;	
	ball.size = STD_BALL_SIZE;
	
	platform.x = LOCX;
	platform.y = LOCY + 40;
	platform.width = PLATFORM_WIDTH;
	platform.height = PLATFORM_HEIGHT;
	platform.visible = true;

	clearScreen();
	
	//Initialise the boxes.					
	//i keeps track of horizontal span, and j keeps track of vertical span
	
	int tracker = BLOCKS_ON_TOP, previousCount = 0;

	for (int count = 0; count < listLength; count++){

		boxes[count].height = BOX_HEIGHT;
		boxes[count].width = BOX_WIDTH;
		boxes[count].x = LOCX + (BOX_WIDTH + offset)*i + (BOX_WIDTH/2)*j;
		boxes[count].y = LOCY + (BOX_HEIGHT + offset) * j ;
		boxes[count].visible = true;		
		i++;
		
		if ((count - previousCount + 1) == tracker) {
			
			tracker--;
			previousCount  = count + 1;	
			i=0;
			j++;
		}

	}		

	//Draw the boxes. Only need to draw them once since only the ball's position will be updated.
	for (int count = 0; count < listLength; count++){

		drawRect(boxes[count].height, boxes[count].width, boxes[count].x , boxes[count].y) ;						

	}

	//Set ball colour.
	SetConsoleTextAttribute(std_out, 244);

	//Draw the platform
	drawRect(platform.height, platform.width, platform.x, platform.y);
	
	bool reached = false;	

	i = j = 0;	

	//Game Loop
	while (visibles > 0) {

		if (visibles < 6 && !reached) {
			
			Beep(SHRILL/2, 1000);
			ball.velX = (ball.velX > 0) ? VEL_X_FAST : -VEL_X_FAST;
			ball.velY = (ball.velY > 0) ? VEL_Y_FAST : -VEL_Y_FAST;
			reached = true;

		}

		//HANDLE BOUNDS
		if (ball.x >= WINDOW_WIDTH && ball.velX > 0) {
			ball.velX = -ball.velX; 
			Beep( SHRILL, 30);
		}

		else if (ball.x <= 0 && ball.velX < 0) {
			ball.velX = -ball.velX; 
			Beep( SHRILL, 30);
		}

		if (ball.y >= WINDOW_HEIGHT && ball.velY > 0) { 
			ball.velY = -ball.velY;
			Beep( SHRILL, 300);
			goto end;
		}
		
		else if (ball.y <= 0 && ball.velY < 0) {
			 ball.velY = -ball.velY;
			Beep( SHRILL, 30);
		}

		//STORE PREVIOUS LOCATION
		ball.pX = ball.x;
		ball.pY = ball.y;

		//UPDATE LOCATION	
		ball.x += ball.velX;
		ball.y += ball.velY;

		//RENDER THE BALL
		setCursorPos(ball.x,ball.y);
		printf("%c", BOX);
		setCursorPos(ball.pX, ball.pY);
		printf("%c", space);		
			

		if (intersects(ball.x, ball.y, &platform)){
			
			ball.velY = -ball.velY;
		}
								
		for (j = 0; j < listLength; j++) {
			
			if ((boxes + j)->visible && intersects(ball.x, ball.y, (boxes+j))){ 
					
				Beep(SHRILL/2,15);
				(boxes + j)-> visible = false;
				visibles--;
				drawCharacterRect((boxes + j)->height, (boxes + j)->width, (boxes + j)->x, (boxes + j)->y, space);						
			
				if (ball.velX > 0 && (ball.velY < 0 || ball.velY > 0)) {
			
					ball.velY = -ball.velY;	
			
				}
				
				else if (ball.velX < 0 && (ball.velY < 0 || ball.velY > 0)) {
		
					ball.velY = -ball.velY;			
		
				}
			}			
		}		
		
		// See if arrows were pressed using _kbhit()

		if (kbhit())	{
		 	
			keyInput = getch();	

			//change platform location accordingly.
			if (keyInput == 'M'){		
				drawCharacterRect(platform.height,platform.width,platform.x,platform.y,space);
				platform.x += P_SPEED;						
				if (platform.x + platform.width >= WINDOW_WIDTH) platform.x = WINDOW_WIDTH - platform.width;					
				drawCharacterRect(platform.height,platform.width,platform.x,platform.y,BOX);
			}

			else if (keyInput == 'K'){
		
				drawCharacterRect(platform.height,platform.width,platform.x,platform.y,space);	
				platform.x -= P_SPEED;
				if (platform.x <= 0) platform.x = 0;							
				drawCharacterRect(platform.height,platform.width,platform.x,platform.y,BOX);
		
			}

		}		
		
		Sleep(16);			
	}
					
	
	cursorInfo.bVisible = true;			       // Set flag visible.
 	SetConsoleCursorInfo(std_out, &cursorInfo);           // Apply changes	
	
	goto end;

end:
	clearScreen();
	setCursorPos(WINDOW_WIDTH/2 - (strlen("GAME OVER :O PRESS CTRL+C TO EXIT")/2), WINDOW_HEIGHT/2);
	printf("GAME OVER :O ! PRESS CTRL+C TO EXIT");
	while (true) Sleep(100);
	
}


void drawRect(int height, int width, int x, int y){

	COORD cord;
	cord.X = x;
	cord.Y = y;

	int i = 0, j = 0;		
	
	for (i = 0; i < height; i++) 	{						
		
		for ( j = 0 ; j < width; j++) {						

			setCursorPos(cord.X + j, cord.Y + i);
			printf("%c", BOX);
			//printf("%d %d\n\n", cord->X + j, cord->X + i);
			
		}
				
	}

}


void drawCharacterRect(int height, int width, int x, int y, char c){

	COORD cord;
	cord.X = x;
	cord.Y = y;

	int i = 0, j = 0;		
	
	for (i = 0; i < height; i++) 	{						
		
		for ( j = 0 ; j < width; j++) {						

			setCursorPos(cord.X + j, cord.Y + i);
			printf("%c", c);						
		}
				
	}

}

inline void drawSquare(int size, int x, int y){			
	
	drawRect(size, size, x, y);

}

inline bool intersects (int x, int y, Box *myRect){
		
    if (x >= myRect->x && x <= (myRect->x + myRect->width) && y >= myRect->y && (y <= myRect->y + myRect->height)) return true;
	else return false;
}

void drawCharacterSquare(int size, int x, int y, char c){			
	
	drawCharacterRect(size,size,x,y,c);
}


void fillScreen(char character){

	COORD cord = {0 , 0};	
	GetConsoleScreenBufferInfo(std_out, &bufferInfo);
	DWORD consoleSize;
	consoleSize = bufferInfo.dwSize.X * bufferInfo.dwSize.Y;
	DWORD out = 0;
	FillConsoleOutputCharacter(std_out, (TCHAR) character, consoleSize, cord, &out);	
}

void clearScreen(){

	fillScreen(space);
}

void setCursorPos(int x, int y){
	
	GetConsoleScreenBufferInfo(std_out, &bufferInfo);
	bufferInfo.dwCursorPosition.X = x;	
	bufferInfo.dwCursorPosition.Y = y;		
	SetConsoleCursorPosition(std_out, bufferInfo.dwCursorPosition);		

}
