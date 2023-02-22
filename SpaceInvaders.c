// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 5/6/2020 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2019

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2019

 Copyright 2019 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWRE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "Print.h"
#include "Random.h"
#include "PLL.h"
#include "ADC.h"
#include "Images.h"
#include "Sound.h"
#include "Timer0.h"
#include "Timer1.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds
typedef enum {dead, alive} status_t;

struct sprite{
	int32_t x;			//x coordeinatr
	int32_t y;			//y coordicnate
	int32_t vx,vy; //pixels/30Hz
	const unsigned short *image;
	const unsigned short *black;
	status_t life;
	uint32_t w;	//width
	uint32_t h; //height
	uint32_t needDraw; //true if need to draw
};
typedef struct sprite sprite_t;
sprite_t Enemy[18];
sprite_t Player[1];
sprite_t Bullet[20];
sprite_t EnemyBullet[18];
//sprite_t Bunker[1];

typedef enum {English, Spanish} Language_t;
Language_t myLanguage = English;
typedef enum {LANGUAGE, PAUSED, PRESS, CONTINUE, GAMEOVER, GOODJOB, HUMAN, SCORE} phrase_t;
const char Language_English[] = "English";
const char Language_Spanish[] = "Espa\xA4ol";
const char Paused_English[] = "GAME PAUSED";
const char Paused_Spanish[] = "JUEGO PAUSADO";
const char Press_English[] = "Press button again";
const char Press_Spanish[] = "Presione el bot\xA2n";
const char Continue_English[] = "to continue";
const char Continue_Spanish[] = "para continuar";
const char GameOver_English[] = "GAME OVER";
const char GameOver_Spanish[] = "JUEGO TERMINADO";
const char GoodJob_English[] = "Good job,";
const char GoodJob_Spanish[] = "\xAD Bien hecho,";
const char Human_English[] = "Earthling!";
const char Human_Spanish[] = "humano!";
const char Score_English[] = "Score: ";
const char Score_Spanish[] = "Puntos: ";
const char *Phrases[8][2] = {
	{Language_English, Language_Spanish},
	{Paused_English, Paused_Spanish},
	{Press_English, Press_Spanish},
	{Continue_English, Continue_Spanish},
	{GameOver_English, GameOver_Spanish},
	{GoodJob_English, GoodJob_Spanish},
	{Human_English, Human_Spanish},
	{Score_English, Score_Spanish}
};

int Flag;
int Anyalive;
int now, last;
void GameInit(void){int i;
	
	Flag = 0;
	Player[0].x = 53;
	Player[0].y = 159;
	Player[0].image = PlayerShip0;
	Player[0].black = BlackBar;
	Player[0].life = alive;
	Player[0].w = 18;
	Player[0].h = 8;
	Player[0].needDraw = 1;
	
	
	for (i=0; i<15; i++){
		Bullet[i].y = 153;
		Bullet[i].vx = 0;
		Bullet[i].vy = -1;
		Bullet[i].image = Laser;
		Bullet[i].black = BlackLaser;
		Bullet[i].life = dead;
		Bullet[i].w = 2;
		Bullet[i].h = 3;
		Bullet[i].needDraw = 0;
	}
	
	for (i=0; i<18; i++){
		EnemyBullet[i].y = Enemy[i].y;
		EnemyBullet[i].x = Enemy[i].x;
		EnemyBullet[i].vx = 0;
		EnemyBullet[i].vy = 1;
		EnemyBullet[i].image = Laser;
		EnemyBullet[i].black = BlackLaser;
		EnemyBullet[i].life = dead;
		EnemyBullet[i].w = 4;
		EnemyBullet[i].h = 6;
		EnemyBullet[i].needDraw = 0;
	}
	
int randomval = 18;
		
	for(i=0; i<6 ; i++){
		Enemy[i].x = randomval*i;
		Enemy[i].y = 10;
		Enemy[i].vx = 0;
		Enemy[i].vy = 0;
		Enemy[i].image = SmallEnemy10pointA;
		Enemy[i].black = BlackEnemy;
		Enemy[i].life = alive;
		Enemy[i].w = 16;
		Enemy[i].h = 10;
		Enemy[i].needDraw = 1;}

	for(i=6;i<12;i++){
		Enemy[i].x = randomval*(i-6);
		Enemy[i].y = 25;
		Enemy[i].vx = 0;
		Enemy[i].vy = 0;
		Enemy[i].image = SmallEnemy10pointA;
		Enemy[i].black = BlackEnemy;
		Enemy[i].life = alive;
		Enemy[i].w = 16;
		Enemy[i].h = 10;
		Enemy[i].needDraw = 1;}
	
	for(i=12;i<18;i++){
		Enemy[i].x = randomval*(i-12);
		Enemy[i].y = 40;
		Enemy[i].vx = 0;
		Enemy[i].vy = 0;
		Enemy[i].image = SmallEnemy10pointA;
		Enemy[i].black = BlackEnemy;
		Enemy[i].life = alive;
		Enemy[i].w = 16;
		Enemy[i].h = 10;
		Enemy[i].needDraw = 1;}
	
}

//Moving/////////////////////////////////////////////////////////////
int moveLimit = 450;
int moveCount = 0;
int downLimit = 1300;
int downCount = 0;
int direction = 0; //0right;1left
void EnemyMove(void){
	int i;
	int down;

	for(i=0;i<18;i++){
		if(moveCount == moveLimit){
				moveCount = 0;
				if (direction == 0){
					direction = 1;
				}
				else if (direction == 1){
					direction = 0;
				}
		}
			else{
			moveCount++;
		}
			
		if(downCount == downLimit){
			downCount = 0;
			//Enemy[i].y += 1;
			Enemy[i].vy += 1;
			if (Anyalive == 1){
				Sound_Fastinvader4();
			}
		}
		else{
			downCount++;
		}
			
		
		if(Enemy[i].life == alive){
			Enemy[i].needDraw = 1;
			Anyalive = 1;
			if(Enemy[i].y > 141){
				Enemy[i].life = dead;
				Sound_Explosion();
			}
			else if(Enemy[i].y < 10){
				Enemy[i].life = dead;
			}
			else if((moveCount < moveLimit) && (direction == 0)){
				Enemy[i].vx = 1;
				Enemy[i].x += Enemy[i].vx;
				Enemy[i].y += Enemy[i].vy;
			}
			else if((moveCount < moveLimit) && (direction == 1)){
				Enemy[i].vx = -1;
				Enemy[i].x += Enemy[i].vx;
				Enemy[i].y += Enemy[i].vy;
			}
		}
	}
}

void ShipMove(void){
	if(Player[0].life == alive){
		Player[0].needDraw = 1;
		Player[0].x = ADC_In() * 110/4096;
	}
}

void FireMove(void){
	int i;
	for(i=0;i<15;i++){
		if(Bullet[i].life == alive){
		
			if(Bullet[i].y < 0){
					Bullet[i].life = dead;
					Bullet[i].y = 153;
					Bullet[i].needDraw = 0;
				}
			else{
				ST7735_DrawBitmap(Bullet[i].x,Bullet[i].y,Bullet[i].black,Bullet[i].w,Bullet[i].h);
				Bullet[i].y += Bullet[i].vy;
				ST7735_DrawBitmap(Bullet[i].x,Bullet[i].y,Bullet[i].image,Bullet[i].w,Bullet[i].h);
			}
		}
		Bullet[i].needDraw = 0;
	}
}


//Drawing////////////////////////////////////////////////////////////
void EnemyDraw(void){
int i;
	for(i=0;i<18;i++){
		if(Enemy[i].needDraw == 1){
			if(Enemy[i].life == alive){
				ST7735_DrawBitmap(Enemy[i].x,Enemy[i].y,Enemy[i].image,Enemy[i].w,Enemy[i].h);
			}
			else{
				ST7735_DrawBitmap(Enemy[i].x,Enemy[i].y,Enemy[i].black,Enemy[i].w,Enemy[i].h);
			}
			Enemy[i].needDraw = 0;
		}	
	}
}
	
void ShipDraw(void){
	if((Player[0].needDraw == 1) && (Player[0].life == alive)){
		ST7735_DrawBitmap(0,159,Player[0].black,128,8);
		ST7735_DrawBitmap(Player[0].x,Player[0].y,Player[0].image,Player[0].w,Player[0].h);
	}
	Player[0].needDraw = 0;
}

void BunkerDraw(void){
	ST7735_DrawBitmap(53, 151, Bunker0, 18,5);
}
void LineDraw(void){
	ST7735_DrawBitmap(1, 140, Line, 127, 2);
}

//Functions that Move and Draw///////////////////////////////////////
void GameMove(void){
	Anyalive = 0;
	ShipMove();
	EnemyMove();
	FireMove();
}

void GameDraw(void){
	EnemyDraw();
	ShipDraw();
	//BunkerDraw();
	LineDraw();
}

void GameTask(void){
	
//Button Intterupt///////////////////////////////////////////////////
	GameMove();
	//GameDraw();
	Flag = 1;
}

void PortE_Init(void){volatile long delay;                            
  SYSCTL_RCGCGPIO_R |= 0x10;           // activate port F
  while((SYSCTL_PRGPIO_R&0x10)==0){}; // allow time for clock to start
  delay = 100;                  //    allow time to finish activating
  GPIO_PORTE_DIR_R &= ~0x11;    // 5) make PF4,0 in
  //GPIO_PORTE_AFSEL_R &= ~0x11;  // 6) disable alt funct on PF4,0
	//GPIO_PORTE_PUR_R |= 0x11;     
  GPIO_PORTE_DEN_R |= 0x11;     // 7) enable digital I/O on PF4,0
	GPIO_PORTE_PCTL_R &= ~0x0000FFFF;
  GPIO_PORTE_IS_R &= ~0x11;     //    PF4,0 is edge-sensitive
  GPIO_PORTE_IBE_R &= ~0x11;    //    PF4,0 is not both edges
  //GPIO_PORTE_IEV_R &= ~0x11;     //    PF4,0 falling edge event (Neg logic)
  GPIO_PORTE_ICR_R = 0x11;      //    clear flag1-0
  GPIO_PORTE_IM_R |= 0x11;      // 8) arm interrupt on PF4,0
                                // 9) GPIOF priority 2
  NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF1F)|0xA0;
  NVIC_EN0_R |= 0x10;   // 10)enable interrupt 30 in NVIC
}

int lang;
int shotIndex = 0;

void GPIOPortE_Handler(void){
	if ((GPIO_PORTE_RIS_R & 0x01)==0x01){ //PE0
		Sound_Shoot();
		int i = 0;
		for (i=0;i<15;i++){
			if (Bullet[i].life == dead){
				shotIndex = i;
				break;
			}
		}
			Bullet[shotIndex].life = alive;
			Bullet[shotIndex].needDraw = 1;
			Bullet[shotIndex].x = Player[0].x + 9;
			Bullet[shotIndex].y = 153;
	}
	
	if ((GPIO_PORTE_RIS_R & 0x10)==0x10){ //PE4
		ST7735_FillScreen(0x0000);   // set screen to black
		if(lang == 1){
			ST7735_SetCursor(5, 1);
			ST7735_OutString((char *)Phrases[PAUSED][English]);
			ST7735_SetCursor(2, 4);
			ST7735_OutString((char *)Phrases[PRESS][English]);
			ST7735_SetCursor(4, 5);
			ST7735_OutString((char *)Phrases[CONTINUE][English]);
		}
		else if(lang == 0){
			ST7735_FillScreen(0x0000);   // set screen to black
			ST7735_SetCursor(5, 1);
			ST7735_OutString((char *)Phrases[PAUSED][Spanish]);
			ST7735_SetCursor(2, 4);
			ST7735_OutString((char *)Phrases[PRESS][Spanish]);
			ST7735_SetCursor(4, 5);
			ST7735_OutString((char *)Phrases[CONTINUE][Spanish]);
		}
		
		while((GPIO_PORTE_DATA_R & 0x10)== 0){};
		Delay100ms(5);
		while((GPIO_PORTE_DATA_R & 0x11)== 0x10){};
		ST7735_FillScreen(0x0000);   // set screen to black
	}	
  GPIO_PORTE_ICR_R = 0x11;      // acknowledge flag4
}


int score = 0;
void collisions(void){
	int i,j;
	for (i = 0; i<18; i++){
		for(j=0;j<15;j++){
			if((Enemy[i].life == alive) && (Bullet[j].life == alive) && (Enemy[i].y == Bullet[j].y) && (Bullet[j].x >= Enemy[i].x) && (Bullet[j].x <= Enemy[i].x + 16)){
				Enemy[i].life=dead;
				Enemy[i].needDraw = 0;
				Bullet[j].life = dead;
				Bullet[j].needDraw = 0;
				Sound_Killed();
				score += 10;
				ST7735_DrawBitmap(Enemy[i].x,Enemy[i].y,Enemy[i].black,Enemy[i].w,Enemy[i].h);
				ST7735_DrawBitmap(Bullet[j].x,Bullet[j].y,Bullet[j].black,Bullet[j].w,Bullet[j].h);
			}
		}
	}
}


int Language(void){			//1English0Spanish
	ST7735_SetCursor(2, 1);
  ST7735_OutString("INVADERS OF SPACE");
	ST7735_SetCursor(1, 4);
  ST7735_OutString("Select Language:");
	ST7735_SetCursor(1, 6);
	ST7735_OutString((char *)Phrases[LANGUAGE][English]);
	ST7735_SetCursor(11, 6);
	ST7735_OutString((char *)Phrases[LANGUAGE][Spanish]);
	  
	SYSCTL_RCGCGPIO_R |= 0x10;           // activate port F
  while((SYSCTL_PRGPIO_R&0x10)==0){}; // allow time for clock to start
  GPIO_PORTE_DIR_R &= ~0x11;    // 5) make PF4,0 in
  GPIO_PORTE_DEN_R |= 0x11;     // 7) enable digital I/O on PF4,0
		
	while ((GPIO_PORTE_DATA_R & 0x11) == 0x00){};
	if ((GPIO_PORTE_DATA_R & 0x11)== 0x10){
		while((GPIO_PORTE_DATA_R & 0x11)== 0x10){};
		return 1;
	}
	else if ((GPIO_PORTE_DATA_R & 0x11) == 0x01){
		while((GPIO_PORTE_DATA_R & 0x11)== 0x01){};
		return 0;
	}
}

//Main Function//////////////////////////////////////////////////////	

int main(void){
  DisableInterrupts();
  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
  Random_Init(1);
	ADC_Init();
	
  Output_Init();
	Sound_Init();
	GameInit();
	ST7735_FillScreen(0x0000);
	lang = Language();
	PortE_Init();
  ST7735_FillScreen(0x0000);            // set screen to black
  
  ST7735_DrawBitmap(53, 159, PlayerShip0, 18,8); // player ship bottom
  //ST7735_DrawBitmap(53, 151, Bunker0, 18,5);
	
	Timer1_Init(&GameTask,80000000/25);
	EnableInterrupts();
	
	//last = PortE_Input();
	do{
		while(Flag==0){};
			Flag = 0;
			collisions();
			GameDraw();
	}
	while(Anyalive);
	Delay100ms(50);
	DisableInterrupts();
	if (lang == 1){
		ST7735_FillScreen(0x0000);   // set screen to black
		ST7735_SetCursor(3, 4);
		ST7735_OutString((char *)Phrases[GAMEOVER][English]);
		ST7735_SetCursor(4, 6);
		ST7735_OutString((char *)Phrases[GOODJOB][English]);
		ST7735_SetCursor(5, 7);
		ST7735_OutString((char *)Phrases[HUMAN][English]);
		ST7735_SetCursor(4, 8);
		ST7735_OutString((char *)Phrases[SCORE][English]);
		LCD_OutDec(score);
	}
	else if (lang == 0){
		ST7735_FillScreen(0x0000);   // set screen to black
		ST7735_SetCursor(3, 4);
		ST7735_OutString((char *)Phrases[GAMEOVER][Spanish]);
		ST7735_SetCursor(4, 6);
		ST7735_OutString((char *)Phrases[GOODJOB][Spanish]);
		ST7735_SetCursor(5, 7);
		ST7735_OutString((char *)Phrases[HUMAN][Spanish]);
		ST7735_SetCursor(4, 8);
		ST7735_OutString((char *)Phrases[SCORE][Spanish]);
		LCD_OutDec(score);
	}
  while(1){
  }

}
/////////////////////////////////////////////////////////////////////////////////////////////////////














int realmain(void){
  DisableInterrupts();
  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
  Random_Init(1);

  Output_Init();
  ST7735_FillScreen(0x0000);            // set screen to black
  
  ST7735_DrawBitmap(22, 159, PlayerShip0, 18,8); // player ship bottom
  ST7735_DrawBitmap(53, 151, Bunker0, 18,5);
  ST7735_DrawBitmap(42, 159, PlayerShip1, 18,8); // player ship bottom
  ST7735_DrawBitmap(62, 159, PlayerShip2, 18,8); // player ship bottom
  ST7735_DrawBitmap(82, 159, PlayerShip3, 18,8); // player ship bottom

  ST7735_DrawBitmap(0, 9, SmallEnemy10pointA, 16,10);
  ST7735_DrawBitmap(20,9, SmallEnemy10pointB, 16,10);
  ST7735_DrawBitmap(40, 9, SmallEnemy20pointA, 16,10);
  ST7735_DrawBitmap(60, 9, SmallEnemy20pointB, 16,10);
  ST7735_DrawBitmap(80, 9, SmallEnemy30pointA, 16,10);
  ST7735_DrawBitmap(100, 9, SmallEnemy30pointB, 16,10);

  Delay100ms(50);              // delay 5 sec at 80 MHz

  ST7735_FillScreen(0x0000);   // set screen to black
  ST7735_SetCursor(1, 1);
  ST7735_OutString("GAME OVER");
  ST7735_SetCursor(1, 2);
  ST7735_OutString("Nice try,");
  ST7735_SetCursor(1, 3);
  ST7735_OutString("Earthling!");
  ST7735_SetCursor(2, 4);
  LCD_OutDec(1234);
  while(1){
  }

}


// You can't use this timer, it is here for starter code only 
// you must use interrupts to perform delays
void Delay100ms(uint32_t count){uint32_t volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}
