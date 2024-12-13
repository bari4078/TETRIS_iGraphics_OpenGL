# include "iGraphics.h"
#include <time.h>
#include <stdlib.h>
#include <windows.h>

#define screen_width 1100
#define screen_height 619

#define little_block 28

#define grid_height 560
#define grid_bottom 20

#define grid_left 300
#define grid_width 280

struct polygons
{
	double base_x[4];
	double base_y[4];
	int r,g,b;
} polygon[7] =
	{
			{	//straight tetromino
				{384, 412, 440, 468}, 
				{552, 552, 552, 552}, 
					135, 206, 235
			},
			{	//square tetromino
				{412,440,440,412}, 
				{524,524,552,552}, 
					255, 179, 67
			}, 
			{	//left L tetromino
				{412,440,440,440}, 
				{496,496,524,552}, 
					65, 105, 225
			},
			{	//right L tetromino
				{412,412,412,440}, 
				{552,524,496,496}, 
					253, 225, 100
			},
			{	//T shaped tetromino
				{384,412,440,412}, 
				{552,552,552,524}, 
					207, 159 , 255
			},
			{	// Z shaped tetromino
				{384,412,412,440}, 
				{552,552,524,524}, 
					238, 109 , 81
			},
			{	// mirror Z shaped tetromino
				{412,440,384,412}, 
				{552,552,524,524}, 
					0, 103 , 79
			},

			
	};

struct button_co_ord
{
	int x;
	int y;
} button_coordinate[2];

int game_state = -1;
//game state -1: main menu
//game state 0: gameplay
//game state 1: pause
//game state 2: game over

int chosen_poly_no = rand() % 7;
int up_next_poly = rand() % 7;
int n = 4;

double poly_x[4];
double poly_y[4];

double shift_x = 0;
double shift_y = 0;


struct block
{
    double x;
    double y;
	int r,g,b;
} occupied_blocks[201];

int no_blocks=0;

void spawn()
{	
	chosen_poly_no = up_next_poly;
	up_next_poly = rand() % 7;
	
	shift_x = 0;
	shift_y = 0;

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
	for(int i=0; i < n ; i++)
	{
		if(poly_x[i]<=300)
		{
			return 1;
		}
	}
	return 0;
}
//right collision check ---parameter: x co-ordinate array of vertices
int collision_right(double poly_x[])
{
	for(int i=0;i<n;i++)
	{
		if(poly_x[i]+ little_block >= (grid_bottom + grid_height))
		{
			return 1;
		}
	}
	return 0;
}

int collision_grid_x(double poly_x[],double poly_y[])
{
	int i,j;
	int collision_up=0,collision_down=0;
	for(i=0;i<n;i++)
	{
		if(poly_y[i]<=20)
		{
			return 1;
		}
	}
	
	for(i=0; i < n ; i++)
	{
		for(j=0; j<= no_blocks; j++)
		{
			if((poly_x[i] == occupied_blocks[j].x) && (abs(poly_y[i] - occupied_blocks[j].y) == little_block))
				return 1;
		}
	}
	return 0;
}


void iDraw() {
	iClear();

	int i;
	if(game_state == -1)
	{
		iShowBMP(45,0,"resourse\\home_menu_1.bmp");
		iShowBMP(button_coordinate[0].x,button_coordinate[0].y,"resourse\\start.bmp");
		iShowBMP(button_coordinate[1].x,button_coordinate[1].y,"resourse\\exit_button.bmp");
	}
	else if(game_state == 2)
	{
		iShowBMP(0,0,"resourse\\game_over.bmp");
		iText(470,85,"Press 'end' to quit",GLUT_BITMAP_9_BY_15);
		iText(420,70,"Press 'M' to return to main menu",GLUT_BITMAP_9_BY_15);
		iText(460,55,"Press 'P' to play again",GLUT_BITMAP_9_BY_15);
	}
	else
	{
	//background images
		iShowBMP(0,0,"resourse\\tetris.bmp");
		iShowBMP(630,150,"resourse\\tetris_2.bmp");
		iSetColor(255, 255, 255);
		iText(900,70,"Press 'end' to quit",GLUT_BITMAP_9_BY_15);

	//tetromino preview
		iSetColor(255, 255, 255);
		iText(650, 175, "UP NEXT",GLUT_BITMAP_HELVETICA_18); 
		iRectangle(630,20,200,150);

		//up next tetromino
		iSetColor(polygon[up_next_poly].r,polygon[up_next_poly].g,polygon[up_next_poly].b);
			for(i=0;i<n;i++)
			{
				iFilledRectangle(polygon[up_next_poly].base_x[i] + 290,polygon[up_next_poly].base_y[i]-450,little_block,little_block);
			}

	//picked tetromino
		for (i = 0; i < n; i++)
		{
			poly_x[i] = polygon[chosen_poly_no].base_x[i] + shift_x;
			poly_y[i] = polygon[chosen_poly_no].base_y[i] + shift_y;
		}

		iSetColor(polygon[chosen_poly_no].r, polygon[chosen_poly_no].g, polygon[chosen_poly_no].b);
		for(i=0; i < n; i++)
		{
			iFilledRectangle(poly_x[i],poly_y[i],little_block,little_block);
		}

	//drawing occupied grid
		for (i = 0; i < no_blocks; i++)
		{
			iSetColor(occupied_blocks[i].r,occupied_blocks[i].g,occupied_blocks[i].b);
			iFilledRectangle(occupied_blocks[i].x, occupied_blocks[i].y, little_block, little_block);
		}
	
	//big-grid
		iSetColor(255, 255, 255);
		iRectangle(grid_left, grid_bottom, grid_width, grid_height);
		iSetColor(137,148,153);
		//horizontal lines
		for(i=0;i<9;i++)
		{
			iLine( (grid_left+little_block) + i*little_block , grid_bottom,
			 		( (grid_left+little_block)) + i*little_block,grid_bottom+grid_height);
		}
		//vertical lines
		for(i=0;i<19;i++)
		{
			iLine(grid_left,48+little_block*i,(grid_bottom+grid_height),48+little_block*i);
		}
	}
}

/*
	function iMouseMove() is called when the user presses and drags the mouse.
	(mx, my) is the position where the mouse pointer is.
	*/
void iMouseMove(int mx, int my) {
	printf("x = %d, y= %d\n",mx,my);
}

/*
	function iMouse() is called when the user presses/releases the mouse.
	(mx, my) is the position where the mouse pointer is.
	*/
void iMouse(int button, int state, int mx, int my) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if((mx >= button_coordinate[0].x && mx <= button_coordinate[0].x + 250) && (my >= button_coordinate[0].y && my <= button_coordinate[0].y + 65))
		{
			game_state = 0;
		}
		else if((mx >= button_coordinate[1].x && mx <= button_coordinate[1].x + 250) && (my >= button_coordinate[1].y && my <= button_coordinate[1].y + 65))
		{
			exit(0);
		}
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
		//will rotate the polygons
	}
	else if ((key == 'a' || key == 'A') && (!collision_left(poly_x) && !has_collided_grid))
	{
		shift_x = shift_x - little_block;
	}
	else if ((key == 's' || key == 'S') && (!has_collided_grid))
	{
		shift_y = shift_y - little_block;
	}
	else if ((key == 'd' || key == 'D') && (!collision_right(poly_x) && !has_collided_grid))
	{
		shift_x = shift_x + little_block;
	}
	
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
	srand(time(NULL));

	//startbutton
		button_coordinate[0].x = 425;
		button_coordinate[0].y = 445;
	//quit button
		button_coordinate[1].x = 425;
		button_coordinate[1].y = 350;

	int fall_delay = 800;
	iSetTimer(fall_delay, gravity);

	iInitialize(screen_width, screen_height, "TETRIS");
	return 0;
}

void gravity()
{
	int i;
	if(!collision_grid_x(poly_x,poly_y))
		shift_y = shift_y - little_block;
	else
	{
		for(i=0; i<n ; i++)
		{
			occupied_blocks[no_blocks].x = poly_x[i];
			occupied_blocks[no_blocks].y = poly_y[i];

			occupied_blocks[no_blocks].r = polygon[chosen_poly_no].r;
			occupied_blocks[no_blocks].g = polygon[chosen_poly_no].g;
			occupied_blocks[no_blocks].b = polygon[chosen_poly_no].b;

			no_blocks++;
		}
		for(i=0;i <= no_blocks;i++)
		{
			if(occupied_blocks[i].y >= 552)
			{
				game_state = 2;
				return;
			}
		}
		spawn();
	}
}