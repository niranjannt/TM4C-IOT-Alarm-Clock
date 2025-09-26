// ----------------------------------------------------------------------------
//
// File name: MQTT.c
//
// Description: This code is used to bridge the TM4C123 board and the MQTT Web Application
//              via the ESP8266 WiFi board

// Authors:       Mark McDermott
// Orig gen date: June 3, 2023
// Last update:   July 21, 2023
//
// ----------------------------------------------------------------------------


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "PLL.h"

#include "UART.h"
#include "UART5.h"
#include "esp8266.h"
#include "FIFO3.h"
#include "globals.h"   // relative path

#define DEBUG1                // First level of Debug
#undef  DEBUG1                // Comment out to enable Debug1

#define UART5_FR_TXFF            0x00000020  // UART Transmit FIFO Full
#define UART5_FR_RXFE            0x00000010  // UART Receive FIFO Empty
#define UART5_LCRH_WLEN_8        0x00000060  // 8 bit word length
#define UART5_LCRH_FEN           0x00000010  // UART Enable FIFOs
#define UART5_CTL_UARTEN         0x00000001  // UART Enable

// ----   Function Prototypes not defined in header files  ------
// 
void EnableInterrupts(void);    // Defined in startup.s
void FillTransmitBuffer(void);
// ----------   VARIABLES  ----------------------------
void ReceiveTransmitBuffer(void);

//extern uint32_t         Kp1; //Extern for any variables we need to set/get for MQTT code
//extern uint32_t         Kp2;
//extern uint32_t         Ki1;
//extern uint32_t         Ki2;
//extern uint32_t         Xstar;
uint32_t example_left_motor_speed;

//Buffers for send / recv
char                    input_char;
char                    b2w_buf[64];

char                    w2b_buf[128];
static uint32_t         w2bbufget         = 0;
static uint32_t         bufpos         = 0;

//static uint32_t         bufpos          = 0;

char b2w_message[25];


void ReceiveTransmitBuffer(void){
char message=Fifo1_Get();
if(message!=','){	
	switch(message){
		//Analog/Digital/24 Hour Toggle
		case 0:     digital=0; break;
		case 1:     digital=1; break;
		case 2:     digital=2; break;	
		case 3:     hours++; break; // Increment Hours
		case 4:     minutes++; break;         //Increment Minutes
		case 5:     seconds++; break;         //Increment Seconds
		case 6:	    hours--; break;               //Decrement Hours
		case 7:     minutes--; break; //Decrement Minutes
		case 8:     seconds--; break;              //Decrement Seconds
		case 9:     alarmhours++; break;                //Increment Alarm Hours
		case 10:    alarmminutes++; break;                 //Increment Alarm Minutes
		case 11:    alarmseconds++; break;                     //Increment Alarm Seconds
		case 12:    alarmsetdone=1; break;                     //Alarm Time is Set
		case 13:    timesetdone=1; break;                //Initial Time is Set
		case 14:     isPM=1; break;         //isPM
		case 15:     colorindex=(colorindex+1)%6; break;         //colorindex
	
}
	}
}

void FillTransmitBuffer(void){
Fifo2_Put(digital);
Fifo2_Put(hours);
Fifo2_Put(minutes);
Fifo2_Put(seconds);
Fifo2_Put(isPM);
Fifo2_Put(colorindex);
Fifo2_Put(alarmsetdone);
Fifo2_Put(alarmhours);
Fifo2_Put(alarmminutes);
Fifo2_Put(alarmseconds);
Fifo2_Put(alarmisPM);
Fifo2_Put(timesetdone);

	
	
}












// --------------------------     W2B Parser      ------------------------------
//
// This parser decodes and executes commands from the MQTT Web Appication 
//
void Parser(void) {
  //Perform operations on the w2b buffer to set variables as appropriate
	//atoi() and strtok() may be useful
	
	char first_token[8];
	strcpy(first_token, strtok(w2b_buf, ","));
	
	#ifdef DEBUG1
		UART_OutString("Token 1: ");
		UART_OutString(first_token);
		UART_OutString("\r\n");
	#endif
}
  
// -----------------------  TM4C_to_MQTT Web App -----------------------------
// This routine publishes clock data to the
// MQTT Web Application via the MQTT Broker
// The data is sent using CSV format:
//
// ----------------------------------------------------------------------------
//    
//    Convert this routine to use a FIFO
//
// 
void TM4C_to_MQTT(void){
	//Fifo2_Init();
	//Fifo1_Init();
  //TODO: Add other commands here
  //sprintf(b2w_buf, "%d,\n", example_left_motor_speed);  // Msg is comma seperated, and must end with '\n'          
  
	
	char mode=Fifo2_Get();
	char hours=Fifo2_Get();
	char minutes=Fifo2_Get();
	char seconds=Fifo2_Get();
	char isPM=Fifo2_Get();
	char style=Fifo2_Get();
	char alarmset=Fifo2_Get();
	char alarmhours=Fifo2_Get();
	char alarmminutes=Fifo2_Get();
	char alarmseconds=Fifo2_Get();
	char alarmisPM=Fifo2_Get();
	char timesetdone=Fifo2_Get();
	sprintf(b2w_message, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", mode, hours, minutes, seconds, isPM, style, alarmset, alarmhours, alarmminutes, alarmseconds, alarmisPM, timesetdone);
  UART5_OutString(b2w_message);      

  #ifdef DEBUG1
   UART_OutString("B2W: ");
   UART_OutString(b2w_buf);         
   UART_OutString("\r\n"); 
  #endif 
}
// 
// -------------------------   MQTT_to_TM4C  -----------------------------------
// This routine receives the command data from the MQTT Web App and parses the
// data and feeds the commands to the TM4C.
// -----------------------------------------------------------------------------

//    Convert this routine to use a FIFO

 
void MQTT_to_TM4C(void) {
    
  if ((UART5_FR_R & UART5_FR_RXFE) ==0 ) {  // Check to see if a there is data in the RXD buffer
		
			input_char =(UART5_DR_R & 0xFF);      // Read the data from the UART
        
      if (input_char != '\n'){              // If the incoming character is not a newline then process byte             
        
		Fifo1_Put(input_char);
                        // again until the end of package marker is read.
        #ifdef DEBUG1
					UART_OutChar(input_char);         // Debug only
				#endif       
      } else if (input_char == '\n'){
				
//        w2b_buf[bufpos] = ',';           // Add a trailing comma
//        bufpos++;       								 // Update bufpos
				Fifo1_Put(',');
        if (Fifo1_isEmpty()==1) {
					Parser();                      // Call the parser if new data received
        }
        bufpos = 0;                      // Reset for next string	
      }   
  } 
  
}// End of routine

//void MQTT_to_TM4C(void){
//if(Fifo1_isEmpty()==0){	
//	input_char = (UART5_DR_R & 0xFF);
//	if(input_char != '\n'){
//		Fifo1_Put(input_char);
//	}
//	else if(input_char == '\n'){
//	Fifo1_Put(',');
//  if(w2bbufput > 1){
//  Parser();
//		
//		
//		
//	
//}
//}
//}
//}
// FIFO1.c
// Runs on any microcontroller
// Provide functions that implement the Software FiFo Buffer
// Last Modified: 10/29/2023
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly
#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
#include <stdio.h>

// Declare state variables for FiFo
//        size, buffer, put and get indexes

//int32_t static FIF0[8];
//uint32_t static FIFOget;
//uint32_t static FIFOput;



//char                    w2b_buf[128];


// *********** Fifo1_Init**********
// Initializes a software FIFO1 of a
// fixed size and sets up indexes for
// put and get operations
void Fifo1_Init(){
//Complete this
    w2bbufget=bufpos=0;
    
}


uint32_t Fifo1_isEmpty(){
	if(w2bbufget==bufpos){
	return 1;
	}
	else{
		
		return 0;
		
	}
	
}

// *********** Fifo1_Put**********
// Adds an element to the FIFO1
// Input: data is character to be inserted
// Output: 1 for success, data properly saved
//         0 for failure, FIFO1 is full
uint32_t Fifo1_Put(char data){
  //Complete this routine
    if( ((bufpos+1)&(128-1))==w2bbufget){
        return 0;
    }
    w2b_buf[bufpos]=data;
    bufpos=(bufpos+1)&(128-1);
    return 1;
    }




// *********** Fifo1_Get**********
// Gets an element from the FIFO1
// Input: none
// Output: If the FIFO1 is empty return 0
//         If the FIFO1 has data, remove it, and return it
char Fifo1_Get(void){
char data=0;



if(w2bbufget==bufpos){
    return 0;
}

// replace this line with your solution
data=w2b_buf[w2bbufget];
w2bbufget= (w2bbufget+1)&(128-1);
return data;
}


#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
#include <stdio.h>

static  uint32_t  b2w_bufget=0;
static  uint32_t  b2w_bufput=0;
//char                    b2w_buf[64];


void Fifo2_Init(){
//Complete this
    b2w_bufget=b2w_bufput=0;

}

uint32_t Fifo2_isEmpty(){
	if(b2w_bufget==b2w_bufput){
	return 1;
	}
	else{
		
		return 0;
		
	}
	
}
// *********** Fifo1_Put**********
// Adds an element to the FIFO1
// Input: data is character to be inserted
// Output: 1 for success, data properly saved
//         0 for failure, FIFO1 is full
uint32_t Fifo2_Put(char data){
  //Complete this routine
    if( ((b2w_bufput+1)&(64-1))==b2w_bufget){
        return 0;
    }
    b2w_buf[b2w_bufput]=data;
    b2w_bufput=(b2w_bufput+1)&(64-1);
    return 1;
    }




// *********** Fifo1_Get**********
// Gets an element from the FIFO1
// Input: none
// Output: If the FIFO1 is empty return 0
//         If the FIFO1 has data, remove it, and return it
char Fifo2_Get(void){
char data=0;



if(b2w_bufget==b2w_bufput){
    return 0;
}

// replace this line with your solution
data=b2w_buf[b2w_bufget];
b2w_bufget= (b2w_bufget+1)&(64-1);
return data;
}
