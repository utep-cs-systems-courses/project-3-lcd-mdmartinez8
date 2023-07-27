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

static short redrawScreen = 0;

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
      update_game();
    }
    P1OUT &= ~LED;     //LED off when CPU is going to sleep
    or_sr(0x10);       // CPU asleep
    P1OUT |= LED;     //LED on when CPU awake
  }
}


void update_game()
{

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
  
}

void __interrupt_vec(PORT2_VECTOR) Port_2()
{
  if (P2IFG & SWITCHES)
  {
    P2IFG &= ~SWITCHES;
    switch_interrupt_handler();
  }
}

/*Gets called every time the CPU wakes up*/
void wdt_c_handler()
{


}
