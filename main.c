#include <LPC17xx.h>
#include "glcd.h"
#include "KBD.h" 
#include <RTL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "LED.h"


#define WH	White
#define GR	Green
#define RE  Red
#define DG 	0x0300                                 

#define SuperSpeed 0

OS_MUT mut_snake;
OS_MUT mut_heading;

volatile uint32_t heading = 105;
volatile uint32_t pushbutton = 1;
volatile uint32_t start_screen = 1;
volatile uint32_t game_over = 0;

__task void base_task( void );

unsigned short snake_bitmap[] = {GR, GR, GR, GR, GR, GR, GR, GR, GR, GR,
								 GR, GR, GR, GR, GR, GR, GR, GR, GR, GR,
								 GR, GR, GR, GR, GR, GR, GR, GR, GR, GR,
								 GR, GR, GR, GR, GR, GR, GR, GR, GR, GR,
								 GR, GR, GR, GR, GR, GR, GR, GR, GR, GR,
								 GR, GR, GR, GR, GR, GR, GR, GR, GR, GR,
								 GR, GR, GR, GR, GR, GR, GR, GR, GR, GR,
								 GR, GR, GR, GR, GR, GR, GR, GR, GR, GR,
								 GR, GR, GR, GR, GR, GR, GR, GR, GR, GR,
								 GR, GR, GR, GR, GR, GR, GR, GR, GR, GR};


unsigned short background_bitmap[] = {WH, WH, WH, WH, WH, WH, WH, WH, WH, WH,
									  WH, WH, WH, WH, WH, WH, WH, WH, WH, WH,
									  WH, WH, WH, WH, WH, WH, WH, WH, WH, WH,
									  WH, WH, WH, WH, WH, WH, WH, WH, WH, WH,
									  WH, WH, WH, WH, WH, WH, WH, WH, WH, WH,
									  WH, WH, WH, WH, WH, WH, WH, WH, WH, WH,
									  WH, WH, WH, WH, WH, WH, WH, WH, WH, WH,
									  WH, WH, WH, WH, WH, WH, WH, WH, WH, WH,
									  WH, WH, WH, WH, WH, WH, WH, WH, WH, WH,
									  WH, WH, WH, WH, WH, WH, WH, WH, WH, WH};


typedef struct{
	int xpos;
	int ypos;
	unsigned short*bitmap;
} food_t;


typedef struct node{
	int xpos;
	int ypos;
	struct node * next;
	struct node * prev; 
} node_t;


typedef struct{
	int size; 
	uint32_t curr_heading;
	node_t* head; 
	node_t* tail;
	unsigned short*bitmap; //head of the snake
} snake_ll_t;

//initialize linked list
volatile snake_ll_t snake;
//initialize food
volatile food_t food;

void move(volatile snake_ll_t* snake, uint32_t heading){
	node_t* node = snake->tail;
	while((node != NULL) && (node->prev != NULL)) {
		node->xpos = node->prev->xpos;
		node->ypos = node->prev->ypos;
		node = node->prev; 
	}
			/* NOTE VALUES FOR KBD:
				REST: 121
				LEFT: 113
				RIGHT: 89
				UP: 105
				DOWN: 57
				PRESSED: 120
		*/
	if((heading != 121) && (heading != 120)) {
		//check to see if heading change is legal 
		if ((heading == 113 && snake->curr_heading != 89) || (heading == 89 && snake->curr_heading != 113) || (heading == 105 && snake->curr_heading != 57) || (heading == 57 && snake->curr_heading != 105))
			snake->curr_heading = heading;
	}
			
	switch(snake->curr_heading) {
			case 113: //left 
      node->xpos -= 10;
      break;
	
   case 89  : //right
      node->xpos += 10;
      break;
	 
   case 105: //up 
			node->ypos -= 10;
      break;
   
	 case 57: //down 
      node->ypos += 10;
      break;
	}
	
}


void display(volatile snake_ll_t* snake, volatile food_t* food, unsigned short bitmap[]){
	node_t* node = snake->head;
	
	while(node != NULL)
	{
		if (node ==                                                                                                                                                                                         snake->head) GLCD_Bitmap(node->xpos, node->ypos,10,10, (unsigned char*)(snake->bitmap));
		else GLCD_Bitmap(node->xpos, node->ypos,10,10, (unsigned char*)(bitmap));
		node = node->next;
	}
		GLCD_Bitmap(food->xpos, food->ypos,10,10, (unsigned char*)(food->bitmap));
}


void clear_tail(volatile snake_ll_t* snake, unsigned short bitmap[]){
	if(snake->tail != NULL)
	{
		GLCD_Bitmap(snake->tail->xpos, snake->tail->ypos,10,10, (unsigned char*)bitmap);
	}
}


int is_out(node_t* head) { //game over when snake goes off edge of screen
		if((head->xpos < 0) || (head->xpos > 230) || (head->ypos < 0) || (head->ypos > 310)) 
			return 1;
		else return 0;
}	


int is_collision(volatile snake_ll_t* snake) {
	node_t* temp;
	if (snake->head != NULL)
		temp = snake->head;
	else
		return 0;//error
	
	while(temp->next != NULL) {
		//check collision between head and temp->next
		if((snake->head->xpos == temp->next->xpos) && (snake->head->ypos == temp->next->ypos) ) {
			return 1; //game over
		}                                                                                                                                                         
		//iterate temp to temp->next
		temp = temp->next;
	}
	return 0;
}

void clear_spot(int x, int y) {
			unsigned short background_bitmap[] = {WH, WH, WH, WH, WH, WH, WH, WH, WH, WH,
																						WH, WH, WH, WH, WH, WH, WH, WH, WH, WH,
																						WH, WH, WH, WH, WH, WH, WH, WH, WH, WH,
																						WH, WH, WH, WH, WH, WH, WH, WH, WH, WH,
																						WH, WH, WH, WH, WH, WH, WH, WH, WH, WH,
																						WH, WH, WH, WH, WH, WH, WH, WH, WH, WH,
																						WH, WH, WH, WH, WH, WH, WH, WH, WH, WH,
																						WH, WH, WH, WH, WH, WH, WH, WH, WH, WH,
																						WH, WH, WH, WH, WH, WH, WH, WH, WH, WH,
																						WH, WH, WH, WH, WH, WH, WH, WH, WH, WH};
			GLCD_Bitmap(x, y,10,10, (unsigned char*)(background_bitmap));
}

void spawn_food(volatile food_t* food) {
	clear_spot(food->xpos, food->ypos);
	food->xpos = rand() % 230;
	food->ypos = rand() % 310; 
}
	

void grow_snake(volatile snake_ll_t* snake) {
	node_t* new_body = (node_t*)malloc(sizeof(node_t)); 
	new_body->prev = snake->tail;
	new_body->next = NULL;
	new_body->xpos = snake->tail->xpos;
	new_body->ypos = snake->tail->ypos;
	
	move(snake, snake->curr_heading);
	
	snake->tail->next = new_body;
	snake->tail = new_body;
	snake->size++;
}

void cut_snake(volatile snake_ll_t* snake) {
	node_t* temp = snake->tail;
	if(!(snake->head == snake->tail))
	{
		while(temp->prev != NULL)
		{
			clear_tail(snake, background_bitmap);
			os_dly_wait (2000);
			temp->prev->next = NULL;
			snake->tail = temp->prev;
			free(temp);
			snake->size--;
			temp = snake->tail;
		}
	}
}


void eat(volatile snake_ll_t* snake, volatile food_t* food) {
	if((fabs(snake->head->xpos - food->xpos) < 10) && (fabs(snake->head->ypos - food->ypos) < 10)) {
		grow_snake(snake);
		spawn_food(food);
	}
}

__task void get_inputs() {
	uint32_t temp;
	while(start_screen == 1) {
		pushbutton = INT0_Get();
	}
	while(game_over == 0){
		os_mut_wait (&mut_heading, 0xffff);
		temp = KBD_Get();
		if(temp != 121 && temp != 120)
			heading = temp;
		os_mut_release (&mut_heading);
		os_dly_wait (300);
	}
	os_tsk_delete_self ();
}

__task void manager() {
	while(start_screen == 1) {
		if(pushbutton == 0)
		{
			start_screen = 0;
		}
	}
	os_dly_wait (10000);
	while(game_over == 0) {
		//wait for semaphore
		os_mut_wait (&mut_snake, 0xffff);
		{
			os_mut_wait(&mut_heading, 0xffff);
			{
				//move snake
				move(&snake, heading);
			}
			os_mut_release (&mut_heading);
		
			//check collisions
			if(is_out(snake.head) == 1 || is_collision(&snake) == 1)
			{
				game_over = 1;
				os_mut_release (&mut_snake);
				break;
			}
			eat(&snake, &food);
		}
		LED_Out((unsigned int)(snake.size));
		os_mut_release (&mut_snake);

		#if SuperSpeed
		if ((5000 - 100*(snake.size)) > 2000)
			os_dly_wait (3000 - 100*(snake.size));
		else
			os_dly_wait (2000);
		#else
		os_dly_wait (5000);
		#endif
	}
	
	//reset values
	os_dly_wait(1000);
	os_mut_wait (&mut_snake, 0xffff);
	cut_snake(&snake);
	snake.head->xpos = 120;
	snake.head->ypos = 140;
	snake.curr_heading = 105;
	os_mut_release (&mut_snake);
	os_tsk_create (base_task, 1);
	os_mut_wait(&mut_heading, 0xffff);
	heading = 105;
	os_mut_release (&mut_heading);
	pushbutton = 1;
	start_screen = 1;
	game_over = 0;
	os_tsk_delete_self ();
}

__task void display_task() {
	GLCD_DisplayString(3, 0, 1, (unsigned char *)("   Game Over"));
	GLCD_SetTextColor(Black);
	GLCD_DisplayString(5, 0, 1, (unsigned char *)("  Push Button"));
	GLCD_DisplayString(6, 0, 1, (unsigned char *)("   To Start!"));
	while(start_screen == 1);
	GLCD_ClearLn(3,1);
	GLCD_ClearLn(5,1);
	GLCD_ClearLn(6,1);
	while(game_over == 0)
	{
		os_mut_wait (&mut_snake, 0xffff);
		{
			display(&snake, &food, snake_bitmap);
		#if SuperSpeed
		if ((3000 - 100*(snake.size)) > 200)
			os_dly_wait (3000 - 100*(snake.size));
		else
			os_dly_wait (300);
		#else
		os_dly_wait (3000);	
		#endif
			
			clear_tail(&snake, background_bitmap);
		}
		os_mut_release (&mut_snake);
	}
	os_tsk_delete_self ();
}

__task void base_task( void ) {
	os_tsk_prio_self( 3 );
	
	os_mut_init (&mut_snake);
	os_mut_init (&mut_heading);
	
	GLCD_Clear(White);
	
	os_tsk_create (get_inputs, 2);
	os_tsk_create (manager, 2);
	os_tsk_create (display_task, 2);

	os_tsk_delete_self ();
}


int main( void ) {
		node_t head;

		unsigned short food_map[] = {RE, RE, RE, RE, RE, RE, RE, RE, RE, RE,
		RE, RE, RE, RE, RE, RE, RE, RE, RE, RE,
		RE, RE, RE, RE, RE, RE, RE, RE, RE, RE,
		RE, RE, RE, RE, RE, RE, RE, RE, RE, RE,
		RE, RE, RE, RE, RE, RE, RE, RE, RE, RE,
		RE, RE, RE, RE, RE, RE, RE, RE, RE, RE,
		RE, RE, RE, RE, RE, RE, RE, RE, RE, RE,
		RE, RE, RE, RE, RE, RE, RE, RE, RE, RE,
		RE, RE, RE, RE, RE, RE, RE, RE, RE, RE,
		RE, RE, RE, RE, RE, RE, RE, RE, RE, RE};

		unsigned short snake_head_map[] = {DG, DG, DG, DG, DG, DG, DG, DG, DG, DG,
		DG, DG, DG, DG, DG, DG, DG, DG, DG, DG,
		DG, DG, DG, DG, DG, DG, DG, DG, DG, DG,
		DG, DG, DG, DG, DG, DG, DG, DG, DG, DG,
		DG, DG, DG, DG, DG, DG, DG, DG, DG, DG,
		DG, DG, DG, DG, DG, DG, DG, DG, DG, DG,
		DG, DG, DG, DG, DG, DG, DG, DG, DG, DG,
		DG, DG, DG, DG, DG, DG, DG, DG, DG, DG,
		DG, DG, DG, DG, DG, DG, DG, DG, DG, DG,
		DG, DG, DG, DG, DG, DG, DG, DG, DG, DG};

		srand(10);
		
		food.bitmap = food_map;
		snake.bitmap = snake_head_map;
		snake.size = 1;

		food.xpos = rand() % 230;
		food.ypos = rand() % 310; 

		head.xpos = 120;
		head.ypos = 140;
		head.next = NULL;
		head.prev = NULL;

		snake.head = &head;
		snake.tail = &head;

		SystemInit();
		GLCD_Init();
		GLCD_Clear(WH);
		GLCD_SetBackColor(White);
    GLCD_SetTextColor(White);
		LED_Init();
		
		os_sys_init( base_task );

		while (1);
}
