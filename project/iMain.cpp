#include "iGraphics.h"
#include <time.h>
#include <stdlib.h>
#include <windows.h>

#define NULL 0

#define main_menu_sound PlaySound(TEXT("resourse\\Tetris_Main_Menu_Theme.wav"),NULL,SND_ASYNC|SND_LOOP|SND_FILENAME)
#define in_game_sound PlaySound(TEXT("resourse\\Tetris.wav"),NULL,SND_ASYNC|SND_LOOP|SND_FILENAME)
#define game_over_sound PlaySound(TEXT("resourse\\game_over.wav"),NULL,SND_ASYNC|SND_LOOP|SND_FILENAME)


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
} Mstart_button,Mexit_button,Pexit_button,Presume_button,Prestart_button,Minfo_button;

int game_state = -1;
//game state -1: main menu
//game state 0: gameplay
//game state 1: pause
//game state 2: game over
//game state 3: game complete
int about = 0;

int chosen_poly_no = rand() % 7;
int up_next_poly = rand() % 7;
struct polygons players_poly = polygon[chosen_poly_no];
int position = 0;

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
} occupied_blocks[201] = {
	{0,0,0,0,0}
};

int no_blocks=0;

int score = 0;
char score_text[50];
char high_score_text[50];
int high_score = 0;

int fall_delay = 800;


void high_score_update()
{
	FILE *f_read;
	f_read = fopen("data.txt","r");

	fscanf(f_read,"%d",&high_score);
	fclose(f_read);

	if(score > high_score)
	{
		high_score = score;

		FILE *f_write;
		f_write = fopen("data.txt","w");

		fprintf(f_write, "%d", high_score);
		fclose(f_write);
	}
}

void spawn()
{	
	chosen_poly_no = up_next_poly;
	up_next_poly = rand() % 7;
	players_poly = polygon[chosen_poly_no];
	position = 0;

	shift_x = 0;
	shift_y = 0;

	for (int i = 0; i < n; i++)
	{
		poly_x[i] = players_poly.base_x[i];
		poly_y[i] = players_poly.base_y[i];
	}

	return;
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


void rotate()
{
	if(chosen_poly_no == 1) return;
	struct polygons temp_poly = players_poly;
	int i;
	//straight tetromino
	if(chosen_poly_no == 0)
	{	// straight tetromino
		if(position == 0)
		{
			temp_poly.base_x[0] += 56;
			temp_poly.base_y[0] += 56;

			temp_poly.base_x[1] += 28;
			temp_poly.base_y[1] += 28;

			temp_poly.base_x[3] -= 28;
			temp_poly.base_y[3] -= 28;


			double *temp_poly_x,*temp_poly_y;
			temp_poly_x = (double*)malloc(4*sizeof(double));
			temp_poly_y = (double*)malloc(4*sizeof(double));

			for(i=0; i<4; i++)
			{
				temp_poly_x[i] = temp_poly.base_x[i] + shift_x;
				temp_poly_y[i] = temp_poly.base_y[i] + shift_y;
			}

			if(!collision_left(temp_poly_x) && !collision_right(temp_poly_x))
			{
				position = (position+1) % 2;
				players_poly = temp_poly;
				free(temp_poly_x);
				free(temp_poly_y);
				return;
			}
		}
		else if(position == 1)
		{
			temp_poly.base_x[0] -= 56;
			temp_poly.base_y[0] -= 56;

			temp_poly.base_x[1] -= 28;
			temp_poly.base_y[1] -= 28;

			temp_poly.base_x[3] += 28;
			temp_poly.base_y[3] += 28;


			double *temp_poly_x,*temp_poly_y;
			temp_poly_x = (double*)malloc(4*sizeof(double));
			temp_poly_y = (double*)malloc(4*sizeof(double));

			for(i=0; i<4; i++)
			{
				temp_poly_x[i] = temp_poly.base_x[i] + shift_x;
				temp_poly_y[i] = temp_poly.base_y[i] + shift_y;
			}

			if(!collision_left(temp_poly_x) && !collision_right(temp_poly_x))
			{
				position = (position+1) % 2;
				players_poly = temp_poly;
				free(temp_poly_x);
				free(temp_poly_y);
				return;
			}
		}

	}
	
	//left L tetromino
	else if(chosen_poly_no == 2)
	{	
		if(position == 0)
		{	
			temp_poly.base_x[0] = 440;
			temp_poly.base_x[1] = 440;
			temp_poly.base_x[2] = 412;
			temp_poly.base_x[3] = 384;

			temp_poly.base_y[0] = 524;
			temp_poly.base_y[1] = 552;
			temp_poly.base_y[2] = 552;
			temp_poly.base_y[3] = 552;

			double *temp_poly_x,*temp_poly_y;
			temp_poly_x = (double*)malloc(4*sizeof(double));
			temp_poly_y = (double*)malloc(4*sizeof(double));

			for(i=0; i<4; i++)
			{
				temp_poly_x[i] = temp_poly.base_x[i] + shift_x;
				temp_poly_y[i] = temp_poly.base_y[i] + shift_y;
			}

			if(!collision_left(temp_poly_x) && !collision_right(temp_poly_x))
			{
				position = (position+1) % 4;
				players_poly = temp_poly;
				free(temp_poly_x);
				free(temp_poly_y);
				return;
			}
		}
		else if(position == 1)
		{	
			temp_poly.base_x[0] = 440;
			temp_poly.base_x[1] = 412;
			temp_poly.base_x[2] = 412;
			temp_poly.base_x[3] = 412;

			temp_poly.base_y[0] = 552;
			temp_poly.base_y[1] = 552;
			temp_poly.base_y[2] = 524;
			temp_poly.base_y[3] = 496;
			
			double *temp_poly_x,*temp_poly_y;
			temp_poly_x = (double*)malloc(4*sizeof(double));
			temp_poly_y = (double*)malloc(4*sizeof(double));

			for(i=0; i<4; i++)
			{
				temp_poly_x[i] = temp_poly.base_x[i] + shift_x;
				temp_poly_y[i] = temp_poly.base_y[i] + shift_y;
			}

			if(!collision_left(temp_poly_x) && !collision_right(temp_poly_x))
			{
				position = (position+1) % 4;
				players_poly = temp_poly;
				free(temp_poly_x);
				free(temp_poly_y);
				return;
			}
		}
		else if(position == 2)
		{	
			temp_poly.base_x[0] = 384;
			temp_poly.base_x[1] = 384;
			temp_poly.base_x[2] = 412;
			temp_poly.base_x[3] = 440;

			temp_poly.base_y[0] = 552;
			temp_poly.base_y[1] = 524;
			temp_poly.base_y[2] = 524;
			temp_poly.base_y[3] = 524;
			
			double *temp_poly_x,*temp_poly_y;
			temp_poly_x = (double*)malloc(4*sizeof(double));
			temp_poly_y = (double*)malloc(4*sizeof(double));

			for(i=0; i<4; i++)
			{
				temp_poly_x[i] = temp_poly.base_x[i] + shift_x;
				temp_poly_y[i] = temp_poly.base_y[i] + shift_y;
			}

			if(!collision_left(temp_poly_x) && !collision_right(temp_poly_x))
			{
				position = (position+1) % 4;
				players_poly = temp_poly;
				free(temp_poly_x);
				free(temp_poly_y);
				return;
			}
		}
		else if(position == 3)
		{	
			temp_poly.base_x[0] = 412;
			temp_poly.base_x[1] = 440;
			temp_poly.base_x[2] = 440;
			temp_poly.base_x[3] = 440;

			temp_poly.base_y[0] = 496;
			temp_poly.base_y[1] = 496;
			temp_poly.base_y[2] = 524;
			temp_poly.base_y[3] = 552;
			
			double *temp_poly_x,*temp_poly_y;
			temp_poly_x = (double*)malloc(4*sizeof(double));
			temp_poly_y = (double*)malloc(4*sizeof(double));

			for(i=0; i<4; i++)
			{
				temp_poly_x[i] = temp_poly.base_x[i] + shift_x;
				temp_poly_y[i] = temp_poly.base_y[i] + shift_y;
			}

			if(!collision_left(temp_poly_x) && !collision_right(temp_poly_x))
			{
				position = (position+1) % 4;
				players_poly = temp_poly;
				free(temp_poly_x);
				free(temp_poly_y);
				return;
			}
		}
	}
	

	//right L tetromino
	else if(chosen_poly_no == 3)
	{	
		if(position == 0)
		{	
			temp_poly.base_x[0] = 440;
			temp_poly.base_x[1] = 440;
			temp_poly.base_x[2] = 412;
			temp_poly.base_x[3] = 384;

			temp_poly.base_y[0] = 552;
			temp_poly.base_y[1] = 524;
			temp_poly.base_y[2] = 524;
			temp_poly.base_y[3] = 524;

			double *temp_poly_x,*temp_poly_y;
			temp_poly_x = (double*)malloc(4*sizeof(double));
			temp_poly_y = (double*)malloc(4*sizeof(double));

			for(i=0; i<4; i++)
			{
				temp_poly_x[i] = temp_poly.base_x[i] + shift_x;
				temp_poly_y[i] = temp_poly.base_y[i] + shift_y;
			}

			if(!collision_left(temp_poly_x) && !collision_right(temp_poly_x))
			{
				position = (position+1) % 4;
				players_poly = temp_poly;
				free(temp_poly_x);
				free(temp_poly_y);
				return;
			}
		}
		else if(position == 1)
		{	
			temp_poly.base_x[0] = 384;
			temp_poly.base_x[1] = 412;
			temp_poly.base_x[2] = 412;
			temp_poly.base_x[3] = 412;

			temp_poly.base_y[0] = 552;
			temp_poly.base_y[1] = 552;
			temp_poly.base_y[2] = 524;
			temp_poly.base_y[3] = 496;
			
			double *temp_poly_x,*temp_poly_y;
			temp_poly_x = (double*)malloc(4*sizeof(double));
			temp_poly_y = (double*)malloc(4*sizeof(double));

			for(i=0; i<4; i++)
			{
				temp_poly_x[i] = temp_poly.base_x[i] + shift_x;
				temp_poly_y[i] = temp_poly.base_y[i] + shift_y;
			}

			if(!collision_left(temp_poly_x) && !collision_right(temp_poly_x))
			{
				position = (position+1) % 4;
				players_poly = temp_poly;
				free(temp_poly_x);
				free(temp_poly_y);
				return;
			}
		}
		else if(position == 2)
		{	
			temp_poly.base_x[0] = 384;
			temp_poly.base_x[1] = 384;
			temp_poly.base_x[2] = 412;
			temp_poly.base_x[3] = 440;

			temp_poly.base_y[0] = 524;
			temp_poly.base_y[1] = 552;
			temp_poly.base_y[2] = 552;
			temp_poly.base_y[3] = 552;
			
			double *temp_poly_x,*temp_poly_y;
			temp_poly_x = (double*)malloc(4*sizeof(double));
			temp_poly_y = (double*)malloc(4*sizeof(double));

			for(i=0; i<4; i++)
			{
				temp_poly_x[i] = temp_poly.base_x[i] + shift_x;
				temp_poly_y[i] = temp_poly.base_y[i] + shift_y;
			}

			if(!collision_left(temp_poly_x) && !collision_right(temp_poly_x))
			{
				position = (position+1) % 4;
				players_poly = temp_poly;
				free(temp_poly_x);
				free(temp_poly_y);
				return;
			}
		}
		else if(position == 3)
		{	
			temp_poly.base_x[0] = 412;
			temp_poly.base_x[1] = 412;
			temp_poly.base_x[2] = 412;
			temp_poly.base_x[3] = 440;

			temp_poly.base_y[0] = 552;
			temp_poly.base_y[1] = 524;
			temp_poly.base_y[2] = 496;
			temp_poly.base_y[3] = 496;
			
			double *temp_poly_x,*temp_poly_y;
			temp_poly_x = (double*)malloc(4*sizeof(double));
			temp_poly_y = (double*)malloc(4*sizeof(double));

			for(i=0; i<4; i++)
			{
				temp_poly_x[i] = temp_poly.base_x[i] + shift_x;
				temp_poly_y[i] = temp_poly.base_y[i] + shift_y;
			}

			if(!collision_left(temp_poly_x) && !collision_right(temp_poly_x))
			{
				position = (position+1) % 4;
				players_poly = temp_poly;
				free(temp_poly_x);
				free(temp_poly_y);
				return;
			}
		}
	}
	
	//T shaped tetronimo
	else if(chosen_poly_no == 4)
	{
		if(position == 0)
		{	
			temp_poly.base_x[0] = 412;
			temp_poly.base_x[1] = 412;
			temp_poly.base_x[2] = 412;
			temp_poly.base_x[3] = 440;

			temp_poly.base_y[0] = 552;
			temp_poly.base_y[1] = 524;
			temp_poly.base_y[2] = 496;
			temp_poly.base_y[3] = 524;

			double *temp_poly_x,*temp_poly_y;
			temp_poly_x = (double*)malloc(4*sizeof(double));
			temp_poly_y = (double*)malloc(4*sizeof(double));

			for(i=0; i<4; i++)
			{
				temp_poly_x[i] = temp_poly.base_x[i] + shift_x;
				temp_poly_y[i] = temp_poly.base_y[i] + shift_y;
			}

			if(!collision_left(temp_poly_x) && !collision_right(temp_poly_x))
			{
				position = (position+1) % 4;
				players_poly = temp_poly;
				free(temp_poly_x);
				free(temp_poly_y);
				return;
			}
		}
		else if(position == 1)
		{	
			temp_poly.base_x[0] = 384;
			temp_poly.base_x[1] = 412;
			temp_poly.base_x[2] = 440;
			temp_poly.base_x[3] = 412;

			temp_poly.base_y[0] = 524;
			temp_poly.base_y[1] = 524;
			temp_poly.base_y[2] = 524;
			temp_poly.base_y[3] = 552;
			
			double *temp_poly_x,*temp_poly_y;
			temp_poly_x = (double*)malloc(4*sizeof(double));
			temp_poly_y = (double*)malloc(4*sizeof(double));

			for(i=0; i<4; i++)
			{
				temp_poly_x[i] = temp_poly.base_x[i] + shift_x;
				temp_poly_y[i] = temp_poly.base_y[i] + shift_y;
			}

			if(!collision_left(temp_poly_x) && !collision_right(temp_poly_x))
			{
				position = (position+1) % 4;
				players_poly = temp_poly;
				free(temp_poly_x);
				free(temp_poly_y);
				return;
			}
		}
		else if(position == 2)
		{	
			temp_poly.base_x[0] = 412;
			temp_poly.base_x[1] = 412;
			temp_poly.base_x[2] = 412;
			temp_poly.base_x[3] = 384;

			temp_poly.base_y[0] = 552;
			temp_poly.base_y[1] = 524;
			temp_poly.base_y[2] = 496;
			temp_poly.base_y[3] = 524;
			
			double *temp_poly_x,*temp_poly_y;
			temp_poly_x = (double*)malloc(4*sizeof(double));
			temp_poly_y = (double*)malloc(4*sizeof(double));

			for(i=0; i<4; i++)
			{
				temp_poly_x[i] = temp_poly.base_x[i] + shift_x;
				temp_poly_y[i] = temp_poly.base_y[i] + shift_y;
			}

			if(!collision_left(temp_poly_x) && !collision_right(temp_poly_x))
			{
				position = (position+1) % 4;
				players_poly = temp_poly;
				free(temp_poly_x);
				free(temp_poly_y);
				return;
			}
		}
		else if(position == 3)
		{	
			temp_poly.base_x[0] = 384;
			temp_poly.base_x[1] = 412;
			temp_poly.base_x[2] = 440;
			temp_poly.base_x[3] = 412;

			temp_poly.base_y[0] = 552;
			temp_poly.base_y[1] = 552;
			temp_poly.base_y[2] = 552;
			temp_poly.base_y[3] = 524;
			
			double *temp_poly_x,*temp_poly_y;
			temp_poly_x = (double*)malloc(4*sizeof(double));
			temp_poly_y = (double*)malloc(4*sizeof(double));

			for(i=0; i<4; i++)
			{
				temp_poly_x[i] = temp_poly.base_x[i] + shift_x;
				temp_poly_y[i] = temp_poly.base_y[i] + shift_y;
			}

			if(!collision_left(temp_poly_x) && !collision_right(temp_poly_x))
			{
				position = (position+1) % 4;
				players_poly = temp_poly;
				free(temp_poly_x);
				free(temp_poly_y);
				return;
			}
		}
	}

	//Z shaped tetromino
	else if(chosen_poly_no == 5)
	{	
		if(position == 0)
		{	
			temp_poly.base_x[0] = 440;
			temp_poly.base_x[1] = 440;
			temp_poly.base_x[2] = 412;
			temp_poly.base_x[3] = 412;

			temp_poly.base_y[0] = 552;
			temp_poly.base_y[1] = 524;
			temp_poly.base_y[2] = 524;
			temp_poly.base_y[3] = 496;

			double *temp_poly_x,*temp_poly_y;
			temp_poly_x = (double*)malloc(4*sizeof(double));
			temp_poly_y = (double*)malloc(4*sizeof(double));

			for(i=0; i<4; i++)
			{
				temp_poly_x[i] = temp_poly.base_x[i] + shift_x;
				temp_poly_y[i] = temp_poly.base_y[i] + shift_y;
			}

			if(!collision_left(temp_poly_x) && !collision_right(temp_poly_x))
			{
				position = (position+1) % 2;
				players_poly = temp_poly;
				free(temp_poly_x);
				free(temp_poly_y);
				return;
			}
		}
		else if(position == 1)
		{	
			temp_poly.base_x[0] = 384;
			temp_poly.base_x[1] = 412;
			temp_poly.base_x[2] = 412;
			temp_poly.base_x[3] = 440;

			temp_poly.base_y[0] = 552;
			temp_poly.base_y[1] = 552;
			temp_poly.base_y[2] = 524;
			temp_poly.base_y[3] = 524;
			
			double *temp_poly_x,*temp_poly_y;
			temp_poly_x = (double*)malloc(4*sizeof(double));
			temp_poly_y = (double*)malloc(4*sizeof(double));

			for(i=0; i<4; i++)
			{
				temp_poly_x[i] = temp_poly.base_x[i] + shift_x;
				temp_poly_y[i] = temp_poly.base_y[i] + shift_y;
			}

			if(!collision_left(temp_poly_x) && !collision_right(temp_poly_x))
			{
				position = (position+1) % 2;
				players_poly = temp_poly;
				free(temp_poly_x);
				free(temp_poly_y);
				return;
			}
		}
	}

	//mirror Z shaped tetromino
	else if(chosen_poly_no == 6)
	{	
		if(position == 0)
		{	
			temp_poly.base_x[0] = 412;
			temp_poly.base_x[1] = 412;
			temp_poly.base_x[2] = 440;
			temp_poly.base_x[3] = 440;

			temp_poly.base_y[0] = 552;
			temp_poly.base_y[1] = 524;
			temp_poly.base_y[2] = 524;
			temp_poly.base_y[3] = 496;

			double *temp_poly_x,*temp_poly_y;
			temp_poly_x = (double*)malloc(4*sizeof(double));
			temp_poly_y = (double*)malloc(4*sizeof(double));

			for(i=0; i<4; i++)
			{
				temp_poly_x[i] = temp_poly.base_x[i] + shift_x;
				temp_poly_y[i] = temp_poly.base_y[i] + shift_y;
			}

			if(!collision_left(temp_poly_x) && !collision_right(temp_poly_x))
			{
				position = (position+1) % 2;
				players_poly = temp_poly;
				free(temp_poly_x);
				free(temp_poly_y);
				return;
			}
		}
		else if(position == 1)
		{	
			temp_poly.base_x[0] = 412;
			temp_poly.base_x[1] = 440;
			temp_poly.base_x[2] = 384;
			temp_poly.base_x[3] = 412;

			temp_poly.base_y[0] = 552;
			temp_poly.base_y[1] = 552;
			temp_poly.base_y[2] = 524;
			temp_poly.base_y[3] = 524;
			
			double *temp_poly_x,*temp_poly_y;
			temp_poly_x = (double*)malloc(4*sizeof(double));
			temp_poly_y = (double*)malloc(4*sizeof(double));

			for(i=0; i<4; i++)
			{
				temp_poly_x[i] = temp_poly.base_x[i] + shift_x;
				temp_poly_y[i] = temp_poly.base_y[i] + shift_y;
			}

			if(!collision_left(temp_poly_x) && !collision_right(temp_poly_x))
			{
				position = (position+1) % 2;
				players_poly = temp_poly;
				free(temp_poly_x);
				free(temp_poly_y);
				return;
			}
		}
	}

}


void update_score()
{
	score = score + 150;
	if(score >= 2300)
	{
		fall_delay = 270;
	}
	else if(score >= 2000)
	{
		fall_delay = 300;
	}	
	else if(score >= 1800)
	{
		fall_delay = 350;
	}	
	else if(score >= 1500)
	{
		fall_delay = 400;
	}	
	else if(score >= 1200)
	{
		fall_delay = 500;
	}	
	else if(score >= 1000)
	{
		fall_delay = 600;
	}	
	else if(score >= 500)
	{
		fall_delay = 700;
	}
}

void clear_row() 
{
    int row, col, i, j;
    int full_row;

    for(row = grid_bottom; row < (grid_bottom + grid_height); row = row+ little_block)
	{
        full_row = 1;

        for(col = grid_left; col < (grid_left + grid_width); col = col + little_block)
		{
            int found = 0;
            for(i = 0; i <= no_blocks; i++)
			{
                if((occupied_blocks[i].x == col) && (occupied_blocks[i].y == row)) 
				{
                    found = 1;
                    break;
                }
            }
            if(!found) 
			{
                full_row = 0; 
                break;
            }
        }

        if(full_row) 
		{
			score = score + 150;
			
            for(i = 0; i <= no_blocks; i++)
			{
                if(occupied_blocks[i].y == row) 
				{
                    for(j = i; j <= (no_blocks - 1); j++) 
					{
                        occupied_blocks[j] = occupied_blocks[j + 1];
                    }
                    no_blocks--;
                    i--; 
                }
            }

            for(i = 0; i <= no_blocks; i++) 
			{
                if(occupied_blocks[i].y > row) 
				{
                    occupied_blocks[i].y -= little_block;
                }
            }
        }
    }
}



void reset()
{
	position = 0;

	int i;
	shift_x = 0;
	shift_y = 0;
	score = 0;

	for(i=0;i < no_blocks;i++)
	{
		occupied_blocks[i].x = NULL;
		occupied_blocks[i].y = NULL;
		occupied_blocks[i].r = NULL;
		occupied_blocks[i].g = NULL;
		occupied_blocks[i].b = NULL;
	}
	no_blocks = 0;
}

void iDraw() {
	iClear();

	int i;

	//Main Menu
	if(game_state == -1)
	{	//Main Menu
		iShowBMP(45,0,"resourse\\home_menu_1.bmp");
		iShowBMP(Minfo_button.x,Minfo_button.y,"resourse\\info_button.bmp");
		if(about == 0)
		{
			iShowBMP(Mstart_button.x,Mstart_button.y,"resourse\\start.bmp");
			iShowBMP(Mexit_button.x,Mexit_button.y,"resourse\\exit.bmp");
		}
		else
		{
			iShowBMP(200,200,"resourse\\about.bmp");
		}
	}

	//Pause Menu
	else if(game_state == 1)
	{	//Pause Menu
		iShowBMP(0,0,"resourse\\tetris.bmp");
		iShowBMP(630,150,"resourse\\tetris_2.bmp");
		iShowBMP(Presume_button.x,Presume_button.y,"resourse\\resume.bmp");
		iShowBMP(Pexit_button.x,Pexit_button.y,"resourse\\exit.bmp");
		iShowBMP(Prestart_button.x,Prestart_button.y,"resourse\\restart.bmp");
	}

	//Game Over
	else if(game_state == 2)
	{	//Game Over
		iShowBMP(0,0,"resourse\\game_over.bmp");
		iSetColor(255,255,0);
		iText(470,85,"Press 'end' to quit",GLUT_BITMAP_9_BY_15);

		iText(420,70,"Press 'M' to return to main menu",GLUT_BITMAP_9_BY_15);

		iText(460,55,"Press 'P' to play again",GLUT_BITMAP_9_BY_15);

		//score
		sprintf(score_text, "SCORE: %d", score);
		iText(535, 519, score_text,GLUT_BITMAP_HELVETICA_18); 

		//high score
		sprintf(high_score_text,"HIGH SCORE: %d",high_score);
		iText(506,490,high_score_text,GLUT_BITMAP_HELVETICA_18);

	}
	
	//Game play
	else
	{
	//background images

		iShowBMP(0,0,"resourse\\tetris.bmp");
		iShowBMP(630,150,"resourse\\tetris_2.bmp");
		iSetColor(255, 255, 255);
		iText(900,70,"Press 'end' to QUIT",GLUT_BITMAP_9_BY_15);
		iText(900,83,"Press 'P' to PAUSE",GLUT_BITMAP_9_BY_15);

	//score
		iSetColor(255,255,0);
		sprintf(score_text, "SCORE: %d", score);
		iText(650, 330, score_text,GLUT_BITMAP_HELVETICA_18); 
		iSetColor(255,255,255);

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
			poly_x[i] = players_poly.base_x[i] + shift_x;
			poly_y[i] = players_poly.base_y[i] + shift_y;
		}

		iSetColor(players_poly.r, players_poly.g, players_poly.b);
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
		if(game_state == -1)
		{
			if((mx >= Minfo_button.x && mx <= Minfo_button.x + 56) && (my >= Minfo_button.y && my <= Minfo_button.y + 56))
			{
				if(!about) about = 1;
				else about = 0;
			}
			if((mx >= Mstart_button.x && mx <= Mstart_button.x + 250) && (my >= Mstart_button.y && my <= Mstart_button.y + 65) && !about)
			{
				game_state = 0;
				in_game_sound;
			}
			else if((mx >= Mexit_button.x && mx <= Mexit_button.x + 250) && (my >= Mexit_button.y && my <= Mexit_button.y + 65) && !about)
			{
				exit(0);
			}
		}
		else if(game_state == 1)
		{
			if((mx >= Presume_button.x && mx <= Presume_button.x + 250) && (my >= Presume_button.y && my <= Presume_button.y + 65))
			{
				game_state = 0;
				in_game_sound;
			}
			else if((mx >= Pexit_button.x && mx <= Pexit_button.x + 250) && (my >= Pexit_button.y && my <= Pexit_button.y + 65))
			{
				exit(0);
			}
			else if((mx >= Prestart_button.x && mx <= Prestart_button.x + 250) && (my >= Prestart_button.y && my <= Prestart_button.y + 65))
			{
				game_state = 0;	
				in_game_sound;
				reset();
			}
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
	if(key == 'p')
	{
		//pause & was playing
		if(game_state == 0)
		{
			game_state = 1;
			main_menu_sound;
		}
		//play again & game over
		else if(game_state == 2)
		{
			game_state = 0;
			in_game_sound;
			reset();
		}
	}
	//return to main menu
	else if(key == 'm' && game_state == 2)
	{
		game_state = -1;
		main_menu_sound;
		reset();
	}
	//will rotate the polygons
	else if (key == 'w')
	{
		rotate();
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
//Main menu
	//startbutton
		Mstart_button.x = 425;
		Mstart_button.y = 445;
	//quit button
		Mexit_button.x = 425;
		Mexit_button.y = 350;
	//about button
		Minfo_button.x = 40;
		Minfo_button.y = 75;

//Pause menu
	//resume button
		Presume_button.x = 425;
		Presume_button.y = 415;
	//restart button
		Prestart_button.x = 425;
		Prestart_button.y = 320;
	//exit button
		Pexit_button.x = 425;
		Pexit_button.y = 225;
			
	iSetTimer(fall_delay, gravity);

	main_menu_sound;

	iInitialize(screen_width, screen_height, "TETRIS");
	return 0;
}

void gravity()
{
	if(game_state != 0) return;
	int i;
	if(!collision_grid_x(poly_x,poly_y))
		shift_y = shift_y - little_block;
	else
	{
		for(i=0; i<n ; i++)
		{
			occupied_blocks[no_blocks].x = poly_x[i];
			occupied_blocks[no_blocks].y = poly_y[i];

			occupied_blocks[no_blocks].r = players_poly.r;
			occupied_blocks[no_blocks].g = players_poly.g;
			occupied_blocks[no_blocks].b = players_poly.b;

			no_blocks++;
		}
		for(i=0;i <= no_blocks;i++)
		{
			if(occupied_blocks[i].y >= 552)
			{
				game_state = 2;
				high_score_update();
				game_over_sound;
				return;
			}
		}
		clear_row();
		spawn();
	}
}