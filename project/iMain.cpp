# include "iGraphics.h"
#include <time.h>
#include <stdlib.h>


struct polygons
{
	double base_x[10];
	double base_y[10];
	int r,g,b;
	int vertices;
} polygon[7] =
	{
			{	//straight tetromino
				{384, 412, 440, 468, 496, 496, 468, 440, 412, 384}, 
				{552, 552, 552, 552, 552, 580, 580, 580, 580, 580}, 
					135, 206, 235,
					10
			},
			{	//square tetromino
				{412,440,468,468,468,440,412,412}, 
				{524,524,524,552,580,580,580,552}, 
					255, 179, 67,
					8
			}, 
			{	//left L tetromino
				{440,412,412,440,468,468,468,468,440,440}, 
				{524,524,496,496,496,524,552,580,580,552}, 
					0, 0, 139,
					10
			},
			{	//right L tetromino
				{440,440,440,412,412,412,412,440,468,468}, 
				{524,552,580,580,552,524,496,496,496,524}, 
					255, 222, 33,
					10
			},
			
	};

int chosen_poly_no = rand() % 4;
int n = polygon[chosen_poly_no].vertices;

double poly_x[10];
double poly_y[10];

double shift_x = 0;
double shift_y = 0;


struct block
{
    double x;
    double y;
} occupied_blocks[300];

int no_blocks=0;

void spawn()
{	
	chosen_poly_no = rand() % 4;
	
	shift_x = 0;
	shift_y = 0;
	n = polygon[chosen_poly_no].vertices;

	for (int i = 0; i < n; i++)
	{
		poly_x[i] = polygon[chosen_poly_no].base_x[i];
		poly_y[i] = polygon[chosen_poly_no].base_y[i];
	}
}
//brings the polygon down automatically
void gravity();


// left collision check ---parameter: x co-ordinate array of vertices
int collision_left(double poly_x[])
{
	int i;
	int collision=0;
	for(i=0;i<n;i++)
	{
		if(poly_x[i]<=300)
		{
			collision=1;
			break;
		}
	}
	return collision;
}
//right collision check ---parameter: x co-ordinate array of vertices
int collision_right(double poly_x[])
{
	int i;
	int collision=0;
	for(i=0;i<n;i++)
	{
		if(poly_x[i]>=580)
		{
			collision=1;
			break;
		}
	}
	return collision;
}

int collision_grid_x(double poly_x[],double poly_y[])
{
	int i,j;
	int collision=0;
	for(i=0;i<n;i++)
	{
		if(poly_y[i]<=20)
		{
			return 1;
		}
	}
	for(i=0;i<no_blocks;i++)
	{
		for(j=0; j<n;j++)
		{
			if(poly_x[j] == occupied_blocks[i].x && poly_y[j] == occupied_blocks[i].y)
			{
				collision++;
				if(collision >= 2) return 1;
			}
		}
	}
	return 0;
}


void iDraw() {
	iClear();

	//background images
	iShowBMP(0,0,"tetris.jpg");
	iShowBMP(630,150,"tetris_2.jpg");


	//tetromino preview
		iSetColor(255, 255, 255);
		iText(650, 172, "UP NEXT",GLUT_BITMAP_HELVETICA_18); 
		iRectangle(630,20,200,150);

	//picked tetromino
		for (int i = 0; i < n; i++)
		{
			poly_x[i] = polygon[chosen_poly_no].base_x[i] + shift_x;
			poly_y[i] = polygon[chosen_poly_no].base_y[i] + shift_y;
		}

		iSetColor(polygon[chosen_poly_no].r, polygon[chosen_poly_no].g, polygon[chosen_poly_no].b);
		iFilledPolygon(poly_x, poly_y, n);

	//drawing occupied grid
		iSetColor(77, 19, 0);
		for (int i = 0; i < no_blocks; i++)
		{
			iFilledRectangle(occupied_blocks[i].x, occupied_blocks[i].y, 28, 28);
		}
	
	//big-grid
	//each square is 28 by 28
		iSetColor(255, 255, 255);
		iRectangle(300, 20, 280, 560);
		iSetColor(137,148,153);
		//horizontal lines
		for(int i=0;i<9;i++)
		{
			iLine(328+i*28,20,328+i*28,580);
		}
		//vertical lines
		for(int i=0;i<19;i++)
		{
			iLine(300,48+28*i,580,48+28*i);
		}
}

/*
	function iMouseMove() is called when the user presses and drags the mouse.
	(mx, my) is the position where the mouse pointer is.
	*/
void iMouseMove(int mx, int my) {
	printf("x = %d, y= %d\n",mx,my);
	//place your codes here
}

/*
	function iMouse() is called when the user presses/releases the mouse.
	(mx, my) is the position where the mouse pointer is.
	*/
void iMouse(int button, int state, int mx, int my) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
	}
}

/*
	function iKeyboard() is called whenever the user hits a key in keyboard.
	key- holds the ASCII value of the key pressed.
	*/
void iKeyboard(unsigned char key) {
	int has_collided_grid = collision_grid_x(poly_x,poly_y);
	if (key == 'w')
	{
		
	}
	else if ((key == 'a' || key == 'A') && (!collision_left(poly_x) && !has_collided_grid))
	{
		shift_x = shift_x - 28;
	}
	else if ((key == 's' || key == 'S') && (!has_collided_grid))
	{
		shift_y = shift_y - 28;
	}
	else if ((key == 'd' || key == 'D') && (!collision_right(poly_x) && !has_collided_grid))
	{
		shift_x = shift_x + 28;
	}
	//place your codes for other keys here
}

/*
	function iSpecialKeyboard() is called whenver user hits special keys like-
	function keys, home, end, pg up, pg down, arraows etc. you have to use
	appropriate constants to detect them. A list is:
	GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
	GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11, GLUT_KEY_F12,
	GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN, GLUT_KEY_PAGE UP,
	GLUT_KEY_PAGE DOWN, GLUT_KEY_HOME, GLUT_KEY_END, GLUT_KEY_INSERT
	*/
void iSpecialKeyboard(unsigned char key)
{
	if (key == GLUT_KEY_END) {
		exit(0);
	}
	//place your codes for other keys here
}


int main() {
	//place your own initialization codes here.
	srand(time(NULL));

	int fall_delay = 800;
	iSetTimer( fall_delay, gravity);

	iInitialize(1100, 619, "TETRIS");
	return 0;
}

void gravity()
{
	if(!collision_grid_x(poly_x,poly_y))
		shift_y = shift_y - 28;
	else
	{
		for(int i=0; i<n ; i++)
		{
			occupied_blocks[no_blocks].x = poly_x[i];
			occupied_blocks[no_blocks].y = poly_y[i];
			no_blocks++;
		}
		spawn();
	}
}