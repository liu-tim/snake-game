#include <LPC17xx.h>
#include "glcd.h"
#include "KBD.h" 
#include <RTL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WH	White
#define GR	Green
#define RE  Red
#define DG 	0x0300                                                                 
                                                                                    
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
	unsigned short*bitmap; 
} snake_ll_t;

void move(snake_ll_t* snake, uint32_t heading){
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
      break; /* optional */
	
   case 89  : //right
      node->xpos += 10;
      break; /* optional */
	 
   case 105: //up 
			node->ypos -= 10;
      break; /* optional */
   
	 case 57: //down 
      node->ypos += 10;
      break; /* optional */ 
	}
	
}

void display(snake_ll_t* snake, food_t* food, unsigned short bitmap[]){
	node_t* node = snake->head;
	
	while(node != NULL)
	{
		if (node ==                                                                                                                                                                                         snake->head) GLCD_Bitmap(node->xpos, node->ypos,10,10, (unsigned char*)(snake->bitmap));
		else GLCD_Bitmap(node->xpos, node->ypos,10,10, (unsigned char*)(bitmap));
		node = node->next;
	}
		GLCD_Bitmap(food->xpos, food->ypos,10,10, (unsigned char*)(food->bitmap));
}

void clear_tail(snake_ll_t* snake, unsigned short bitmap[]){
	if(snake->tail != NULL)
	{
		GLCD_Bitmap(snake->tail->xpos, snake->tail->ypos,10,10, (unsigned char*)bitmap);
	}
}

int is_out(node_t* head) {
		if((head->xpos < 0) || (head->xpos > 230) || (head->ypos < 0) || (head->ypos > 310)) 
			return 1;
		else return 0;
}	

int is_collision(snake_ll_t* snake) {
	node_t* temp;
	if (snake->head != NULL)
		temp = snake->head;
	else
		return 0;//error
	
	while(temp->next != NULL) {
		//check collision between head and temp->next
		if((snake->head->xpos == temp->next->xpos) && (snake->head->ypos == temp->next->ypos) ) {
			printf("Collision\n");
			return 1; //game over
		}                                                                                                                                                         
		//iterate temp to temp->next
		temp = temp->next;
	}
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
void spawn_food(food_t* food) {
	clear_spot(food->xpos, food->ypos);
	food->xpos = rand() % 230;
	food->ypos = rand() % 310; 
}
	
void grow_snake(snake_ll_t* snake) {
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

void eat(snake_ll_t* snake, food_t* food) {
	if((fabs(snake->head->xpos - food->xpos) < 10) && (fabs(snake->head->ypos - food->ypos) < 10)) {
			printf("eat\n");
		grow_snake(snake); 
		spawn_food(food);
		
	}
}

void delay_ms(unsigned ms) { 
	unsigned t0 = os_time_get(); 
	printf("%d\n",t0);
	printf("%d\n",os_time_get());
	while ((os_time_get () - t0) < ms);
} 

int main( void ) {

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

int i; //used for delay
//uint32_t heading; //used for heading position 
//initialize linked list
snake_ll_t snake;
node_t head;
node_t body;
food_t food; 
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


srand(1);
food.bitmap = food_map;
snake.bitmap = snake_head_map; 

snake.size = 1;

food.xpos = rand() % 230;
food.ypos = rand() % 310; 

head.xpos = 160;
head.ypos = 140;

//body.xpos = 160;
//body.ypos = 130;

//body.next = NULL;
//body.prev = &head; 

head.next = NULL;//&body;
head.prev = NULL;

snake.head = &head;
snake.tail = &head;//&body; 

		SystemInit();
		GLCD_Init();
		GLCD_Clear(WH);
		//SER_Init ();                            /* initialize serial interface     */
		//LED_Init ();                            /* initialize LEDs                 */
		KBD_Init ();                            /* initialize Push Button          */


while (1){
		move(&snake, KBD_Get());
		if(is_out(snake.head) == 1 || is_collision(&snake) == 1) break;
		eat(&snake, &food);
		display(&snake, &food, snake_bitmap);
		//os_dly_wait (20);
		for(i = 0; i < 1000000; i++){};
	  //GLCD_Bitmap (head.xpos, head.ypos,10,10, (unsigned char*)BG_bitmap);
		//display(&snake, &food, background_bitmap);
		clear_tail(&snake, background_bitmap);
		
}
		GLCD_Clear(WH);

}