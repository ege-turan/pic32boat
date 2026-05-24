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
#include "FuelServoService.h"
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "PWM_PIC32.h"
#include "dbprintf.h"

/*----------------------------- Module Defines ----------------------------*/
// #define SHOW_FUEL_RECEIVED

#define SERVO_MIN_PW   2500   // 1.0 ms / 0.4 µs = 2500 ticks
#define SERVO_MAX_PW   5000   // 2.0 ms / 0.4 µs = 5000 ticks
#define SERVO_PERIOD   50000  // 20 ms / 0.4 µs = 50,000 ticks
#define SERVO_CHANNEL  4      // Using PWM channel 4 (RPA2)
#define PWM_TIMER    _Timer2_
#define SERVO_UPDATE   10     // ms update timer of servos

#define MIN_FUEL_VAL 0
#define MAX_FUEL_VAL 200

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;
static uint16_t ServoPosition;
static uint16_t CurrentFuelVal;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitFuelServoService

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
bool InitFuelServoService(uint8_t Priority)
{
    ES_Event_t ThisEvent;

    MyPriority = Priority;
    /********************************************
   in here you write your initialization code
   *******************************************/
    DB_printf("\rStarting FuelServoService: ");
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
     PostFuelServoService

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
bool PostFuelServoService(ES_Event_t ThisEvent)
{
    return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunFuelServoService

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
ES_Event_t RunFuelServoService(ES_Event_t ThisEvent)
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
            DB_printf("\rES_INIT received in FuelServoService, priority: %d\r\n", MyPriority);
            // Initialize hardware for fuel servo here
            // Basic PWM setup
            PWMSetup_BasicConfig(4);
            PWMSetup_AssignChannelToTimer(SERVO_CHANNEL, PWM_TIMER);
            PWMSetup_SetPeriodOnTimer(SERVO_PERIOD, PWM_TIMER);
            DB_printf("\rFuelServoService initialization complete\r\n");

            // Map PWM channel 4 to RPA2/RA2
            PWMSetup_MapChannelToOutputPin(SERVO_CHANNEL, PWM_RPA2);

            // Start servo centered
            ServoPosition = (SERVO_MIN_PW + SERVO_MAX_PW) / 2;
            PWMOperate_SetPulseWidthOnChannel(ServoPosition, SERVO_CHANNEL);

            // Initialize variables
            CurrentFuelVal = MAX_FUEL_VAL;
        }
        break;

        case ES_FUEL_VAL_RECEIVED:
        {
            CurrentFuelVal = ThisEvent.EventParam;
            if (CurrentFuelVal > MAX_FUEL_VAL)
            {
                CurrentFuelVal = MAX_FUEL_VAL;
            }
            else if (CurrentFuelVal < MIN_FUEL_VAL)
            {
                CurrentFuelVal = MIN_FUEL_VAL;
            }
            // Map fuel value (0-100) to pulse width range
            uint16_t newPW = SERVO_MAX_PW - ((CurrentFuelVal * (SERVO_MAX_PW - SERVO_MIN_PW)) / (MAX_FUEL_VAL - MIN_FUEL_VAL));
            PWMOperate_SetPulseWidthOnChannel(newPW, SERVO_CHANNEL);  
            
            #ifdef SHOW_FUEL_RECEIVED
            DB_printf("\rFuel value received: %u, mapped pulse width: %u (min: %u, max: %u)\r\n", CurrentFuelVal, newPW, SERVO_MIN_PW, SERVO_MAX_PW);
            #endif
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
