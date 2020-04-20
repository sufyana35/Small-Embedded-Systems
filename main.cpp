/*
 * @Brief Assignment for Small Embedded Systems Pong Game cm0506
 *				This program is for the pong game assignment for cm0506.
 *				The project must be inside the EN0572 workspace in order to build correctly
 *				A basic framework for a uCOS-II program is provided. 
 *
 * @Author(s): 14014989 
 *
 */


#include <stdbool.h>
#include <mbed.h>
#include <display.h>
#include <MMA7455.h>
#include <time.h>


/*
*********************************************************************************************************
*                                            APPLICATION TASK STACKS
*********************************************************************************************************
*/
static Display *d = Display::theDisplay(); 
static int32_t TIMER_INTERVAL = 0;

int remainingBalls; //int for remaining balls 
int score; //int for remaining scores
int hitSuccess; //int for the amount of times the ball is hit with the bat
int scoreSpeed; //int for the score speed
int start;
int y;
int magicStatus;
float potVal;

#define H_MIN 10 //horizontal minimum
#define H_MAX 460  //horizontal maximum
#define V_MIN 25 //vertical minimum
#define V_MAX 260 //vertical maximum

#define BAT_W 40 //bat width
#define BAT_H 4 //bat height
#define BAT_S 15 //bat speed
#define BALL_RAD 5 //ball radius
#define BALL_V 5 //ball velocity

#define OBST_MIN 40 //obstacle minimum horizontal
#define OBST_MAX 200 //obstacle maximum horizontal
#define OBST_H 2 //obstacle height
#define  APP_TASK_TIMER_STK_SIZE             256
/*
*********************************************************************************************************
*                                            APPLICATION FUNCTION PROTOTYPES
*********************************************************************************************************
*/
typedef struct Obstacle {
	int x, y; //position
} obstacle_t;

obstacle_t obst;

typedef struct Ball {
	int x, y; //position
	int vx, vy; //velocity
} ball_t;

ball_t ball;

typedef struct BatPos {
	int x, y; //position
} batPos_t;

batPos_t bat;

typedef enum {
	JLEFT = 0,
	JRIGHT,
	JUP,
	JDOWN,
	JCENTER,
	APP_TASK_TIMER_PRIO
} buttonId_t;


/*
*********************************************************************************************************
*                                            GLOBAL TYPES AND VARIABLES 
*********************************************************************************************************
*/


static bool buttonPressedAndReleased(buttonId_t button); //checks which buttons are being pressed
static void initialiseDevices(); //intialises the display
static void initialiseGame(); // intialises the game scoring and remaining balls
static void initialiseBatBall(); //intialises the ball and bat
static void initialiseObstacle(); //intialise the obstacle
static void waitForStart(); //the program will be delayed until the JCENTRE is pressed
static void delay(uint32_t ms); //delays the program 
static void updateBall(); //this moves the ball around and updates the screen
static void adjustBat(); //this moves the bat left and right on the screen
static bool testBottom(); //this checks if the ball hits the bottom of the screen
static bool testBatHit(); //this checks if the ball collides with the bat
static bool testObstHit(); //this checks if the obstacle is hit
static void updateScores(); // this will update the score whenever the ball hits the walls
static void magicTime();



//timer interrupt and handler
static void timerHandler();
int tickCt = 0;

int obstMinMax = OBST_MIN + rand() % (OBST_MAX - OBST_MIN);
/*
*********************************************************************************************************
*                                            GLOBAL FUNCTION DEFINITIONS
*********************************************************************************************************
*/

int main() {
	Ticker ticktock;
ticktock.attach(&timerHandler, 1);
	
	initialiseGame();
	initialiseDevices();  

		while (remainingBalls > 0) {	
			waitForStart();
			initialiseBatBall();
			initialiseObstacle();
			ballSpeed()
			bool ballInPlay = 1;		
	
		while (ballInPlay) {
			updateBall();
			adjustBat();
			ballSpeed()
			magicTime();
			
			ballInPlay = testBottom();
			delay(50);
		}     
		initialiseDevices();
		if (remainingBalls == 0) {
			waitForStart();
			initialiseGame();
		}

	}
}

static void initialiseGame() { 
	remainingBalls = 5;
	score = 0;
	scoreSpeed = 1;
	hitSuccess = 0;
}

static void initialiseDevices() { 
	//the display is intialised
	d->fillScreen(WHITE);
	d->setTextColor(BLACK, WHITE);
	d->drawRect(5, 20, 470, 270, BLACK);
	d->drawRect(5, 0, 470, 20, BLACK);
	d->setCursor(10, 7);
	d->printf("BALLS REMAINING : %d", remainingBalls);
	d->setCursor(300, 7);
	d->printf("SCORE : %d", score);
}		

static void initialiseBatBall() { 
	//the bat and ball positioning
	//random H value 
	int randomX = H_MIN + rand() % (H_MAX - H_MIN);
	//random V value
	int randomY = V_MIN + rand() % ((V_MAX/2) - V_MIN);
		
  ball.x = randomX;
	ball.y = randomY;
	ball.vx = BALL_V;
	ball.vy = BALL_V;
		
	bat.x = H_MAX/2;
	bat.y = V_MAX - BAT_H;
}

static void magicTime() {
	magicStatus = 0;
	int interval;
	interval = 5 + (rand() % 10);
	
	int magicDuration;
	magicDuration = 10 + (rand() % 2);
	
	if (tickCt > 45) {
			
			magicStatus = 1;			
			d->fillCircle(ball.x, ball.y, BALL_RAD , RED);
		}
	else {
			d->fillCircle(ball.x, ball.y, BALL_RAD , WHITE);
			d->fillCircle(ball.x, ball.y, BALL_RAD , BLUE);
			magicStatus = 0;
	}
	
}

static void initialiseObstacle() {
	//obstacle positioning
	int obstacleX =  (H_MIN+30) + rand() % (H_MAX - H_MIN);
	int obstacleY = (V_MIN+75);
	obst.x = obstacleX;
	obst.y = obstacleY;
	
	d->fillRect(obst.x, obst.y, obstMinMax, OBST_H, GREEN);
}

static void waitForStart() { 
	//if the centre button is not pressed then the delay will be active
	while (!buttonPressedAndReleased(JCENTER)) {
		delay(1);
		start =1;
	}
}


static void adjustBat() { 
//remove bat
	d->fillRect(bat.x, bat.y, BAT_W, BAT_H , WHITE);
	if (buttonPressedAndReleased(JLEFT)) {
		if (bat.x <= H_MAX - BAT_W) { 
			//bat moves left
			bat.x += BAT_S;
		}
	} else if (buttonPressedAndReleased(JRIGHT)) {
		//bat moves right
		if (bat.x >= H_MIN) {
			bat.x -= BAT_S;
		}
	}
	//redraw bat
	d->fillRect(bat.x, bat.y, BAT_W, BAT_H , BLACK);
	
}

void ballSpeed(){
	AnalogIn pot(p15);
  potVal = 1.0F - pot.read();

	ball.vx = ((potVal *  3) + 0.25);
	ball.vy = ((potVal *  3) + 0.25);

 }

static void updateBall() {
	//remove ball 
	d->fillCircle(ball.x, ball.y, BALL_RAD , WHITE);
	
	//Move the ball by increasing velocity to x and y and adjust vx and vy on collision
	ball.x += ball.vx;
	ball.y += ball.vy;
	
	//verticle wall Collision then vx = -vx
	//horizontal wall Collision then vy = -vy
	if (ball.x >= H_MAX) {
		//wall on the right
		ball.vx = -(ball.vx);
	} else if (ball.x <= H_MIN) {
		//wall on the left
		ball.vx = abs(ball.vx);
	} else if (ball.y <= V_MIN) {
		//the top wall
		updateScores();
		ball.vy = abs(ball.vy);
	} else if (testBatHit()) {
        ball.vy = -(ball.vy);
	} else if (testObstHit()) {
				ball.vy = -(ball.vy);
	}
	//redraw ball
	
	
	d->drawRect(5, 20, 470, 270, BLACK);
	d->drawRect(5, 0, 470, 20, BLACK);
}

static bool testBatHit() {
	//this will check if there is a collision between the ball and bat
	if ((ball.x >= bat.x && ball.x <= bat.x + BAT_W) && (ball.y >= bat.y && ball.y <= bat.y + BALL_RAD)) {
		return 1;
	} else {
		return 0;
	}
}

static bool testObstHit() {
	if ((ball.x >= obst.x && ball.x <=obst.x + obstMinMax) && (ball.y >= obst.y && ball.y <= obst.y + BALL_RAD)) {
		return 1;
	} else {
		return 0;
	}
}

static bool testBottom() {
	//this will check if the ball misses the bat and falls through the bottom of the screen
	if (ball.y >= V_MAX) {
		//a life will be removed if the bat does not hit the ball
		remainingBalls--;
		
		//the hit success will be returned to 0
		hitSuccess = 0;
		
		//the score speed will be returned to 1
		scoreSpeed = 1;
    
		//waits for start (until the jcentre is pressed)
		waitForStart();
		//the bat and ball will be cleared from the screen
		d->fillRect(bat.x, bat.y, BAT_W, BAT_H , WHITE);
		d->fillCircle(ball.x, ball.y, BALL_RAD , WHITE);
    d->setCursor(10, 7);
		d->printf("BALLS REMAINING : %d", remainingBalls);   

		return 0;
	} else {
		return 1;
	}
}

static void updateScores() {
	//the hit success will increase if the the ball is hit
	hitSuccess++;
	//if the hit success is a multiple of 5 then the score speed will be inceased
	if (hitSuccess % 5 == 0) {
		scoreSpeed++;
	}
	//the scores will be added together
	score += scoreSpeed;
	d->setCursor(300, 7);
	d->printf("SCORE : %d", score);
	
	if (magicStatus == 1) {
		
		score += scoreSpeed;
	}
	
	d->setCursor(300, 7);
	d->printf("SCORE : %d", score);
}

static void timerHandler() {
		if ((start == 1)&&(y<282)) {
      
			tickCt++;
	}
		else
		{
			tickCt = 0;
		}
	d->setCursor(390, 7);
	d->printf("Timer Count: %d", tickCt);
}

		/*
*********************************************************************************************************
*                                            APPLICATION TASK DEFINITIONS
*********************************************************************************************************
*/
	
//checks which button is being pressed
bool buttonPressedAndReleased(buttonId_t b) {
	static DigitalIn buttons[] = {P5_0, P5_4, P5_2, P5_1, P5_3}; // LEFT, RIGHT, CENTER
	
	bool result = false;
	uint32_t state;
	static uint32_t savedState[4] = {1,1,1,1};
    
  state = buttons[b].read();
  if (savedState[b] == 0) {
		result = true;
	}
	savedState[b] = state;
	return result;
}

//delay function which is used in the waitForStart
void delay(uint32_t ms) {
	volatile uint32_t i;
	volatile uint32_t j;
	
	for (i=ms; i>0; i--) {
		for (j=11850; j>0; j--) {
		}
	}	
}