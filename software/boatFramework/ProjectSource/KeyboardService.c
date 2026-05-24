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
 20/05/2026     nick     added to boat framework
 25             ege      updated and cleaned
 01/16/12 09:58 jec      began conversion from TemplateFSM.c
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "KeyboardService.h"
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "dbprintf.h"

/*----------------------------- Module Defines ----------------------------*/
#define VERBOSE_KEYBOARD
#define CANNON_LAT (LATAbits.LATA4)
#define CANNON_ON 1
#define CANNON_OFF 0
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
bool InitKeyboardService(uint8_t Priority)
{
    ES_Event_t ThisEvent;

    MyPriority = Priority;

    DB_printf("\rStarting KeyboardService: ");
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
bool PostKeyboardService(ES_Event_t ThisEvent)
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
ES_Event_t RunKeyboardService(ES_Event_t ThisEvent)
{
    ES_Event_t ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

    ES_Event_t KBEvent;

    switch (ThisEvent.EventType)
    {
        // This event is run once at the end of service initialisation
        case ES_INIT:
        {
            DB_printf("\rES_INIT received in KeyboardService, priority: %d\r\n", MyPriority);
        }
        break;

        case ES_NEW_KEY:
        {
            DB_printf("\rES_NEW_KEY received: %d\r\n", ES_NEW_KEY);
            switch (ThisEvent.EventParam)
            {
                // Full Left Turn.
                case 'a':
                {
                    DB_printf("Full Left Turn");

                    uint8_t BoatDirection = 0;   // Max power to turn left
                    uint8_t BoatThrottle  = 127; // No throttle forward or backwards

                    // // combine into parsable drive parameter
                    uint16_t DriveParam = (BoatDirection << 8) | BoatThrottle;

                    KBEvent.EventType  = ES_DRIVE;
                    KBEvent.EventParam = DriveParam;
                }
                break;

                // full right turn.
                case 'd':
                {
                    uint8_t BoatDirection = 255; // Max power to turn right
                    uint8_t BoatThrottle  = 127; // No throttle forward or backwards

                    // combine into parsable drive parameter
                    uint16_t DriveParam = (BoatDirection << 8) | BoatThrottle;

                    KBEvent.EventType  = ES_DRIVE;
                    KBEvent.EventParam = DriveParam;
                }
                break;

                // Full steam ahead
                case 'w':
                {
                    uint8_t BoatDirection = 127; // No direction throttle
                    uint8_t BoatThrottle  = 255; // Max Power forward

                    // combine into parsable drive parameter
                    uint16_t DriveParam = (BoatDirection << 8) | BoatThrottle;

                    KBEvent.EventType  = ES_DRIVE;
                    KBEvent.EventParam = DriveParam;
                }
                break;

                // Full reverse
                case 's':
                {
                    uint8_t BoatDirection = 127; // No direction throttle
                    uint8_t BoatThrottle  = 0;   // Max Power backwards

                    // combine into parsable drive parameter
                    uint16_t DriveParam = (BoatDirection << 8) | BoatThrottle;

                    KBEvent.EventType  = ES_DRIVE;
                    KBEvent.EventParam = DriveParam;
                }
                break;

                // Stop the motors
                case 'p':
                {
                    uint8_t BoatDirection = 127; // No direction throttle
                    uint8_t BoatThrottle  = 127; // No throttle forwards/backwards

                    // combine into parsable drive parameter
                    uint16_t DriveParam = (BoatDirection << 8) | BoatThrottle;

                    KBEvent.EventType  = ES_DRIVE;
                    KBEvent.EventParam = DriveParam;
                }
                break;

                // Toggle water cannon
                case 'c':
                {
                    // Read the state of the pin set last and set it to the opposite state.

                    uint8_t cannonLastSet = CANNON_LAT;

                    // If cannon is currently on, turn it off and if is is currently off, turn it on
                    // from the keypress. Doing it this way shouldn't interfere with comms service.
                    if (CANNON_ON == cannonLastSet)
                    {
                        KBEvent.EventType = ES_CANNON_STOP;
                    }
                    else if (CANNON_OFF == cannonLastSet)
                    {
                        KBEvent.EventType = ES_CANNON_START;
                    }
                    else
                    {
                        DB_printf("Something went wrong with the cannon.");
                    }

                    KBEvent.EventParam = 0; // unused
                }
                break;

                // Open the gate
                case 'g':
                {
                    KBEvent.EventType  = ES_GATE_OPEN;
                    KBEvent.EventParam = 0; // unused
                }
                break;

                // Close the gate
                case 'h':
                {
                    KBEvent.EventType  = ES_GATE_CLOSE;
                    KBEvent.EventParam = 0;
                }

                default:
                    // KB_Event1.EventType  = ES_MOTORS_OFF;
                    DB_printf("KeyboardService msg: %c\r\n", ThisEvent.EventParam);
                    break;
            }
            ES_PostAll(KBEvent);
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
