/****************************************************************************
 Module
   TemplateService.c

 Revision
   1.0.1

 Description
   This file implements TemplateService as a simple service under the
   Gen2 Events and Services Framework.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 25             ege      updated and cleaned
 01/16/12 09:58 jec      began conversion from TemplateFSM.c
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "PairedServoService.h"
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "PIC32_PWM_Lib.h"
#include "dbprintf.h"

/*----------------------------- Module Defines ----------------------------*/
// #define VERBOSE

#define PAIRED_PWM_CH 4
#define PAIRED_PWM_PIN PWM_RPB2
#define PAIRED_PWM_TIMER _Timer3_
#define PAIRED_PWM_SERVO_ANSEL (ANSELBbits.ANSB2) // RB2: digital output for servo PWM
#define PWM_SERVO_CENTER 375 // 1.5 ms pulse width at 20 ms period
#define PWM_SERVO_SIDE 250   // 1 ms
#define PWM_SERVO_OTHER 500  // 2 ms

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitPairedServoService

 Parameters
     uint8_t : the priorty of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, and does any
     other required initialization for this service
 Notes

 Author
     Ege Turan
****************************************************************************/
bool InitPairedServoService(uint8_t Priority)
{
    ES_Event_t ThisEvent;

    MyPriority = Priority;
    /********************************************
   in here you write your initialization code
   *******************************************/
    DB_printf("\rStarting PairedServoService: ");
    DB_printf("compiled at %s on %s", __TIME__, __DATE__);
    DB_printf("\n\r");
    // post the initial transition event
    ThisEvent.EventType = ES_INIT;
    if (ES_PostToService(MyPriority, ThisEvent) == true)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/****************************************************************************
 Function
     PostPairedServoService

 Parameters
     EF_Event_t ThisEvent ,the event to post to the queue

 Returns
     bool false if the Enqueue operation failed, true otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     Ege Turan
****************************************************************************/
bool PostPairedServoService(ES_Event_t ThisEvent)
{
    return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunPairedServoService

 Parameters
   ES_Event_t : the event to process

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   add your description here
 Notes

 Author
   Ege Turan
****************************************************************************/
ES_Event_t RunPairedServoService(ES_Event_t ThisEvent)
{
    ES_Event_t ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
    /********************************************
   in here you write your service code
   *******************************************/
  switch (ThisEvent.EventType)
    {
        // This event is run once at the end of service initialisation
        case ES_INIT:
        {
            DB_printf("\rES_INIT received in PairedServoService, priority: %d\r\n", MyPriority);
            
            PAIRED_PWM_SERVO_ANSEL = 0; // RB2: digital output for servo PWM
            // ----- Configure Timer3 for PWM at 50 Hz (20 ms period) -----
            // PBCLK = 20 MHz, Prescaler = 64 -> Period ticks = 20e6 / (64 * 50) = 6250
            PWM_Setup_ConfigureTimer(PAIRED_PWM_TIMER, 6250, PWM_PS_64);

            // Map PWM channel for servo
            PWM_Setup_SetChannel(PAIRED_PWM_CH);
            PWM_Setup_AssignChannelToTimer(PAIRED_PWM_CH, PAIRED_PWM_TIMER);
            PWM_Setup_MapChannelToOutputPin(PAIRED_PWM_CH, PAIRED_PWM_PIN);

            // Initialize servo to center
            PWM_Operate_SetPulseWidthOnChannel(PWM_SERVO_CENTER, PAIRED_PWM_CH);
        }
        break;

        case ES_PAIRED:
        {
            #ifdef VERBOSE
            DB_printf("\rReceived ES_PAIRED in PairedServoService\r\n");
            #endif
            PWM_Operate_SetPulseWidthOnChannel(PWM_SERVO_SIDE, PAIRED_PWM_CH);
        }
        break;

        case ES_UNPAIRED:
        {
            #ifdef VERBOSE
            DB_printf("\rReceived ES_UNPAIRED in PairedServoService\r\n");
            #endif
            PWM_Operate_SetPulseWidthOnChannel(PWM_SERVO_OTHER, PAIRED_PWM_CH);
        }
        break;

        default:
            break;
    }
    return ReturnEvent;
}

/***************************************************************************
 private functions
 ***************************************************************************/

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/
