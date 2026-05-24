/****************************************************************************
 Module
   BoatActionsService.c

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
#include "BoatActionsService.h"
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "PIC32_PWM_Lib.h"
#include "dbprintf.h"

/*----------------------------- Module Defines ----------------------------*/
#define GATE_PWM_CH 5
#define GATE_PWM_PIN PWM_RPA2
#define GATE_PWM_TIMER _Timer3_

#define CANNON_TRIS (TRISAbits.TRISA4)
#define CANNON_LAT (LATAbits.LATA4)
#define CANNON_ON 1
#define CANNON_OFF 0

// #define GATE_PWM_SERVO_ANSEL (ANSELBbits.ANSA2) // RB2: digital output for servo PWM
#define GATE_PWM_SERVO_OPEN 375  // 1.5 ms pulse width at 20 ms period
#define GATE_PWM_SERVO_CLOSE 250 // 1 ms
#define GATE_PWM_SERVO_OTHER 500 // 2 ms unused

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
     InitTemplateService

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
bool InitBoatActionsService(uint8_t Priority)
{
    ES_Event_t ThisEvent;
    MyPriority = Priority;

    DB_printf("\rStarting BoatActionsService: ");
    DB_printf("compiled at %s on %s", __TIME__, __DATE__);
    DB_printf("\n\r");

    // Initialise water cannon pins

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
     PostTemplateService

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
bool PostBoatActionsService(ES_Event_t ThisEvent)
{
    return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunTemplateService

 Parameters
   ES_Event_t : the event to process

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   add your description here
 Notes

 Author
   Nick Agathangelou
****************************************************************************/
ES_Event_t RunBoatActionsService(ES_Event_t ThisEvent)
{
    ES_Event_t ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

    switch (ThisEvent.EventType)
    {
        // This event is run once at the end of service initialisation
        case ES_INIT:
        {
            DB_printf("\rES_INIT received in BoatActionsService, priority: %d\r\n", MyPriority);

            // GATE_PWM_SERVO_ANSEL = 0; // RB2: digital output for servo PWM
            // ----- Configure Timer3 for PWM at 50 Hz (20 ms period) -----
            // PBCLK = 20 MHz, Prescaler = 64 -> Period ticks = 20e6 / (64 * 50) = 6250
            PWM_Setup_ConfigureTimer(GATE_PWM_TIMER, 6250, PWM_PS_64);

            // Map PWM channel for gate servo
            PWM_Setup_SetChannel(GATE_PWM_CH);
            PWM_Setup_AssignChannelToTimer(GATE_PWM_CH, GATE_PWM_TIMER);
            PWM_Setup_MapChannelToOutputPin(GATE_PWM_CH, GATE_PWM_PIN);

            // Initialize gate servo to center
            PWM_Operate_SetPulseWidthOnChannel(GATE_PWM_SERVO_OPEN, GATE_PWM_CH);

            // Initialise the pins for the water cannon (pin is digital only)
            CANNON_TRIS = 1; // Set as output

            // Start with the cannon off
            CANNON_LAT = 0;
        }
        break;

        case ES_GATE_OPEN:
        {
            // Set the pwm to make the gate open
            DB_printf("\rGate Opened, servo val: %d\r\n", GATE_PWM_SERVO_OPEN);
            PWM_Operate_SetPulseWidthOnChannel(GATE_PWM_SERVO_OPEN, GATE_PWM_CH);
        }
        break;

        case ES_GATE_CLOSE:
        {
            // Set the pwm to make the gate closed
            DB_printf("\rGate Closed, servo val: %d\r\n", GATE_PWM_SERVO_CLOSE);
            PWM_Operate_SetPulseWidthOnChannel(GATE_PWM_SERVO_CLOSE, GATE_PWM_CH);
        }
        break;

        case ES_CANNON_START:
        {
            DB_printf("\rCannon On!");
            CANNON_LAT = CANNON_ON;
        }
        break;

        case ES_CANNON_STOP:
        {
            DB_printf("\rCannon Off!");
            CANNON_LAT = CANNON_OFF;
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
