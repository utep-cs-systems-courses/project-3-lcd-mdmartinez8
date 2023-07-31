#include <msp430.h>
#include <libTimer.h>
#include "lcdutils.h"
#include "lcddraw.h"

#define LED BIT6          //Only LED_GREEN available

#define SW1 1             //Define bits for 4 switch buttons
#define SW2 2
#define SW3 4
#define SW4 8
#define SWITCHES 15

void switch_init();

short redrawScreen = 0;

void main()
{
  P1DIR |= LED;
  P1OUT |= LED;
  configureClocks();
  lcd_init();
  switch_init();
  enableWDTInterrupts();
  or_sr(0x8);              /*GIE enabled*/

  clearScreen(COLOR_BLACK);
  while (1)
  {
    if (redrawScreen)
    {
      redrawScreen = 0;
    }
    P1OUT &= ~LED;     //LED off when CPU is going to sleep
    or_sr(0x10);       // CPU asleep
    P1OUT |= LED;     //LED on when CPU awake
  }
}


/* Function that produces values for switches*/
static char switch_update_interrupt_sense()
{
  char p2val = P2IN;

  P2IES |= (p2val & SWITCHES);
  P2IES &= (p2val | ~SWITCHES);
  return p2val;
}

/* Function that initializes SWITCHES for output and enables resistors*/
void switch_init()
{
  P2REN |= SWITCHES;
  P2IE |= SWITCHES;
  P2OUT |= SWITCHES;
  P2DIR &= ~SWITCHES;               //Sets up switches for input
  switch_update_interrupt_sense();
}

/*This function is called everytime a switch causes an interrupt*/
char switches = 0;
void switch_interrupt_handler()
{
  char p2val = switch_update_interrupt_sense();
  switches = ~p2val & SWITCHES;
  //redrawScreen = 1;
}

void __interrupt_vec(PORT2_VECTOR) Port_2()
{
  if (P2IFG & SWITCHES)
  {
    P2IFG &= ~SWITCHES;
    switch_interrupt_handler();
  }
}

/*Details for the moving ball*/
short colVelocity = 2, rowVelocity = 2;
short currentPos[2] = {screenWidth/2, screenHeight/2};
short colLimits[2] = {3, screenWidth}, rowLimits[2] = {3, screenHeight};
unsigned short ballColor = COLOR_BLACK;

void draw_ball(int col, int row, unsigned short color)
{
  fillRectangle(col-2, row-2, 5, 5, color);
}


void move_ball(char direction)
{
  short oldCol = currentPos[0];
  short oldRow = currentPos[1];

  if (((oldRow+rowVelocity) >= rowLimits[1]) || ((oldRow-rowVelocity) <= rowLimits[0]) || ((oldCol+colVelocity) >= colLimits[1]) || ((oldCol-colVelocity) <= colLimits[0]))
  {
    draw_ball(currentPos[0], currentPos[1], ballColor);
    return;
  }

  
  if (direction == 'U')
  {
    draw_ball(currentPos[0], currentPos[1] - rowVelocity, ballColor);
    currentPos[1] = currentPos[1] - rowVelocity;
  }
    
  if (direction == 'D')
  {
    draw_ball(currentPos[0], currentPos[1] + rowVelocity, ballColor);
    currentPos[1] = currentPos[1] + rowVelocity;
  }
    
  if (direction == 'L')
  {
    draw_ball(currentPos[0] - colVelocity, currentPos[1], ballColor);
    currentPos[0] = currentPos[0] - colVelocity;
  }
    
  if (direction == 'R')
  {
    draw_ball(currentPos[0] + colVelocity, currentPos[1], ballColor);
    currentPos[0] = currentPos[0] + colVelocity;
  }
}

/*Gets called every time the CPU wakes up*/
static int secCount = 0;
void wdt_c_handler()
{
  if (secCount++ >= 25)
  {
    secCount = 0;
  
    char dir;
    if (switches & SW1) dir = 'L';     // Move to the left
    if (switches & SW2) dir = 'U';
    if (switches & SW3) dir = 'D';
    if (switches & SW4) dir = 'R';

    move_ball(dir);
  
    redrawScreen = 1;
  }

}
