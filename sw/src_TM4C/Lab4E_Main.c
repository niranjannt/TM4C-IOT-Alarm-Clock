// -------------------------------------------------------------------
// File name:     Lab4E_Main.c
// Description:   This code is the main loop for the new MQTT Clock Control IOT Lab
//               
// Authors:       Mark McDermott
// Date:          June 6, 2023
//
// 

#include <stdio.h>
#include <stdint.h>

#include "inc/tm4c123gh6pm.h"
#include "inc/ST7735.h"
#include "inc/PLL.h"
#include "inc/Timer2A.h"
#include "inc/Timer5A.h"
#include "inc/UART.h"
#include "inc/UART5.h"
#include "inc/esp8266_base.h"
#include "inc/MQTT.h"
#include "inc/Unified_Port_Init.h"
#include "Lab4E_Main.h"

// ---------- Prototypes   -------------------------
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);
char volatile hours=1;
char volatile minutes=12;
char volatile seconds=59;
char volatile isPM=0;
char volatile timecounter;
char volatile timecycle;
char volatile digital;
char volatile erase=0;
char volatile freeze;
//char volatile set;
char volatile timecycleset;
char volatile alarmhours=1;
char volatile alarmminutes=12;
char volatile alarmseconds=59;
char volatile alarmisPM=0;
char volatile alarmsetdone=0;
char volatile alarm_display=0;
//char volatile alarmupdate;
char volatile alarmdisablecheck=1;
char volatile alarmsound=0;
char volatile tickflag=0;
char volatile stopfreezealarm, stopfreezereleasealarm=0;
char volatile changeclockruntworelease=0;
char volatile changeclockruntwo=0;
char volatile twentyfourhours=0;
static uint16_t digitalcolors[]={ST7735_BLUE, ST7735_RED, ST7735_GREEN, ST7735_YELLOW, ST7735_CYAN, ST7735_MAGENTA};
char volatile colorindex=0;
char volatile timesetdone=0;

void displayScreen(void);
void EnableInterrupts(void);  // Enable interrupts
void drawAnalogHands(void);
#include "Sound.h"
#include "Switch.h"
#include "LED.h"
#include "LCD.h"
#include "inc/SysTick.h"
#include "Timer1A.h"


//uint32_t         Mode_Value;      //
//uint32_t         Left_Value;      //
//uint32_t         Right_Value;     //
//uint32_t         Up_Value;        //
//uint32_t         Down_Value;      //


//----- Prototypes of functions in startup.s  ----------------------
//
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // Go into low power mode

// -----------------------------------------------------------------
// -------------------- MAIN LOOP ----------------------------------
//
int main(void){  
  DisableInterrupts();            // Disable interrupts until finished with inits     
  PLL_Init(Bus80MHz);             // Bus clock at 80 MHz
  UART_Init();                    // Allow us to talk to the PC via PuTTy!
  UART5_Init();                   // Enable ESP8266 Serial Port
  ST7735_InitR(INITR_REDTAB);     // Start up display.
  //Unified_Port_Init();						// Initialize the Ports used for this lab
	
	ST7735_OutString("Reseting ESP\n");
  Reset_8266();                   // Reset the WiFi module
  SetupWiFi();                    // Setup communications to MQTT Broker via 8266 WiFi
	Switch_Init();
	DAC5_Init();
  LED_Init();

	
  Timer2A_Init(&MQTT_to_TM4C, 400000, 7);         // Check/Get data from the ESP every 5ms 
  Timer5A_Init(&TM4C_to_MQTT, 80000000, 7);       // Send data back to MQTT Web App every second 
  
  //EnableInterrupts();
	
//	ST7735_SetCursor(0,10);
//	ST7735_OutString("Press SW to toggle mode:");

  //Integrate your lab 3 here
		    uint32_t cycle_release=0, cycle_press=0;
    uint32_t increment_cycle=0, increment_cycle_release=0;
	uint32_t timesetdone=0;
	uint32_t changeclock=0, changeclock_release, timeset, timesetlast, alarmset, alarmsetrelease, alarmdisable, alarmdisablerelease, setalarmrun, setalarmrundone =0;
	uint32_t changeclock_runrelease, changeclockrun=0;
	uint32_t stopfreeze, stopfreezerelease=0;
	uint32_t decrement_cycle, decrement_cycle_release=0;
	uint32_t colorchange, colorchangerelease=0;
uint32_t setalarmrundisable=128;

	
	

	

    while(timesetdone==0){
			    // set=1;
//	    if(timecycleset){		
//			switch(timecycle){
//				case 0: 	hours = (hours % 12) + 1;
//				case 1: minutes=(minutes+1)%60;
//				
//				
//			}
//		}
			
			colorchange=Switch_In6();
			if((colorchange==1) && (colorchangerelease==0)){
				
				colorindex=(colorindex+1)%6;
				colorchangerelease=colorchange;
				
				
			}
			
			else if((colorchangerelease=1) && (colorchange==0)){

        colorchangerelease=colorchange;




			}				
			
			//Debugged/Wrote Decrement Counter with ChatGPT
			   decrement_cycle=Switch_In4();
        if((decrement_cycle_release == 0) && (decrement_cycle==64)){
				if(timecycle==0){
            hours = ((hours - 2 + 12) % 12) + 1;
            decrement_cycle_release=decrement_cycle;
						}
						else if(timecycle==1){
							minutes = ((minutes - 1 + 60) % 60);
							decrement_cycle_release=decrement_cycle;

							
						}
						else if(timecycle==2){
					   seconds=((seconds-1+60)%60);
             decrement_cycle_release=decrement_cycle;

							
						}
						else if(timecycle==3){
				if(isPM){
              isPM=0;
          }
          else{
              isPM=1;
          }
          decrement_cycle_release=decrement_cycle;
      }
        }
        else if((decrement_cycle_release == 64) && (decrement_cycle==0)){
				 decrement_cycle_release = decrement_cycle;
	  
	
							
						}

			
          increment_cycle=Switch_In();
        if((increment_cycle_release == 0) && (increment_cycle==32)){
				if(timecycle==0){
            hours = (hours % 12) + 1;
            increment_cycle_release=increment_cycle;
						}
						else if(timecycle==1){
							minutes=(minutes+1)%60;
							increment_cycle_release=increment_cycle;

							
						}
						else if(timecycle==2){
					   seconds=(seconds+1)%60;
             increment_cycle_release=increment_cycle;

							
						}
						else if(timecycle==3){
				if(isPM){
              isPM=0;
          }
          else{
              isPM=1;
          }
          increment_cycle_release=increment_cycle;
      }
        }
        else if((increment_cycle_release == 32) && (increment_cycle==0)){
				 increment_cycle_release = increment_cycle;
	  
	
							
						}
        

       //displayScreen();


         cycle_press=Switch_In1();
       if((cycle_release == 0) && (cycle_press==16)){
				timecycle=(timecycle+1)%4;
       cycle_release = cycle_press;
       }
       else if((cycle_release == 16) && (cycle_press==0)){
				   cycle_release = cycle_press;

				 
       }
			 
			 
			 changeclock=Switch_In3();
			 if((changeclock == 64) && (changeclock_release == 0)){
				 
				 changeclock_release = changeclock;
				 
				 
			 }
			 else if((changeclock_release == 64) && (changeclock == 0)){
        digital=(digital+1)%3;
				 erase=1;
        changeclock_release = changeclock;


			 }				 
			 
			 
			 timeset=Switch_In5(); //Change to Switch_In3() as this is for testing
			if((timesetlast == 0) && (timeset==128)){
      timesetlast = timeset;
      }
      else if((timesetlast == 128) && (timeset==0)){
			timesetdone=1;
			timesetlast=timeset;


    }
			
		alarmset=Switch_In2(); //Change to Switch_In4() as this is for testing
		if((alarmsetrelease == 0) && (alarmset == 128) && (alarmsetdone==0)){
		
			alarmsetrelease = alarmset;
			
			
			
		}
else if((alarmset == 0) && (alarmsetrelease == 128) && (alarmsetdone==0)){
	//alarmsetrelease=alarmset;
		ST7735_FillScreen(ST7735_BLACK);
		ST7735_DrawCharS(27, 120, 'A', digitalcolors[colorindex], ST7735_BLACK, 2);
    ST7735_DrawCharS(47, 120, 'L', digitalcolors[colorindex], ST7735_BLACK, 2);
        ST7735_DrawCharS(67, 120, 'A', digitalcolors[colorindex], ST7735_BLACK, 2);
        ST7735_DrawCharS(87, 120, 'R', digitalcolors[colorindex], ST7735_BLACK, 2);
        ST7735_DrawCharS(107, 120, 'M', digitalcolors[colorindex], ST7735_BLACK, 2);
			  uint32_t alarmsetpressed, alarmsetreleased=0;
	      uint32_t alarmback, alarmbackreleased=0;
	      uint32_t decrement_cycle_release_alarm, decrement_cycle_alarm=0;
			  while(alarmsetdone==0){
				//tickflag=0;
				alarm_display=1;
				decrement_cycle_alarm=Switch_In4();
        if((decrement_cycle_release_alarm == 0) && (decrement_cycle_alarm==64)){
				if(timecycle==0){
            alarmhours = ((alarmhours - 2 + 12) % 12) + 1;
            decrement_cycle_release_alarm=decrement_cycle_alarm;
						}
						else if(timecycle==1){
							alarmminutes = ((alarmminutes - 1 + 60) % 60);
							decrement_cycle_release_alarm=decrement_cycle_alarm;

							
						}
						else if(timecycle==2){
					   alarmseconds=((alarmseconds-1+60)%60);
             decrement_cycle_release_alarm=decrement_cycle_alarm;

							
						}
						else if(timecycle==3){
				if(isPM){
              isPM=0;
          }
          else{
              isPM=1;
          }
          decrement_cycle_release_alarm=decrement_cycle_alarm;
      }
        }
        else if((decrement_cycle_release_alarm == 64) && (decrement_cycle_alarm==0)){
				 decrement_cycle_release_alarm = decrement_cycle_alarm;
	  
	
							
						}

			  increment_cycle=Switch_In();
        if((increment_cycle_release == 0) && (increment_cycle==32)){
				if(timecycle==0){
            alarmhours = (alarmhours % 12) + 1;
            increment_cycle_release=increment_cycle;
						}
						else if(timecycle==1){
							alarmminutes=(alarmminutes+1)%60;
							increment_cycle_release=increment_cycle;

							
						}
						else if(timecycle==2){
					   alarmseconds=(alarmseconds+1)%60;
             increment_cycle_release=increment_cycle;

							
						}
						else if(timecycle==3){
				if(alarmisPM){
              alarmisPM=0;
          }
          else{
              alarmisPM=1;
          }
          increment_cycle_release=increment_cycle;
      }
        }
        else if((increment_cycle_release == 32) && (increment_cycle==0)){
				 increment_cycle_release = increment_cycle;
	  
	
							
						}
        

       //displayScreen();


         cycle_press=Switch_In1();
       if((cycle_release == 0) && (cycle_press==16)){
				timecycle=(timecycle+1)%4;
       cycle_release = cycle_press;
       }
       else if((cycle_release == 16) && (cycle_press==0)){
				   cycle_release = cycle_press;

				 
       }
			 
			 alarmsetpressed=Switch_In2();
			 if((alarmsetpressed==128) && (alarmsetreleased==0)){
				 alarmsetreleased=alarmsetpressed;
				 
				 
			 }
			 else if((alarmsetpressed==0) && (alarmsetreleased==128)){
         
				 alarmsetreleased=alarmsetpressed;
				 alarmsetdone=1;
				 ST7735_FillScreen(ST7735_BLACK);
				 alarm_display=0;
				 //alarmupdate=1;
				 //alarmsound=0;
				 alarmdisablecheck=0;

					
				 
				 }
			 
				 
				 alarmback=Switch_In5();
			 if((alarmback==128) && (alarmbackreleased==0)){
//				 		ST7735_DrawCharS(27, 120, 'B', ST7735_YELLOW, ST7735_BLACK, 2);
//    ST7735_DrawCharS(47, 120, 'A', ST7735_YELLOW, ST7735_BLACK, 2);
//        ST7735_DrawCharS(67, 120, 'C', ST7735_YELLOW, ST7735_BLACK, 2);
//        ST7735_DrawCharS(87, 120, 'K', ST7735_YELLOW, ST7735_BLACK, 2);
//        ST7735_DrawCharS(107, 120, 'M', ST7735_YELLOW, ST7735_BLACK, 2);
				 alarmbackreleased=alarmback;
				 
				 
			 }
			 else if((alarmback==0) && (alarmbackreleased==128)){
				           
					 
				 
				 alarmbackreleased=alarmback;
				 alarmsetdone=1;
				 ST7735_FillScreen(ST7735_BLACK);
				 alarm_display=0;
				 alarmdisablecheck=1;
				// alarmupdate=0;
				// alarmsound=0;


				 
				 }
			 
			       displayScreen();

		 }
	
	
				alarmsetrelease = alarmset;

	
}
      
        




      displayScreen();



    }

    EnableInterrupts();
    SysTick_Init();
    Timer1A_Init(7272);


   while(1){
        displayScreen();
		 setalarmrundone=0;
		 //Disable alarm/set alarm
		 //Change color of text
		 //Change to military time
		 //Change to analog clock
		 
//		 		stopfreeze=Switch_In4();
//		if((stopfreeze==64) && (stopfreezerelease==0)){
//			stopfreezerelease=stopfreeze;
//			
//			

//}
//if((stopfreeze==0) && (stopfreezerelease==64)){
//	    freeze=0;
//	    alarmsound=0;
//			stopfreezerelease=stopfreeze;
//			
//			
	colorchange=Switch_In6();
			if((colorchange==1) && (colorchangerelease==0)){
				
				colorindex=(colorindex+1)%6;
				colorchangerelease=colorchange;
				
				
			}
			
			else if((colorchangerelease=1) && (colorchange==0)){

        colorchangerelease=colorchange;




			}				

		 alarmdisable=Switch_In4();
		 if((alarmdisable==64) && (alarmdisablerelease==0)){
			//alarmsound=0;
		 alarmdisablerelease=alarmdisable;
    }
	  else if((alarmdisablerelease==64)&(alarmdisable==0)){
			alarmdisablecheck=1;
			alarmdisablerelease=alarmdisable;
			
			
		}
		setalarmrun=Switch_In2();
		if((setalarmrun==0) && (setalarmrundisable==128) && (setalarmrundone==0)){
			setalarmrundisable=setalarmrun;
			
			
		}
		else if((setalarmrun==128) && (setalarmrundisable==0) && (setalarmrundone==0)){
			  setalarmrundisable=setalarmrun;
			//alarmupdate=0;
        		ST7735_FillScreen(ST7735_BLACK);
		ST7735_DrawCharS(27, 120, 'A', digitalcolors[colorindex], ST7735_BLACK, 2);
    ST7735_DrawCharS(47, 120, 'L', digitalcolors[colorindex], ST7735_BLACK, 2);
        ST7735_DrawCharS(67, 120, 'A', digitalcolors[colorindex], ST7735_BLACK, 2);
        ST7735_DrawCharS(87, 120, 'R', digitalcolors[colorindex], ST7735_BLACK, 2);
        ST7735_DrawCharS(107, 120, 'M', digitalcolors[colorindex], ST7735_BLACK, 2);
			  uint32_t alarmsetpressed=0;
			uint32_t alarmsetreleased=128;
			  uint32_t alarmback, alarmbackreleased=0;
			uint32_t decrement_cycle_run_release, decrement_cycle_run=0;
			  while(setalarmrundone==0){
					tickflag=0;
				alarm_display=1;
								decrement_cycle_run=Switch_In4();
        if((decrement_cycle_run_release == 0) && (decrement_cycle_run==64)){
				if(timecycle==0){
            alarmhours = (alarmhours - 2 + 12) % 12 + 1;
            decrement_cycle_run_release=decrement_cycle_run;
						}
						else if(timecycle==1){
							alarmminutes = (alarmminutes - 1 + 60) % 60;
							decrement_cycle_run_release=decrement_cycle_run;

							
						}
						else if(timecycle==2){
					   alarmseconds=(alarmseconds-1+60)%60;
             decrement_cycle_run_release=decrement_cycle_run;

							
						}
						else if(timecycle==3){
				if(isPM){
              isPM=0;
          }
          else{
              isPM=1;
          }
          decrement_cycle_run_release=decrement_cycle_run;
      }
        }
        else if((decrement_cycle_run_release == 64) && (decrement_cycle_run==0)){
				 decrement_cycle_run_release = decrement_cycle_run;
	  
	
							
						}

			  increment_cycle=Switch_In();
        if((increment_cycle_release == 0) && (increment_cycle==32)){
				if(timecycle==0){
            alarmhours = (alarmhours % 12) + 1;
            increment_cycle_release=increment_cycle;
						}
						else if(timecycle==1){
							alarmminutes=(alarmminutes+1)%60;
							increment_cycle_release=increment_cycle;

							
						}
						else if(timecycle==2){
					   alarmseconds=(alarmseconds+1)%60;
             increment_cycle_release=increment_cycle;

							
						}
						else if(timecycle==3){
				if(alarmisPM){
              alarmisPM=0;
          }
          else{
              alarmisPM=1;
          }
          increment_cycle_release=increment_cycle;
      }
        }
        else if((increment_cycle_release == 32) && (increment_cycle==0)){
				 increment_cycle_release = increment_cycle;
	  
	
							
						}
        

       //displayScreen();


         cycle_press=Switch_In1();
       if((cycle_release == 0) && (cycle_press==16)){
				timecycle=(timecycle+1)%4;
       cycle_release = cycle_press;
       }
       else if((cycle_release == 16) && (cycle_press==0)){
				   cycle_release = cycle_press;

				 
       }
			 
			 alarmsetpressed=Switch_In2(); //change to Switch_In5() for testing
			 if((alarmsetpressed==0) && (alarmsetreleased==128)){
				 alarmsetreleased=alarmsetpressed;
				 
				 
			 }
			 else if((alarmsetpressed==128) && (alarmsetreleased==0)){
				 alarmsetreleased=alarmsetpressed;
				 setalarmrundone=1;
				 ST7735_FillScreen(ST7735_BLACK);
				 alarm_display=0;
				 //alarmupdate=1;
				 //alarmsound=1;
				 alarmdisablecheck=0;


				 
				 }
			 alarmback=Switch_In5();
			 if((alarmback==128) && (alarmbackreleased==0)){
				 alarmbackreleased=alarmback;
				 
				 
			 }
			 else if((alarmback==0) && (alarmbackreleased==128)){
         
				 alarmbackreleased=alarmback;
				 setalarmrundone=1;
				 ST7735_FillScreen(ST7735_BLACK);
				 alarm_display=0;
				 alarmdisablecheck=1;
				 //alarmupdate=0;
				 //alarmsound=0;


				 
				 }
			 
			       displayScreen();

		 }

			//setalarmrundisable=setalarmrun;

			
			
		}
	 changeclockruntwo=Switch_In3();
			 if((changeclockruntwo == 64) && (changeclockruntworelease == 0)){
				 
				 changeclockruntworelease = changeclockruntwo;
				 
				 
			 }
			 else if((changeclockruntworelease == 64) && (changeclockruntwo == 0)){
        digital=(digital+1)%3;
				 erase=1;
        changeclockruntworelease = changeclockruntwo;


			 }					
		


	 }

  }



char volatile laststate=0; 
void displayScreen(){
	 //start=NVIC_ST_CURRENT_R;
	  while(freeze){};
	  if(erase){
			erase=0;
			ST7735_FillScreen(ST7735_BLACK);
			
		}
		//Alarm Disable Check debugged/written with ChatGPT
		if(!alarm_display){
		if(alarmdisablecheck==1){
			if(laststate==0){
	     ST7735_DrawBitmap(20, 140, coveralarm, 26, 23);
			laststate=1;
			
			
		}
	 ST7735_DrawBitmap(20, 140, noalarm, 26, 23);	

	}
		
		 if(alarmdisablecheck==0){
			 if(laststate==1){
		 ST7735_DrawBitmap(20, 140, coveralarm, 26, 23);
		 laststate=0;
	
			
			 }
		ST7735_DrawBitmap(20, 140, alarm, 26, 23);

		}
	}
	  if(digital==0){
    //ST7735_InvertDisplay(1); //Comment out for Adafruit
    char timearray[6];
		if(alarm_display){
		if(alarmhours>9){
      timearray[0]=alarmhours/10+'0';
      timearray[1]=alarmhours%10+'0';

    }
    else if(alarmhours<=9){
      timearray[0]='0';
      timearray[1]=alarmhours+'0';

    }

    if(alarmminutes>9){
      timearray[2]=alarmminutes/10+'0';
      timearray[3]=alarmminutes%10+'0';
    }

    else if(alarmminutes<=9){
      timearray[2]='0';
      timearray[3]=alarmminutes+'0';

    }
    if(alarmseconds>9){
      timearray[4]=alarmseconds/10+'0';
      timearray[5]=alarmseconds%10+'0';
    }
    else if(alarmseconds<=9){
     timearray[4]='0';
     timearray[5]=alarmseconds+'0';

    }






     ST7735_DrawCharS(19, 40, timearray[0], digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(37, 40, timearray[1], digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(55, 40, ':', digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(73, 40, timearray[2], digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(91, 40, timearray[3], digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(57, 70, ':', digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(75, 70, timearray[4], digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(93, 70, timearray[5], digitalcolors[colorindex], ST7735_BLACK, 3);



if(alarmisPM){
     ST7735_DrawCharS(75, 100, 'P', digitalcolors[colorindex], ST7735_BLACK, 2);
     ST7735_DrawCharS(91, 100, 'M', digitalcolors[colorindex], ST7735_BLACK, 2);
}
else{
    ST7735_DrawCharS(75, 100, 'A', digitalcolors[colorindex], ST7735_BLACK, 2);
    ST7735_DrawCharS(91, 100, 'M', digitalcolors[colorindex], ST7735_BLACK, 2);



}	
			
			
			
			
			
		}
		else{
    if(hours>9){
      timearray[0]=hours/10+'0';
      timearray[1]=hours%10+'0';

    }
    else if(hours<=9){
      timearray[0]='0';
      timearray[1]=hours+'0';

    }

    if(minutes>9){
      timearray[2]=minutes/10+'0';
      timearray[3]=minutes%10+'0';
    }

    else if(minutes<=9){
      timearray[2]='0';
      timearray[3]=minutes+'0';

    }
    if(seconds>9){
      timearray[4]=seconds/10+'0';
      timearray[5]=seconds%10+'0';
    }
    else if(seconds<=9){
     timearray[4]='0';
     timearray[5]=seconds+'0';

    }






     ST7735_DrawCharS(19, 40, timearray[0], digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(37, 40, timearray[1], digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(55, 40, ':', digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(73, 40, timearray[2], digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(91, 40, timearray[3], digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(57, 70, ':', digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(75, 70, timearray[4], digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(93, 70, timearray[5], digitalcolors[colorindex], ST7735_BLACK, 3);



if(isPM){
     ST7735_DrawCharS(75, 100, 'P', digitalcolors[colorindex], ST7735_BLACK, 2);
     ST7735_DrawCharS(91, 100, 'M', digitalcolors[colorindex], ST7735_BLACK, 2);
}
else{
    ST7735_DrawCharS(75, 100, 'A', digitalcolors[colorindex], ST7735_BLACK, 2);
    ST7735_DrawCharS(91, 100, 'M', digitalcolors[colorindex], ST7735_BLACK, 2);



}
}
		}
else if(digital==1){
        ST7735_DrawBitmap(25, 100, clock, 80, 80);
	           // ST7735_DrawLine(65, 60, 55, 55, ST7735_YELLOW); //10 hour hand
char prev_hours=60;
char prev_minutes=60;
//AI was used for debugging this switch statement
if(prev_hours!=hours){	
    switch(prev_hours){
        case 12: ST7735_DrawLine(65, 60, 65, 47, ST7735_BLACK); break;
        case 11: ST7735_DrawLine(65, 60, 55, 47, ST7735_BLACK); break;
        case 10: ST7735_DrawLine(65, 60, 55, 55, ST7735_BLACK); break;
        case 9:  ST7735_DrawLine(65, 60, 55, 60, ST7735_BLACK); break;
        case 8:  ST7735_DrawLine(65, 60, 55, 70, ST7735_BLACK); break;
        case 7:  ST7735_DrawLine(65, 60, 58, 73, ST7735_BLACK); break;
        case 6:  ST7735_DrawLine(65, 60, 65, 73, ST7735_BLACK); break;
        case 5:  ST7735_DrawLine(65, 60, 72, 72, ST7735_BLACK); break;
        case 4:  ST7735_DrawLine(65, 60, 75, 65, ST7735_BLACK); break;
        case 3:  ST7735_DrawLine(65, 60, 75, 60, ST7735_BLACK); break;
        case 2:  ST7735_DrawLine(65, 60, 75, 50, ST7735_BLACK); break;
        case 1:  ST7735_DrawLine(65, 60, 72, 47, ST7735_BLACK); break;
    }
	
    switch(hours){
        case 12: ST7735_DrawLine(65, 60, 65, 47, digitalcolors[colorindex]); break;
        case 11: ST7735_DrawLine(65, 60, 55, 47, digitalcolors[colorindex]); break;
        case 10: ST7735_DrawLine(65, 60, 55, 55, digitalcolors[colorindex]); break;
        case 9:  ST7735_DrawLine(65, 60, 55, 60, digitalcolors[colorindex]); break;
        case 8:  ST7735_DrawLine(65, 60, 55, 70, digitalcolors[colorindex]); break;
        case 7:  ST7735_DrawLine(65, 60, 58, 73, digitalcolors[colorindex]); break;
        case 6:  ST7735_DrawLine(65, 60, 65, 73, digitalcolors[colorindex]); break;
        case 5:  ST7735_DrawLine(65, 60, 72, 72, digitalcolors[colorindex]); break;
        case 4:  ST7735_DrawLine(65, 60, 75, 65, digitalcolors[colorindex]); break;
        case 3:  ST7735_DrawLine(65, 60, 75, 60, digitalcolors[colorindex]); break;
        case 2:  ST7735_DrawLine(65, 60, 75, 50, digitalcolors[colorindex]); break;
        case 1:  ST7735_DrawLine(65, 60, 72, 47, digitalcolors[colorindex]); break;
    }

		   // prev_hours = hours;  // update for next loop

	}
		    prev_hours = hours;  // update for next loop



	if(prev_minutes!=minutes){
		  switch(prev_minutes){
    case 59: ST7735_DrawLine(65, 60, 51, 37, ST7735_BLACK); //55 minute hand 
				break;
    case 58: ST7735_DrawLine(65, 60, 51, 37, ST7735_BLACK); //55 minute hand break; 
				break;
    case 57: ST7735_DrawLine(65, 60, 51, 37, ST7735_BLACK); //55 minute hand break;
				break;
    case 56: ST7735_DrawLine(65, 60, 51, 37, ST7735_BLACK); //55 minute hand break; 
				break;
    case 55: ST7735_DrawLine(65, 60, 51, 37, ST7735_BLACK); //55 minute hand break; 
				break;
    case 54: ST7735_DrawLine(65, 60, 36, 48, ST7735_BLACK); //50 minute hand
				break;
    case 53: ST7735_DrawLine(65, 60, 36, 48, ST7735_BLACK); //50 minute hand
				break;
    case 52: ST7735_DrawLine(65, 60, 36, 48, ST7735_BLACK); //50 minute hand
				break;
    case 51: ST7735_DrawLine(65, 60, 36, 48, ST7735_BLACK); //50 minute hand
				break;
    case 50: ST7735_DrawLine(65, 60, 36, 48, ST7735_BLACK); //50 minute hand
				break;
    case 49: 				ST7735_DrawLine(65, 60, 45, 60, ST7735_BLACK); //45 minute hand
break;
    case 48: 				ST7735_DrawLine(65, 60, 45, 60, ST7735_BLACK); //45 minute hand
 break; 
    case 47: 				ST7735_DrawLine(65, 60, 45, 60, ST7735_BLACK); //45 minute hand
 break;
    case 46: 				ST7735_DrawLine(65, 60, 45, 60, ST7735_BLACK); //45 minute hand
break;
    case 45: 				ST7735_DrawLine(65, 60, 45, 60, ST7735_BLACK); //45 minute hand
 break; 
    case 44: 						ST7735_DrawLine(65, 60, 37, 72, ST7735_BLACK); //40 minute hand
 break;
    case 43: 						ST7735_DrawLine(65, 60, 37, 72, ST7735_BLACK); //40 minute hand
 break;
    case 42: 						ST7735_DrawLine(65, 60, 37, 72, ST7735_BLACK); //40 minute hand
 break;
    case 41: 						ST7735_DrawLine(65, 60, 37, 72, ST7735_BLACK); //40 minute hand
 break;
    case 40: 						ST7735_DrawLine(65, 60, 37, 72, ST7735_BLACK); //40 minute hand
 break;
    case 39: 					ST7735_DrawLine(65, 60, 50, 83, ST7735_BLACK); //35 minute hand
 break;
    case 38: 					ST7735_DrawLine(65, 60, 50, 83, ST7735_BLACK); //35 minute hand
 break;
    case 37: 					ST7735_DrawLine(65, 60, 50, 83, ST7735_BLACK); //35 minute hand
 break;
    case 36: 					ST7735_DrawLine(65, 60, 50, 83, ST7735_BLACK); //35 minute hand
break;
    case 35: 					ST7735_DrawLine(65, 60, 50, 83, ST7735_BLACK); //35 minute hand
break;
    case 34: 		   ST7735_DrawLine(65, 60, 65, 80, ST7735_BLACK); //30 minute hand
 break;
    case 33: 		   ST7735_DrawLine(65, 60, 65, 80, ST7735_BLACK); //30 minute hand
 break;
    case 32: 		   ST7735_DrawLine(65, 60, 65, 80, ST7735_BLACK); //30 minute hand
 break;
    case 31: 		   ST7735_DrawLine(65, 60, 65, 80, ST7735_BLACK); //30 minute hand
 break;
    case 30: 		   ST7735_DrawLine(65, 60, 65, 80, ST7735_BLACK); //30 minute hand
 break; 
    case 29: 	    ST7735_DrawLine(65, 60, 76, 82, ST7735_BLACK); //25 minute hand
 break;
    case 28: 	    ST7735_DrawLine(65, 60, 76, 82, ST7735_BLACK); //25 minute hand
 break;
    case 27: 	    ST7735_DrawLine(65, 60, 76, 82, ST7735_BLACK); //25 minute hand
 break;
    case 26: 	    ST7735_DrawLine(65, 60, 76, 82, ST7735_BLACK); //25 minute hand
 break;
    case 25: 	    ST7735_DrawLine(65, 60, 76, 82, ST7735_BLACK); //25 minute hand
break;
    case 24:       ST7735_DrawLine(65, 60, 88, 75, ST7735_BLACK); //20 minute hand
 break;
    case 23:       ST7735_DrawLine(65, 60, 88, 75, ST7735_BLACK); //20 minute hand
 break;
    case 22:       ST7735_DrawLine(65, 60, 88, 75, ST7735_BLACK); //20 minute hand
 break;
    case 21:       ST7735_DrawLine(65, 60, 88, 75, ST7735_BLACK); //20 minute hand
 break;
    case 20:       ST7735_DrawLine(65, 60, 88, 75, ST7735_BLACK); //20 minute hand
 break;
    case 19:       ST7735_DrawLine(65, 60, 85, 60, ST7735_BLACK); //15 minute hand
 break;
    case 18:       ST7735_DrawLine(65, 60, 85, 60, ST7735_BLACK); //15 minute hand
 break;
    case 17:       ST7735_DrawLine(65, 60, 85, 60, ST7735_BLACK); //15 minute hand
 break;
    case 16:       ST7735_DrawLine(65, 60, 85, 60, ST7735_BLACK); //15 minute hand
 break;
    case 15:       ST7735_DrawLine(65, 60, 85, 60, ST7735_BLACK); //15 minute hand
 break;
    case 14: 			      ST7735_DrawLine(65, 60, 88, 47, ST7735_BLACK); //10 minute hand
 break;
    case 13: 			      ST7735_DrawLine(65, 60, 88, 47, ST7735_BLACK); //10 minute hand
 break;
    case 12: 			      ST7735_DrawLine(65, 60, 88, 47, ST7735_BLACK); //10 minute hand
 break;
    case 11: 			      ST7735_DrawLine(65, 60, 88, 47, ST7735_BLACK); //10 minute hand
 break;
    case 10: 			      ST7735_DrawLine(65, 60, 88, 47, ST7735_BLACK); //10 minute hand
 break;
    case 9:  	         ST7735_DrawLine(65, 60, 75, 37, ST7735_BLACK); //05 minute hand 
 break;
    case 8:  	         ST7735_DrawLine(65, 60, 75, 37, ST7735_BLACK); //05 minute hand 
 break;
    case 7:  	         ST7735_DrawLine(65, 60, 75, 37, ST7735_BLACK); //05 minute hand 
 break;
    case 6:  	         ST7735_DrawLine(65, 60, 75, 37, ST7735_BLACK); //05 minute hand 
 break;
    case 5:  	         ST7735_DrawLine(65, 60, 75, 37, ST7735_BLACK); //05 minute hand 
 break;
    case 4:  	       ST7735_DrawLine(65, 60, 65, 40, ST7735_BLACK); //0 minute hand
 break;
    case 3:  	       ST7735_DrawLine(65, 60, 65, 40, ST7735_BLACK); //0 minute hand
 break;
    case 2:  	       ST7735_DrawLine(65, 60, 65, 40, ST7735_BLACK); //0 minute hand
 break;
    case 1:  	       ST7735_DrawLine(65, 60, 65, 40, ST7735_BLACK); //0 minute hand
 break;
    case 0:  	       ST7735_DrawLine(65, 60, 65, 40, ST7735_BLACK); //0 minute hand
 break; 
}


		  switch(minutes){
    case 59: ST7735_DrawLine(65, 60, 51, 37, digitalcolors[colorindex]); //55 minute hand 
				break;
    case 58: ST7735_DrawLine(65, 60, 51, 37, digitalcolors[colorindex]); //55 minute hand break; 
				break;
    case 57: ST7735_DrawLine(65, 60, 51, 37, digitalcolors[colorindex]); //55 minute hand break;
				break;
    case 56: ST7735_DrawLine(65, 60, 51, 37, digitalcolors[colorindex]); //55 minute hand break; 
				break;
    case 55: ST7735_DrawLine(65, 60, 51, 37, digitalcolors[colorindex]); //55 minute hand break; 
				break;
    case 54: ST7735_DrawLine(65, 60, 36, 48, digitalcolors[colorindex]); //50 minute hand
				break;
    case 53: ST7735_DrawLine(65, 60, 36, 48, digitalcolors[colorindex]); //50 minute hand
				break;
    case 52: ST7735_DrawLine(65, 60, 36, 48, digitalcolors[colorindex]); //50 minute hand
				break;
    case 51: ST7735_DrawLine(65, 60, 36, 48, digitalcolors[colorindex]); //50 minute hand
				break;
    case 50: ST7735_DrawLine(65, 60, 36, 48, digitalcolors[colorindex]); //50 minute hand
				break;
    case 49: 				ST7735_DrawLine(65, 60, 45, 60, digitalcolors[colorindex]); //45 minute hand
break;
    case 48: 				ST7735_DrawLine(65, 60, 45, 60, digitalcolors[colorindex]); //45 minute hand
 break; 
    case 47: 				ST7735_DrawLine(65, 60, 45, 60, digitalcolors[colorindex]); //45 minute hand
 break;
    case 46: 				ST7735_DrawLine(65, 60, 45, 60, digitalcolors[colorindex]); //45 minute hand
break;
    case 45: 				ST7735_DrawLine(65, 60, 45, 60, digitalcolors[colorindex]); //45 minute hand
 break; 
    case 44: 						ST7735_DrawLine(65, 60, 37, 72, digitalcolors[colorindex]); //40 minute hand
 break;
    case 43: 						ST7735_DrawLine(65, 60, 37, 72, digitalcolors[colorindex]); //40 minute hand
 break;
    case 42: 						ST7735_DrawLine(65, 60, 37, 72, digitalcolors[colorindex]); //40 minute hand
 break;
    case 41: 						ST7735_DrawLine(65, 60, 37, 72, digitalcolors[colorindex]); //40 minute hand
 break;
    case 40: 						ST7735_DrawLine(65, 60, 37, 72, digitalcolors[colorindex]); //40 minute hand
 break;
    case 39: 					ST7735_DrawLine(65, 60, 50, 83, digitalcolors[colorindex]); //35 minute hand
 break;
    case 38: 					ST7735_DrawLine(65, 60, 50, 83, digitalcolors[colorindex]); //35 minute hand
 break;
    case 37: 					ST7735_DrawLine(65, 60, 50, 83, digitalcolors[colorindex]); //35 minute hand
 break;
    case 36: 					ST7735_DrawLine(65, 60, 50, 83, digitalcolors[colorindex]); //35 minute hand
break;
    case 35: 					ST7735_DrawLine(65, 60, 50, 83, digitalcolors[colorindex]); //35 minute hand
break;
    case 34: 		   ST7735_DrawLine(65, 60, 65, 80, digitalcolors[colorindex]); //30 minute hand
 break;
    case 33: 		   ST7735_DrawLine(65, 60, 65, 80, digitalcolors[colorindex]); //30 minute hand
 break;
    case 32: 		   ST7735_DrawLine(65, 60, 65, 80, digitalcolors[colorindex]); //30 minute hand
 break;
    case 31: 		   ST7735_DrawLine(65, 60, 65, 80, digitalcolors[colorindex]); //30 minute hand
 break;
    case 30: 		   ST7735_DrawLine(65, 60, 65, 80, digitalcolors[colorindex]); //30 minute hand
 break; 
    case 29: 	    ST7735_DrawLine(65, 60, 76, 82, digitalcolors[colorindex]); //25 minute hand
 break;
    case 28: 	    ST7735_DrawLine(65, 60, 76, 82, digitalcolors[colorindex]); //25 minute hand
 break;
    case 27: 	    ST7735_DrawLine(65, 60, 76, 82, digitalcolors[colorindex]); //25 minute hand
 break;
    case 26: 	    ST7735_DrawLine(65, 60, 76, 82, digitalcolors[colorindex]); //25 minute hand
 break;
    case 25: 	    ST7735_DrawLine(65, 60, 76, 82, digitalcolors[colorindex]); //25 minute hand
break;
    case 24:       ST7735_DrawLine(65, 60, 88, 75, digitalcolors[colorindex]); //20 minute hand
 break;
    case 23:       ST7735_DrawLine(65, 60, 88, 75, digitalcolors[colorindex]); //20 minute hand
 break;
    case 22:       ST7735_DrawLine(65, 60, 88, 75, digitalcolors[colorindex]); //20 minute hand
 break;
    case 21:       ST7735_DrawLine(65, 60, 88, 75, digitalcolors[colorindex]); //20 minute hand
 break;
    case 20:       ST7735_DrawLine(65, 60, 88, 75, digitalcolors[colorindex]); //20 minute hand
 break;
    case 19:       ST7735_DrawLine(65, 60, 85, 60, digitalcolors[colorindex]); //15 minute hand
 break;
    case 18:       ST7735_DrawLine(65, 60, 85, 60, digitalcolors[colorindex]); //15 minute hand
 break;
    case 17:       ST7735_DrawLine(65, 60, 85, 60, digitalcolors[colorindex]); //15 minute hand
 break;
    case 16:       ST7735_DrawLine(65, 60, 85, 60, digitalcolors[colorindex]); //15 minute hand
 break;
    case 15:       ST7735_DrawLine(65, 60, 85, 60, digitalcolors[colorindex]); //15 minute hand
 break;
    case 14: 			      ST7735_DrawLine(65, 60, 88, 47, digitalcolors[colorindex]); //10 minute hand
 break;
    case 13: 			      ST7735_DrawLine(65, 60, 88, 47, digitalcolors[colorindex]); //10 minute hand
 break;
    case 12: 			      ST7735_DrawLine(65, 60, 88, 47, digitalcolors[colorindex]); //10 minute hand
 break;
    case 11: 			      ST7735_DrawLine(65, 60, 88, 47, digitalcolors[colorindex]); //10 minute hand
 break;
    case 10: 			      ST7735_DrawLine(65, 60, 88, 47, digitalcolors[colorindex]); //10 minute hand
 break;
    case 9:  	         ST7735_DrawLine(65, 60, 75, 37, digitalcolors[colorindex]); //05 minute hand 
 break;
    case 8:  	         ST7735_DrawLine(65, 60, 75, 37, digitalcolors[colorindex]); //05 minute hand 
 break;
    case 7:  	         ST7735_DrawLine(65, 60, 75, 37, digitalcolors[colorindex]); //05 minute hand 
 break;
    case 6:  	         ST7735_DrawLine(65, 60, 75, 37, digitalcolors[colorindex]); //05 minute hand 
 break;
    case 5:  	         ST7735_DrawLine(65, 60, 75, 37, digitalcolors[colorindex]); //05 minute hand 
 break;
    case 4:  	       ST7735_DrawLine(65, 60, 65, 40, digitalcolors[colorindex]); //0 minute hand
 break;
    case 3:  	       ST7735_DrawLine(65, 60, 65, 40, digitalcolors[colorindex]); //0 minute hand
 break;
    case 2:  	       ST7735_DrawLine(65, 60, 65, 40, digitalcolors[colorindex]); //0 minute hand
 break;
    case 1:  	       ST7735_DrawLine(65, 60, 65, 40, digitalcolors[colorindex]); //0 minute hand
 break;
    case 0:  	       ST7735_DrawLine(65, 60, 65, 40, digitalcolors[colorindex]); //0 minute hand
 break; 
}
				//prev_minutes=minutes;

}
	prev_minutes=minutes;

}
else if(digital==2){
    //ST7735_InvertDisplay(1); //Comment out for Adafruit
    char timearray[6];
		if(alarm_display){
		if(alarmhours>9){
      timearray[0]=alarmhours/10+'0';
      timearray[1]=alarmhours%10+'0';

    }
    else if(alarmhours<=9){
      timearray[0]='0';
      timearray[1]=alarmhours+'0';

    }

    if(alarmminutes>9){
      timearray[2]=alarmminutes/10+'0';
      timearray[3]=alarmminutes%10+'0';
    }

    else if(alarmminutes<=9){
      timearray[2]='0';
      timearray[3]=alarmminutes+'0';

    }
    if(alarmseconds>9){
      timearray[4]=alarmseconds/10+'0';
      timearray[5]=alarmseconds%10+'0';
    }
    else if(alarmseconds<=9){
     timearray[4]='0';
     timearray[5]=alarmseconds+'0';

    }






     ST7735_DrawCharS(19, 40, timearray[0], digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(37, 40, timearray[1], digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(55, 40, ':', digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(73, 40, timearray[2], digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(91, 40, timearray[3], digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(57, 70, ':', digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(75, 70, timearray[4], digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(93, 70, timearray[5], digitalcolors[colorindex], ST7735_BLACK, 3);



if(alarmisPM){
     ST7735_DrawCharS(75, 100, 'P', digitalcolors[colorindex], ST7735_BLACK, 2);
     ST7735_DrawCharS(91, 100, 'M', digitalcolors[colorindex], ST7735_BLACK, 2);
}
else{
    ST7735_DrawCharS(75, 100, 'A', digitalcolors[colorindex], ST7735_BLACK, 2);
    ST7735_DrawCharS(91, 100, 'M', digitalcolors[colorindex], ST7735_BLACK, 2);



}	
			
			
			
			
			
		}
		else{
			
		if(isPM){
		    if(hours>9 && hours!=12){
			twentyfourhours=hours;
			twentyfourhours=twentyfourhours+12;
      timearray[0]=twentyfourhours/10+'0';
      timearray[1]=twentyfourhours%10+'0';

    }
    else if(hours<=9 && hours!=12){
			twentyfourhours=hours;
			twentyfourhours=twentyfourhours+12;
      timearray[0]=twentyfourhours/10+'0';
      timearray[1]=twentyfourhours%10+'0';

    }
    else if(hours==12){
			timearray[0]=hours/10+'0';
      timearray[1]=hours%10+'0';

			
			
		}
    if(minutes>9){
      timearray[2]=minutes/10+'0';
      timearray[3]=minutes%10+'0';
    }

    else if(minutes<=9){
      timearray[2]='0';
      timearray[3]=minutes+'0';

    }
    if(seconds>9){
      timearray[4]=seconds/10+'0';
      timearray[5]=seconds%10+'0';
    }
    else if(seconds<=9){
     timearray[4]='0';
     timearray[5]=seconds+'0';

    }






     ST7735_DrawCharS(19, 40, timearray[0], digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(37, 40, timearray[1], digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(55, 40, ':', digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(73, 40, timearray[2], digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(91, 40, timearray[3], digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(57, 70, ':', digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(75, 70, timearray[4], digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(93, 70, timearray[5], digitalcolors[colorindex], ST7735_BLACK, 3);

	
			
			
			
		}
		else{
    if(hours>9){
      timearray[0]=hours/10+'0';
      timearray[1]=hours%10+'0';

    }
    else if(hours<=9){
      timearray[0]='0';
      timearray[1]=hours+'0';

    }

    if(minutes>9){
      timearray[2]=minutes/10+'0';
      timearray[3]=minutes%10+'0';
    }

    else if(minutes<=9){
      timearray[2]='0';
      timearray[3]=minutes+'0';

    }
    if(seconds>9){
      timearray[4]=seconds/10+'0';
      timearray[5]=seconds%10+'0';
    }
    else if(seconds<=9){
     timearray[4]='0';
     timearray[5]=seconds+'0';

    }






     ST7735_DrawCharS(19, 40, timearray[0], digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(37, 40, timearray[1], digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(55, 40, ':', digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(73, 40, timearray[2], digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(91, 40, timearray[3], digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(57, 70, ':', digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(75, 70, timearray[4], digitalcolors[colorindex], ST7735_BLACK, 3);
     ST7735_DrawCharS(93, 70, timearray[5], digitalcolors[colorindex], ST7735_BLACK, 3);



	
	
	
}

		}


	}
}

void SysTick_Handler(void){
    timecounter++;
    if(timecounter>=5){
			if(!alarm_display){
        tickflag=1;
			}
			LED_Toggle();
       timecounter=0;
       if((hours==11)&&(minutes==59)&&(seconds==59)&&(isPM==1)){
           isPM=0;

       }
       else if((hours==11)&&(minutes==59)&&(seconds==59)&&(isPM==0)){
           isPM=1;

       }
       if(minutes==59 && seconds==59){
              hours = (hours % 12) + 1;
              minutes=0;
              seconds=0;
              }
       else if(seconds==59){
       minutes=(minutes+1)%60;
       seconds=0;
       }

       else{
   seconds++;
       }
				 
	if(alarmdisablecheck==0){
		if(hours==alarmhours && minutes==alarmminutes && seconds==alarmseconds && isPM==alarmisPM){
       alarmsound=1;
			 freeze=1;




		}				 
				 
				 
				 
				 
				 
			 }
			 
    }
	}









//void DAC4_Init(void){
//// Assumes LaunchPad_Init has been called
//// I.e., PortB has already been reset and activated (do not reset PortB here again)
//  // write this
// //Initialize PB0-PB4 as outputs
//    volatile uint32_t delay;
//      SYSCTL_RCGCGPIO_R |= 0x08;        // activate clock for Port E
//      delay = SYSCTL_RCGCGPIO_R;        // allow time for clock to start
//      GPIO_PORTD_DIR_R |= 0x01;         // set direction to output
//      GPIO_PORTD_DEN_R |= 0x01;         // enable digital port
//      GPIO_PORTD_DIR_R |= 0x02;         // set direction to output
//      GPIO_PORTD_DEN_R |= 0x02;         // enable digital port
//      GPIO_PORTD_DIR_R |= 0x04;         // set direction to output
//      GPIO_PORTD_DEN_R |= 0x04;         // enable digital port
//      GPIO_PORTD_DIR_R |= 0x08;         // set direction to output
//      GPIO_PORTD_DEN_R |= 0x08;


//// enable digital port

//}

void Timer1A_Handler(void){
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;  // acknowledge timeout
  // Do stuff here (e.g., output DAC waveform)
//  if(tickflag){
// static uint32_t orange=0;
// GPIO_PORTB_DATA_R = (GPIO_PORTB_DATA_R&(~31)|ticks2[orange]);   // output one value
//    orange++;
//     if(orange>=2113){
//        tickflag=0;
//        orange=0;
//     }
//	 }
//		Switch statement to stop alarm debugged with ChatGPT
//uint32_t 	stopfreeze, stopfreezerelease;
			if(alarmsound){
				tickflag=0;
	static uint32_t alarmsoundtick=0;
 GPIO_PORTB_DATA_R = (GPIO_PORTB_DATA_R&(~31)|bruh[alarmsoundtick]);   // output one value
    alarmsoundtick++;
     if(alarmsoundtick>=4374){
        //tickflag=0;
        alarmsoundtick=0;
     }
	 }
		if(tickflag){
				static uint32_t alarmsoundtick=0;
 GPIO_PORTB_DATA_R = (GPIO_PORTB_DATA_R&(~31)|squareclock[alarmsoundtick]);   // output one value
    alarmsoundtick++;
     if(alarmsoundtick>=14){
        tickflag=0;
        alarmsoundtick=0;
     }
	 }
			
		
  static uint32_t prevSwitch = 0;
  uint32_t currentSwitch = Switch_In4();

  if(currentSwitch == 0 && prevSwitch == 0){
		  alarmdisablecheck=1;
    alarmsound = 0;                 // Stop alarm
		freeze=0;
		tickflag=1;
  }
  prevSwitch = currentSwitch;
}



