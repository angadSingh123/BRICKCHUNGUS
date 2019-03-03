#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include <stdbool.h>



struct Box {
	
	int x;
	int y;
	int size;

};

#define WIN32_LEAN_AND_MEAN

#define box 219
//219 is the box
//42 is the star

void drawSquare(int , int , int);

void clearScreen();

void delay(int);

void setCursorPos(int , int );

HANDLE std_out;         				  // Get standard output		
CONSOLE_CURSOR_INFO cursorInfo;                          // Get Cursor Info
CONSOLE_SCREEN_BUFFER_INFO bufferInfo;			// Get current buffer's information	

int main(){
	
	std_out =  GetStdHandle(STD_OUTPUT_HANDLE);

	int i = 0;
	int length = 50;	
 	
 	GetConsoleCursorInfo(std_out, &cursorInfo);             // Get cursorinfo from output
 	cursorInfo.bVisible = false; 			       // Set flag visible.
 	SetConsoleCursorInfo(std_out, &cursorInfo);           // Apply changes
	

					
	for (; i < length; i++){
							
		drawSquare(3, i, 10);		
		drawSquare(3, i+10, 10);
		//delay(35);	
		Sleep(35);
		if (i != (length - 1)) clearScreen();									
	}
		
	cursorInfo.bVisible = true;			       // Set flag visible.
 	SetConsoleCursorInfo(std_out, &cursorInfo);           // Apply changes
	FreeConsole();
	

}

void delay(int milliSeconds) {

	clock_t startTime = clock();
	
	while (clock() - startTime < (milliSeconds));
}


void drawSquare(int size, int x, int y){			
	
	COORD cord;
	cord.X = x;
	cord.Y = y;

	int i = 0, j = 0;		
	
	for (i = 0; i < size; i++) 	{						
		
		for ( j = 0 ; j < size; j++) {						

			setCursorPos(cord.X + i, cord.Y + j);
			printf("%c", box);
			//printf("%d %d\n\n", cord->X + j, cord->X + i);
			
		}
				
	}

}

void drawCharacterSquare(int size, int x, int y, char c){			
	
	COORD cord;
	cord.X = x;
	cord.Y = y;

	int i = 0, j = 0;		
	
	for (i = 0; i < size; i++) 	{						
		
		for ( j = 0 ; j < size; j++) {						

			setCursorPos(cord.X + i, cord.Y + j);
			printf("%c", c);
			//printf("%d %d\n\n", cord->X + j, cord->X + i);
			
		}
				
	}

}


void clearScreen(){


	COORD cord = {0 , 0};	
	GetConsoleScreenBufferInfo(std_out, &bufferInfo);
	DWORD consoleSize;
	consoleSize = bufferInfo.dwSize.X * bufferInfo.dwSize.Y;
	DWORD out = 0;
	FillConsoleOutputCharacter(std_out, (TCHAR) ' ', consoleSize, cord, &out);	

}

void fillScreen(char character){

	COORD cord = {0 , 0};	
	GetConsoleScreenBufferInfo(std_out, &bufferInfo);
	DWORD consoleSize;
	consoleSize = bufferInfo.dwSize.X * bufferInfo.dwSize.Y;
	DWORD out = 0;
	FillConsoleOutputCharacter(std_out, (TCHAR) character, consoleSize, cord, &out);	

}


void setCursorPos(int x, int y){
	
	GetConsoleScreenBufferInfo(std_out, &bufferInfo);
	bufferInfo.dwCursorPosition.X = x;	
	bufferInfo.dwCursorPosition.Y = y;		
	SetConsoleCursorPosition(std_out, bufferInfo.dwCursorPosition);		

}

//Debugging
//printf("Set curr pos to %d %d\n", bufferInfo.dwCursorPosition.X , bufferInfo.dwCursorPosition.Y );
//printf("%d  %d\n", cord.X, cord.Y);
//printf("Set curr pos to %d %d\n", cord.X + j, cord.Y + i);
