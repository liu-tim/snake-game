#include <LPC17xx.h>
#include "glcd.h"
#include <RTL.h>
#include <stdio.h>
#include <stdlib.h>

#define BG	White
#define FG	Blue


typedef struct node{
	int xpos;
	int ypos;
	struct node * next;
} node_t;

typedef struct{
	int size; 
	node_t* head; 
	node_t* tail;
} snake_ll_t;

void move(snake_ll_t* snake){
	node_t* node = snake->head;
	
	while(node != NULL)
	{
		//node->xpos += 10;
		node->ypos += 10;
		node = node->next;
	}
}

void display(snake_ll_t* snake, unsigned short bitmap[]){
	node_t* node = snake->head;
	
	while(node != NULL)
	{
		GLCD_Bitmap(node->xpos, node->ypos,10,10, (unsigned char*)bitmap);
		node = node->next;
	}
}


void delay_ms(unsigned ms) { 
	unsigned t0 = os_time_get(); 
	printf("%d\n",t0);
	printf("%d\n",os_time_get());
	while ((os_time_get () - t0) < ms);
} 

int main( void ) {
		unsigned short bitmap[] = {FG, FG, FG, FG, FG, FG, FG, FG, FG, FG,
FG, FG, FG, FG, FG, FG, FG, FG, FG, FG,
FG, FG, FG, FG, FG, FG, FG, FG, FG, FG,
FG, FG, FG, FG, FG, FG, FG, FG, FG, FG,
FG, FG, FG, FG, FG, FG, FG, FG, FG, FG,
FG, FG, FG, FG, FG, FG, FG, FG, FG, FG,
FG, FG, FG, FG, FG, FG, FG, FG, FG, FG,
FG, FG, FG, FG, FG, FG, FG, FG, FG, FG,
FG, FG, FG, FG, FG, FG, FG, FG, FG, FG,
FG, FG, FG, FG, FG, FG, FG, FG, FG, FG};

		unsigned short BG_bitmap[] = {BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
BG, BG, BG, BG, BG, BG, BG, BG, BG, BG};


int i;
//initialize linked list
snake_ll_t snake;
node_t head;
node_t body;

snake.size = 1;

head.xpos = 160;
head.ypos = 140;

body.xpos = 160;
body.ypos = 130;

body.next = NULL;

head.next = &body;

snake.head = &head;

		SystemInit();
		GLCD_Init();

while (1){
		
		//GLCD_Clear(BG);
		//GLCD_Bitmap (head.xpos, head.ypos,10,10, (unsigned char*)bitmap);
		display(&snake, bitmap);
		//os_dly_wait (20);
		//delay_ms(20);
		for(i = 0; i < 1000000; i++){};
	  //GLCD_Bitmap (head.xpos, head.ypos,10,10, (unsigned char*)BG_bitmap);
		display(&snake, BG_bitmap);
		move(&snake);
}

}