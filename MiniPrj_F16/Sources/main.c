/*
************************************************************************
 ECE 362 - Mini-Project C Source File - Fall 2016
***********************************************************************
	 	   			 		  			 		  		
 Team ID: < 11 >

 Project Name: < Stacker-Mini >

 Team Members:

   - Team/Doc Leader: < Zane Johnson >      Signature: _____Zane Johnson_____
   
   - Software Leader: < Cameron Ellison >   Signature: ____Cameron Ellison___

   - Interface Leader: < Dale Klose >       Signature: ____Dale Klose________

   - Peripheral Leader: < Dongqi Wu >       Signature: ____Dongqi Wu_________


 Academic Honesty Statement:  In signing above, we hereby certify that we 
 are the individuals who created this HC(S)12 source file and that we have
 not copied the work of any other student (past or present) while completing 
 it. We understand that if we fail to honor this agreement, we will receive 
 a grade of ZERO and be subject to possible disciplinary action.

***********************************************************************

 The objective of this Mini-Project is to create a mini, handheld version of the 
 well known arcade game "Stacker". The goal of the game is to align rows of moving 
 blocks on top of each other. The blocks increase in speed as the number of rows increase. 
 A sound needs to play when the blocks hit the side of the game and when the user pushes
 the input button. We want the user to be able to increase the difficulty using a potentiometer.
 Increasing the difficulty can be accomplished by making the TC7 variable smaller.
 We also want the user to be able to pause the game and come back where they left off.


***********************************************************************

 List of project-specific success criteria (functionality that will be
 demonstrated):

 1. Successfully utilize and update the TIM interrupt to play a game

 2. Increase the difficulty using the ATD from a potentiometer
    
 3. Successfully use external pushbuttons, potentiometers, LED matrices, etc. 

 4. Make it portable and handheld, functioning with a portable battery pack

 5. Use the PWM to incorporate sound through a headphone jack

***********************************************************************

  Date code started: < 11/21/2016 >

  Update history (add an entry every time a significant change is made):

  Date: < 11/21/2016 >  Name: < Zane Johnson >   Update: < Set up all needed initializations >
  
  Date: < 11/22/2016 >  Name: < Zane Johnson >   Update: < Got the array working how it should along with the ATD, just need to get the LED matrix to work >

  Date: < 11/23/2016 >  Name: < Everyone >   Update: < The array now is output to the LED matrix with ease. Any 8x8 array will output using SPI >

  Date: < 11/29/2016 >  Name: < Zane Johnson >   Update: < Enabled the PWM to produce sounds when certain functions are called >

  Date: < 11/30/2016 >  Name: < Everyone >   Update: < Finished up everything so that the whole game was functioning properly >
***********************************************************************
*/

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <mc9s12c32.h>
 
 
 

 

/* All functions after main should be initialized here */
char inchar(void);
void outchar(char x);
void update_display(int *, int *);
int check_win(void);
int toggle(int); 
void lcdwait(void);
void shiftout(char x, char y);
void windisp(void);
void delay(void);
void delay1(void);
void hbeep(void);
void lbeep(void);
void Delay0(int delayTime);
#define col0  0x80
#define col1  0x40
#define col2  0x20
#define col3  0x10
#define col4  0x08
#define col5  0x04
#define col6  0x02
#define col7  0x01

/* Variable declarations */
char leftpb	= 0;  // left pushbutton flag
char rghtpb	= 0;  // right pushbutton flag
int p_leftpb = 0;    // previous state of pushbuttons (variable)
int p_rghtpb = 0;
int p_2 = 0;
int ATDCH0 = 0; //value of ATD potentiometer
int mid = 4;  //middle block
int right = 0; //right block
int left = 0; //left block
int direction = 1; //direction of blocks
int i = 0; //indexing variables
int k = 0;
int pause = 0; //pause flag 
int row = 0; //current row
int win = 0; //flag for if user won
int cnt = 0; //count variable
int hundred = 0;  //hundred flag for TIM
unsigned char x = 0;  
char rowcode[8] = {1,2,3,4,5,6,7,8};  //array for SPI row code 
char colcode[8] = {col0,col1,col2,col3,col4,col5,col6,col7};  //array for SPI column code
unsigned char y = 0;
int numblock = 0;  //variable for the number of blocks 
//initial array
int stacker[8][8] = 
{0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0};	   			 		  			 		       

/* Special ASCII characters */
#define CR 0x0D		// ASCII return?#define LF 0x0A		// ASCII new line?
/* LCD COMMUNICATION BIT MASKS (note - different than previous labs) */
#define RS 0x10		// RS pin mask (PTT[4])
#define RW 0x20		// R/W pin mask (PTT[5])
#define LCDCLK 0x40	// LCD EN/CLK pin mask (PTT[6])

/* LCD INSTRUCTION CHARACTERS */
#define LCDON 0x0F	// LCD initialization command
#define LCDCLR 0x01	// LCD clear display command
#define TWOLINE 0x38	// LCD 2-line enable command
#define CURMOV 0xFE	// LCD cursor move instruction
#define LINE1 = 0x80	// LCD line 1 cursor position
#define LINE2 = 0xC0	// LCD line 2 cursor position




int W[8][8] = {
0,1,1,0,0,1,1,0,
0,1,1,1,1,1,1,0,
0,1,0,1,1,0,1,0,
1,1,0,1,1,0,1,1,
1,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,1};

int I[8][8] = {
0,1,1,1,1,1,1,0,
0,0,0,1,1,0,0,0,
0,0,0,1,1,0,0,0,
0,0,0,1,1,0,0,0,
0,0,0,1,1,0,0,0,
0,0,0,1,1,0,0,0,
0,0,0,1,1,0,0,0,
0,1,1,1,1,1,1,0};

int N[8][8] = {
1,1,0,0,0,0,1,1,
1,1,0,0,0,0,1,1,
1,1,0,0,0,1,1,1,
1,1,0,0,1,1,1,1,
1,1,0,1,1,0,1,1,
1,1,1,1,0,0,1,1,
1,1,1,0,0,0,1,1,
1,1,0,0,0,0,1,1};

int LOSS[8][8] = {
0,1,0,0,0,0,1,0,
0,0,1,1,1,1,0,0,
0,0,0,0,0,0,0,0,
0,0,1,0,0,1,0,0,
0,0,1,0,0,1,0,0,
0,0,1,0,0,1,0,0,
0,0,1,0,0,1,0,0,
0,0,0,0,0,0,0,0};

int PAUSE[8][8] ={
0,1,1,0,0,1,1,0,
0,1,1,0,0,1,1,0,
0,1,1,0,0,1,1,0,
0,1,1,0,0,1,1,0,
0,1,1,0,0,1,1,0,
0,1,1,0,0,1,1,0,
0,1,1,0,0,1,1,0,
0,1,1,0,0,1,1,0
}; 
	 	   		
/*	 	   		
***********************************************************************
 Initializations
***********************************************************************
*/

void  initializations(void) {

/* Set the PLL speed (bus clock = 24 MHz) */
  CLKSEL = CLKSEL & 0x80; //; disengage PLL from system
  PLLCTL = PLLCTL | 0x40; //; turn on PLL
  SYNR = 0x02;            //; set PLL multiplier
  REFDV = 0;              //; set PLL divider
  while (!(CRGFLG & 0x08)){  }
  CLKSEL = CLKSEL | 0x80; //; engage PLL


/* Additional port pin initializations */
    DDRT = 0xFF; //port T for output mode
    DDRAD = 0; 		//program port AD for input mode
    DDRM = 0xFF; //port M outputs

/* Initialize digital I/O port pins */
    ATDDIEN = 0xC0; //program PAD7 and PAD6 pins as digital inputs

/* Disable watchdog timer (COPCTL register) */
   COPCTL = 0x40   ; //COP off; RTI and COP stopped in BDM-mode

/* Initialize asynchronous serial port (SCI) for 9600 baud, interrupts off initially */
  SCIBDH =  0x00; //set baud rate to 9600
  SCIBDL =  0x9C; //24,000,000 / 16 / 156 = 9600 (approx)  
  SCICR1 =  0x00; //$9C = 156
  SCICR2 =  0x0C; //initialize SCI for program-driven operation
  DDRB   =  0x10; //set PB4 for output mode
  PORTB  =  0x10; //assert DTR pin on COM port

/* Initialize peripherals */

 //ATD initilizations
    ATDCTL2 = 0x80; // power up the ADC and disable interrupts
    lcdwait();
    lcdwait();
    lcdwait();    // wait for ADC to warm up
    ATDCTL3 = 0x10;
    ATDCTL4 = 0x85;

 //PWM initilizations
    
    MODRR = 0x08;
    PWME = 0x08;
    PWMPOL = 0xFF;
    PWMCTL = 0x00;
    PWMCAE = 0x00;
    PWMDTY3 = 0x80;
    PWMCLK = 0x00; 
    PWMPRCLK = 0x00;

/* Initialize the SPI to 6 Mbs */
    SPIBR = 0x01;
    SPICR1 = 0x50; //MSB first, enabled, master mode
    SPICR2 = 0x00;

            
/* Initialize interrupts */

/* Initialize RTI for 2.048 ms interrupt rate */
    
    CRGINT_RTIE = 1;    //set rti enable bit
    RTICTL = 0x1F;      //2.048 ms delay for each RTI call

/* Initialize TIM Ch 7 (TC7) for periodic interrupts every 10 ms
     - enable timer subsystem
     - set channel 7 for output compare
     - set appropriate pre-scale factor and enable counter reset after OC7
     - set up channel 7 to generate 10 ms interrupt rate
     - initially disable TIM Ch 7 interrupts      
*/
    TIOS = 0x80;
    TSCR1 = 0x80;
    TSCR2 = 0x0C;
    TC7 = 15000;
    TIE = 0x80;

/* Initialize LED matrix */
   //shiftout(0x0F, 0x01);
   //update_display(&(stacker[0][0]), &(stacker[0][0]));
   shiftout(0x0B, 0x07); //scan all 8 digits
   shiftout(0x09, 0x00); //no decode
   shiftout(0x0C, 0x01); //no shutdown
   shiftout(0x0F, 0x00); //no test mode
   for (i=0; i < 8; i++){
      shiftout((char) i, 0x00);                           // turn all segments off
   }
   shiftout(0x0A, 0x0F); //highest intesity
   
    	      
}

	 		  			 		  		
/*	 		  			 		  		
***********************************************************************
Main
***********************************************************************
*/
void main(void) {
  	DisableInterrupts
	initializations(); 		  			 		  		
	EnableInterrupts;
 for(;;) {
  
/* < start of your main loop > */ 

	TC7 = 10000 - ((ATDCH0) * 32) - (row * 200); //TC7 can be from 15000 to 2250 (1000 ms to 150 ms) minus a factor of the current row * 200 (0-1400)

  /* if leftpb is pressed
	-clr leftpb  
	-toggle pause flag
	-toggle TIE
	*/

	if (leftpb == 1){
	    TIE = 0x00;
	    CRGINT_RTIE = 0;
	    leftpb = 0;
	    pause = toggle(pause);
	    TIE = 0x80;

		  if(pause == 1){
		    update_display(&(PAUSE[0][0]), &(PAUSE[0][0]));
		    TIE = 0x00;
		  } else TIE = 0x80;
      CRGINT_RTIE = 1;
	}
 
  /* if rghtpb is pressed
	-clr rghtpb 
	- if pause = 0
		win = check_win
		if win is a 0
			updatedisplay(LOSS)
		if win is a 1 and row is 7
			updatedisplay(WIN)			
		row++
		reset mid and win 
		TIE = 1
   */

	if (rghtpb == 1){
	    TIE = 0x00;
	    CRGINT_RTIE = 0;
	    rghtpb = 0;
	    if (pause == 0){
		    win = check_win();
		  if (win == 0){
		    TIE = 0x00;
		    CRGINT_RTIE = 0;
		    pause = 1; 
		    windisp();
		    }
		  if (win == 1 && row == 7) { 
		    TIE = 0x00;
		    CRGINT_RTIE = 0;
		    pause = 1;
		    windisp();
		    }

		    row++;
		    mid = 4;
		    win = 0;
		    direction = 1;
	    }
	    TIE = 0x80;
		  CRGINT_RTIE = 1;    
	}

   } /* loop forever */
   
}   /* do not leave main */




/*
***********************************************************************   ?????  ?
 RTI interrupt service routine: RTI_ISR
************************************************************************
*/

interrupt 7 void RTI_ISR(void)
{
    //leftpb is the pause function
    //rightpb is the game's button

    unsigned int c_leftpb = 0; 
    unsigned int c_rghtpb = 0;
    
    // clear RTI interrupt flag
    
    CRGFLG = CRGFLG | 0x80;
    
    
    //set pushbutton flags 
    c_leftpb = PTIAD_PTIAD7;
    c_rghtpb = PTIAD_PTIAD6;
    if (c_leftpb == 0 && p_leftpb == 1){
        leftpb = 1;
    }
    if (c_rghtpb == 0 && p_rghtpb == 1){
        rghtpb = 1;
    }
    
    p_leftpb = c_leftpb;
    p_rghtpb = c_rghtpb;
    p_2 = p_leftpb;

    //ATD for speed control

    ATDCTL5 = 0x90 ; // sets up ADC to perform a single conversion,
    // and store the results ADR0H - ADR3H.
    while((ATDSTAT0 & 0x80) != 0x80){} // Wait for conversion to finish
    
    ATDCH0 = ATDDR0; 

}

/*
***********************************************************************   ?????  ?
  TIM interrupt service routine	  		
***********************************************************************
*/

interrupt 15 void TIM_ISR(void)
{

  	// clear TIM CH 7 interrupt flag 
 	TFLG1 = TFLG1 | 0x80;

 hundred++;
 if (hundred == 100){
	
  hundred = 0;

	//clear row for change
	if(numblock == 1 || row > 5){
	  for (i = 0; i < 8; i++){
	  stacker[row][i] = 0;
	  }

	 //if the middle block is at the max value it can be on either side, reverse
	  if (mid == 7){
           direction = -1;
           hbeep();
	  } else if (mid == 0){
       	   direction = 1;
       	   hbeep();
	  }
	
	//update mid
	  mid = mid + direction;



	//set the values of the blocks in the array to 1
	  stacker[row][mid] = 1;


	//update display
	  update_display(&(stacker[0][0]), &(stacker[0][0]));	
	  
	} else if (numblock == 2 || row > 2){
		for (i = 0; i < 8; i++){
	  stacker[row][i] = 0;
	  }

	//if the middle block is at the max value it can be on either side, reverse
	  if (mid == 6){
           direction = -1;
           hbeep();           
	  } else if (mid == 0){
       	   direction = 1;
           hbeep();       	   
	  }
	
	//update mid and right based on the mid value
	  mid = mid + direction;
	  right = mid + 1;


	//set the values of the blocks in the array to 1
	  stacker[row][mid] = 1;
	  stacker[row][right] = 1;

	  //update display
	  update_display(&(stacker[0][0]), &(stacker[0][0]));	
	  
	} else {
	  for (i = 0; i < 8; i++){
	   stacker[row][i] = 0;
	  }

	//if the middle block is at the max value it can be on either side, reverse
	   if (mid == 6){
           direction = -1;
           hbeep();  //high beep noise          
	  } else if (mid == 1){
       	   direction = 1;
           hbeep(); //high beep noise      	   
	  }
	
	  //update mid, right, and left based on the mid value
	   mid = mid + direction;
	   right = mid + 1;
	   left = mid - 1;

	 //set the values of the blocks in the array to 1
	   stacker[row][mid] = 1;
	   stacker[row][left] = 1;
	   stacker[row][right] = 1;

	 //update display
	   update_display(&(stacker[0][0]), &(stacker[0][0]));
	 }
	}
}


/***********************************************************************
 Name:         toggle
 Description:  toggles a variable passed to it
***********************************************************************/
int toggle(int tog){
	if (tog == 1) tog = 0;
	else if (tog == 0) tog = 1;
	return tog;
}

/***********************************************************************
 Name:         windisp and lossdisp
 Description:  displays W-I-N or =( on LED until reset button is hit
***********************************************************************/
void windisp(void){
  for(;;){
   if (win == 0){
      update_display(&(LOSS[0][0]), &(LOSS[0][0]));
   }else{
    
      update_display(&(W[0][0]), &(W[0][0]));
      delay();
      update_display(&(I[0][0]), &(I[0][0]));
      delay();
      update_display(&(N[0][0]), &(N[0][0]));
      delay();
   }
  }
}


//delay functions

void delay()
{
    long cnt = 2147480 / 2;
    while(cnt != 0){
        cnt--;
    }
}

void delay1()
{
    long cnt = 214748;
    while(cnt != 0){
        cnt--;
    }
}

/***********************************************************************
 Name:         update_display
 Description:  recieves an 8x8 array and sends it to the LED matrix via SPI
***********************************************************************/
void update_display(int * r, int * c){

    for(i = 0; i < 8; i++){
        x = rowcode[7 - i];
        for(k = 0; k < 8; k++){
            if (*c == 1){
                y |= colcode[k];  
            }
            c++;
        }
        shiftout(x,y);
        x = 0;
        y = 0;
        r++;         
    } 
    

    
      
    
   


}
/***********************************************************************
  shiftout: Transmits the characters x and y to external shift 
            register using the SPI.  It should shift MSB first.
 ************************************************************************/
void shiftout(char x, char y)
{
    int cnt;
    // read the SPTEF bit, continue if bit is 1
    // write data to SPI data register
    // wait for 30 cycles for SPI data to shift out
    PTT_PTT7 = 1;    
    while(!SPISR_SPTEF){
    }
    SPIDR = x;
    for (cnt = 0; cnt < 30; cnt++);
    while(!SPISR_SPTEF){
    }
    SPIDR = y;
    for (cnt = 0; cnt < 30; cnt++);

    PTT_PTT7 = 0;
    PTT_PTT7 = 1;
    
    
}

/***********************************************************************
 Name:         check_win
 Description:  recieves an 8x8 array and check if the current row, and the row below match,
if there is one match, return 1, update so that current row = (current row & previous row)
if there is no match, return  0, update so that current row = (current row & previous row) 
***********************************************************************/
int check_win(void){
    numblock = 0;
    if(row == 0) win = 1;
    else{
	 for (i = 0; i < 8; i++){
	    if (stacker[row][i] && stacker[row - 1][i]){
			win++;
			numblock++;
		}
	    if (stacker[row][i] == 1 && stacker[row - 1][i] == 0){
	     stacker[row][i] = 0; 
	    }
	 }
        if (win != 0){    
		win = 1;
	    }
    }
	  update_display(&(stacker[0][0]), &(stacker[0][0]));
	  TIE = 0x00;
      lbeep();
	  TIE = 0x80;
      return win;
}


/*
 ***********************************************************************
 lcdwait: Delay for approx 2 ms
 ***********************************************************************
 */

void lcdwait()
{
    long cnt = 5600;
    while(cnt != 0){
        cnt--;
    }
}

//another delay function
void Delay0(int delayTime)							/*Short delay*/
{
  	int  x =0;									/*outer loop counter */
  	char y =0;									/*inner loop counter */

  	for (x=0; x<delayTime; x++)
  	{	
  		for (y=0; y<1; y++); 
	}
}


/*
 ***********************************************************************
 hbeep: create a high beep noise
  Code for regular beep used and modified from 
  [1]G. More, "Audio Reproduction on HCS12 Microcontrollers," in NXP, 2002. [Online]. Available: http://www.nxp.com/assets/documents/data/en/application-notes/AN2250.pdf?&fsrch=1&sr=1&pageNum=1. Accessed: Nov. 30, 2016.
 ***********************************************************************
 */
void hbeep(){

				for(x=0xFF; x>=0x01; x--) 		/*Loop controls length of beep*/
				{
					PWMPER3 = 0xFF; 	/*Set PWM period at maximum*/
											
					for (y=0x00; y<=0xFE; y++)	/*Fixed loop to generate up slope of sinusoid*/
					{					
						PWMDTY3 = y;	/*Loads PWM duty register*/
						//Delay0(0);				/*Optional delay. Higher value generates lower tone output*/
			 		}
  				  	
  			  		for (y=0xFF; y>=0x01; y--)	/*Fixed loop to generate down slope of sinusoid*/
  			  		{
  			  			PWMDTY3 = y;	/*Loads PWM duty register*/
  			  			//Delay0(0);				/*Optional delay. See above*/
  			  			
  			  		}	
				}
}

/*
 ***********************************************************************
 lbeep: create a low beep noise
 Code for regular beep used and modified from 
 [1]G. More, "Audio Reproduction on HCS12 Microcontrollers," in NXP, 2002. [Online]. Available: http://www.nxp.com/assets/documents/data/en/application-notes/AN2250.pdf?&fsrch=1&sr=1&pageNum=1. Accessed: Nov. 30, 2016.
 ***********************************************************************
 */
void lbeep(){


				for(x=0x0A; x>=0x01; x--) 		/*Loop controls length of beep*/
				{
					PWMPER3 = 0xFF; 	/*Set PWM period at maximum*/
											
					for (y=0x00; y<=0xFE; y++)	/*Fixed loop to generate up slope of sinusoid*/
					{					
						PWMDTY3 = y;	/*Loads PWM duty register*/
						Delay0(10);				/*Optional delay. Higher value generates lower tone output*/
			 		}
  				  	
  			  		for (y=0xFF; y>=0x01; y--)	/*Fixed loop to generate down slope of sinusoid*/
  			  		{
  			  			PWMDTY3 = y;	/*Loads PWM duty register*/
  			  			Delay0(10);				/*Optional delay. See above*/
  			  			
  			  		}	
				}}

/*
***********************************************************************
 Character I/O Library Routines for 9S12C32 
***********************************************************************
 Name:         inchar
 Description:  inputs ASCII character from SCI serial port and returns it
 Example:      char ch1 = inchar();
***********************************************************************
*/

char inchar(void) {
  /* receives character from the terminal channel */
        while (!(SCISR1 & 0x20)); /* wait for input */
    return SCIDRL;
}

/*
***********************************************************************
 Name:         outchar    (use only for DEBUGGING purposes)
 Description:  outputs ASCII character x to SCI serial port
 Example:      outchar('x');
***********************************************************************
*/

void outchar(char x) {
  /* sends a character to the terminal channel */
    while (!(SCISR1 & 0x80));  /* wait for output buffer empty */
    SCIDRL = x;
}