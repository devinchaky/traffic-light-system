// Lab5.c starter program EE319K Lab 5, Spring 2023
// Runs on TM4C123
// Devin Chaky
// 2/5/2023 version


/* Option A1, connect LEDs to PB5-PB0, switches to PA5-3, walk LED PF321
   Option A2, connect LEDs to PB5-PB0, switches to PA4-2, walk LED PF321
   Option A6, connect LEDs to PB5-PB0, switches to PE3-1, walk LED PF321
   Option A5, connect LEDs to PB5-PB0, switches to PE2-0, walk LED PF321
   Option B4, connect LEDs to PE5-PE0, switches to PC7-5, walk LED PF321
   Option B3, connect LEDs to PE5-PE0, switches to PC6-4, walk LED PF321
   Option B1, connect LEDs to PE5-PE0, switches to PA5-3, walk LED PF321
   Option B2, connect LEDs to PE5-PE0, switches to PA4-2, walk LED PF321
  */
// east/west red light connected to bit 5
// east/west yellow light connected to bit 4
// east/west green light connected to bit 3
// north/south facing red light connected to bit 2
// north/south facing yellow light connected to bit 1
// north/south facing green light connected to bit 0
// pedestrian detector connected to most significant bit (1=pedestrian present)
// north/south car detector connected to middle bit (1=car present)
// east/west car detector connected to least significant bit (1=car present)
// "walk" light connected to PF3-1 (built-in white LED)
// "don't walk" light connected to PF3-1 (built-in red LED)
#include <stdint.h>
#include "Lab5grader.h"
#include "../inc/tm4c123gh6pm.h"
// put both EIDs in the next two lines
char EID1[] = "abc77762"; //  ;replace abc123 with your EID
char EID2[] = "dmc4627"; //  ;replace abc123 with your EID

void DisableInterrupts(void);
void EnableInterrupts(void);

void Wait10ms(uint32_t time);// implemented in delay.s

// State Linked Data Structure
struct State {
	uint32_t OutE;
	uint32_t OutF;
	uint32_t Time;
	uint32_t Next[8];
};
typedef const struct State State_t;

// give states name
#define GoSouth 0
#define WaitSouth 1
#define AllRSouth 2
#define GoWest 3
#define WaitWest 4
#define AllRWest 5
#define Walk 6
#define Flash1 7
#define Off1 8
#define Flash2 9
#define Off2 10
#define Flash3 11
#define Off3 12
#define AllRWalk 13

// init FSM array
State_t FSM[14] = {
	{0x21, 0x02, 100, {GoSouth, WaitSouth, GoSouth, WaitSouth, WaitSouth, WaitSouth, WaitSouth, WaitSouth}},
	{0x22, 0x02, 50, {AllRSouth, AllRSouth, AllRSouth, AllRSouth,AllRSouth, AllRSouth, AllRSouth, AllRSouth}},
	{0x24, 0x02, 50, {Walk, GoWest, GoSouth, GoWest, Walk, Walk, Walk, GoWest}},
	{0x0C, 0x02, 100, {GoWest, GoWest, WaitWest, WaitWest, WaitWest, WaitWest, WaitWest, WaitWest}},
	{0x14, 0x02, 50, {AllRWest, AllRWest, AllRWest, AllRWest, AllRWest, AllRWest, AllRWest, AllRWest}},
	{0x24, 0x02, 50, {GoSouth, GoWest, GoSouth, GoSouth, Walk, Walk, GoSouth, Walk}},
	{0x24, 0x0E, 100, {Walk, Flash1, Flash1, Flash1, Walk, Flash1, Flash1, Flash1}}, 
	{0x24, 0x02, 25, {Off1, Off1, Off1, Off1, Off1, Off1, Off1, Off1}},
	{0x24, 0x00, 25, {Flash2, Flash2, Flash2, Flash2, Flash2, Flash2, Flash2, Flash2}},
	{0x24, 0x02, 25, {Off2, Off2, Off2, Off2, Off2, Off2, Off2, Off2}},
	{0x24, 0x00, 25, {Flash3, Flash3, Flash3, Flash3, Flash3, Flash3, Flash3, Flash3}},
	{0x24, 0x02, 25, {Off3, Off3, Off3, Off3, Off3, Off3, Off3, Off3}},
	{0x24, 0x00, 25, {AllRWalk, AllRWalk, AllRWalk, AllRWalk, AllRWalk, AllRWalk, AllRWalk, AllRWalk}},
	{0x24, 0x02, 50, {GoWest, GoWest, GoSouth, GoWest, Walk, GoWest, GoSouth, GoSouth}}
};

uint32_t S; // index to current state
uint32_t Input; // init global var to hold input

int main(void){ 
  DisableInterrupts();
  TExaS_Init(GRADER);
  // Initialize GPIO ports
	volatile uint32_t delay;
	SYSCTL_RCGCGPIO_R |= 0x31;
	delay = SYSCTL_RCGCGPIO_R;
	
	GPIO_PORTE_DIR_R |= 0x3F;
	GPIO_PORTA_DIR_R &= ~(0x1C);
	GPIO_PORTF_DIR_R |= 0x0E;
	
	GPIO_PORTE_DEN_R |= 0x3F;
	GPIO_PORTA_DEN_R |= 0x1C;
	GPIO_PORTF_DEN_R |= 0x0E;
	
	S = GoSouth;
	
  // Specify initial atate
  EnableInterrupts(); // grader, scope, logic analyzer need interrupts
  while(1){
    // set traffic lights
    // set walk lights
    // wait using Wait10ms
    // read input
    // next state depends on input and current state 
		GPIO_PORTE_DATA_R = (GPIO_PORTE_DATA_R & ~0x3F) | FSM[S].OutE; // set SW lights
		GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~0x0E) | FSM[S].OutF; // set walk lights
		Wait10ms(FSM[S].Time);	// delay in state
		Input = GPIO_PORTA_DATA_R & 0x1C;	// isolate input from switches
		Input >>= 2;	// shift input right 2 to get corresponding numerical next state
		S = FSM[S].Next[Input];	// set current state as next state
  }
}



