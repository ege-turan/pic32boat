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
#include "KeyboardService.h"
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "MallardCommunicationService.h"
#include "dbprintf.h"

/*----------------------------- Module Defines ----------------------------*/
#define VERBOSE_KEYBOARD
/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;

static ES_Event_t KB_Event1;

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
    /********************************************
   in here you write your initialization code
   *******************************************/
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
   Ege Turan
****************************************************************************/
ES_Event_t RunKeyboardService(ES_Event_t ThisEvent)
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
            DB_printf("\rES_INIT received in KeyboardService, priority: %d\r\n", MyPriority);
            KB_Event1.EventType = ES_NO_EVENT; // Initialize event to no event
        }
        break;

        case ES_NEW_KEY:
        {
            switch (ThisEvent.EventParam)
            {
                case '1':
                {
                    KB_Event1.EventType = ES_START_PAIRING;
                }
                break;
                case '2':
                {
                    KB_Event1.EventType  = ES_CHANGE_ADDR;
                    KB_Event1.EventParam = 0x02;
                }
                break;

                case 'w':
                {
                    KB_Event1.EventType  = ES_FUEL_VAL_RECEIVED;
                    KB_Event1.EventParam = 0;
                }
                break;

                case 's':
                {
                    KB_Event1.EventType  = ES_FUEL_VAL_RECEIVED;
                    KB_Event1.EventParam = 200;
                }
                break;

                default:
                    // KB_Event1.EventType  = ES_MOTORS_OFF;
                    DB_printf("KeyboardService msg: %c\r\n", ThisEvent.EventParam);
                    KB_Event1.EventType = ES_NO_EVENT; // Initialize event to no event
                    break;
            }
            ES_PostAll(KB_Event1);
            DB_printf("PostedEvent: %u, with param 0x%x\r\n",
                      (unsigned int)KB_Event1.EventType,
                      (unsigned int)KB_Event1.EventParam);
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
