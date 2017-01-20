# Snake-Game
Classic snake game recreated on Keil MCB 1700 microcontroller. Created using threads, interrupts, and various peripherals in C programming. 

## How to Play
The user starts the game by pressing the INT0 button. A ‘snake’ of length 1 (just its head) 
is spawned and shown on the LCD. One ‘food object’ is also spawned in a random 
position. Immediately the snake begins moving and continues to do so automatically 
throughout the game. The user is able to control the direction of the snake\s head\`s 
movement using the joystick. The ̀body/tail’ of the snake follows its ‘head’. The snake is 
only able to change direction perpendicularly to its current heading. If the snake collides 
with the food object (eats the food), its length (body) is increased by 1. Once ‘eaten’, the 
food object will disappear from the LCD, and respawn in another random location. The 
length of the snake, or game score, will be displayed on the LEDs. The goal of the game 
is to collect as much food as possible, and as such, increase the length of the snake. The 
game resets if the head of the snake collides with its own body, or the wall (edge of LCD 
display). 

##Threads
```__task void base_task( void )``` Initialises the snake game, snake object, and other tasks

```__task void get_inputs()``` Records the player's input

``` __task void manager() ``` Tracks the snake's movement and game over conditions

```__task void display_task()``` Displays the game on the led screen



