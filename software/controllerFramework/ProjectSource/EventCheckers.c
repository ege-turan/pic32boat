/****************************************************************************
 Module
   EventCheckers.c

 Revision
   1.0.1

 Description
   This file implements event checkers for our application along with the event
   checkers used in the basic framework test harness.

 Notes
   Note the use of static variables in sample event checker to detect
   ONLY transitions.

 History
 When           Who     What/Why
 -------------- ---     --------
 25             ege     updated and cleaned
 08/06/13 13:36 jec     initial version
****************************************************************************/

// this will pull in the symbolic definitions for events, which we will want
// to post in response to detecting events
#include "ES_Configure.h"
// This gets us the prototype for ES_PostAll
#include "ES_Framework.h"
// this will get us the structure definition for events, which we will need
// in order to post events in response to detecting events
#include "ES_Events.h"
// if you want to use distribution lists then you need those function
// definitions too.
#include "ES_PostList.h"
// This include will pull in all of the headers from the service modules
// providing the prototypes for all of the post functions
#include "ES_ServiceHeaders.h"
// this test harness for the framework references the serial routines that
// are defined in ES_Port.c
#include "ES_Port.h"
// include our own prototypes to insure consistency between header &
// actual functionsdefinition
#include "EventCheckers.h"
#include "dbprintf.h"

// This is the event checking function sample. It is not intended to be
// included in the module. It is only here as a sample to guide you in writing
// your own event checkers
#if 0
/****************************************************************************
 Function
   Check4Lock
 Parameters
   None
 Returns
   bool: true if a new event was detected
 Description
   Sample event checker grabbed from the simple lock state machine example
 Notes
   will not compile, sample only
 Author
   J. Edward Carryer, 08/06/13, 13:48
****************************************************************************/
bool Check4Lock(void)
{
  static uint8_t  LastPinState = 0;
  uint8_t         CurrentPinState;
  bool            ReturnVal = false;

  CurrentPinState = LOCK_PIN;
  // check for pin high AND different from last time
  // do the check for difference first so that you don't bother with a test
  // of a port/variable that is not going to matter, since it hasn't changed
  if ((CurrentPinState != LastPinState) &&
      (CurrentPinState == LOCK_PIN_HI)) // event detected, so post detected event
  {
    ES_Event ThisEvent;
    ThisEvent.EventType   = ES_LOCK;
    ThisEvent.EventParam  = 1;
    // this could be any of the service post functions, ES_PostListx or
    // ES_PostAll functions
    ES_PostAll(ThisEvent);
    ReturnVal = true;
  }
  LastPinState = CurrentPinState; // update the state for next time

  return ReturnVal;
}

#endif

/****************************************************************************
 Function
   Check4Keystroke
 Parameters
   None
 Returns
   bool: true if a new key was detected & posted
 Description
   checks to see if a new key from the keyboard is detected and, if so,
   retrieves the key and posts an ES_NewKey event to TestHarnessService0
 Notes
   The functions that actually check the serial hardware for characters
   and retrieve them are assumed to be in ES_Port.c
   Since we always retrieve the keystroke when we detect it, thus clearing the
   hardware flag that indicates that a new key is ready this event checker
   will only generate events on the arrival of new characters, even though we
   do not internally keep track of the last keystroke that we retrieved.
 Author
   Ege Turan
****************************************************************************/
bool Check4Keystroke(void)
{
    if (IsNewKeyReady()) // new key waiting?
    {
        ES_Event_t ThisEvent;
        ThisEvent.EventType  = ES_NEW_KEY;
        ThisEvent.EventParam = GetNewKey();
        ES_PostAll(ThisEvent);
        return true;
    }
    return false;
}

/* ------------------------ CONTROLLER / MALLARD ------------------------ */

/* ---- Pair button (RB15, active LOW) ---------------------------------- */
#define PAIR_BTN_ANSEL (ANSELBbits.ANSB15) // RB15: digital input
#define PAIR_BTN_TRIS (TRISBbits.TRISB15)
#define PAIR_BTN_PORT (PORTBbits.RB15)
#define PAIR_BTN_CNPU (CNPUBbits.CNPUB15)     /* Internal pull-up        */
#define PAIR_BTN_PRESSED (PAIR_BTN_PORT == 0) /* Active LOW              */

/****************************************************************************
 Function   
    Check4PairButton
 Parameters 
    None
 Returns    
    bool — true if a pair event was detected and posted
 
 Description
   Polls RB15 for a falling edge (HIGH→LOW transition).
   On detection:
     1. Reads the boat-selector pot on RB2/AN4.
     2. Posts ES_CHANGE_ADDR with the selected boat index (1-5) to
        MallardCommunicationService so it updates desiredAddressLSB.
     3. Posts ES_START_PAIRING to MallardCommunicationService so it
        switches StatusVal to STATUS_PAIRING_BYTE and begins sending.
 
   The two posts are ordered intentionally: address must be updated before
   the first pairing message is built and transmitted.
****************************************************************************/
bool Check4PairButton(void)
{
    static bool initializedPairBtn = false;
    static bool lastButtonState    = true; // HIGH = unpressed (active LOW)
    /* One-time hardware setup (called on first event-checker invocation)  */
    if (!initializedPairBtn)
    {
        PAIR_BTN_ANSEL     = 0; // RB15: digital input
        PAIR_BTN_TRIS      = 1; /* RB15: digital input                      */
        PAIR_BTN_CNPU      = 1; /* Enable internal pull-up on RB15          */
        initializedPairBtn = true;
    }

    bool currentButtonState = (bool)PAIR_BTN_PORT; /* 1=high, 0=low    */

    /* Detect falling edge: was HIGH last tick, is LOW this tick           */
    if ((currentButtonState == false) && (lastButtonState == true))
    {
        DB_printf("\rPair button pressed! Current state is: %d\r\n", currentButtonState);
        lastButtonState = currentButtonState;

        ES_Event_t pairEvent;
        pairEvent.EventType  = ES_START_PAIRING;
        pairEvent.EventParam = 0;
        PostMallardCommunicationService(pairEvent);

        return true;
    }

    lastButtonState = currentButtonState;
    return false;
}

// RA3 or RB12 not sure which for refuel input
/* ---- Refuel Input (RB12, active LOW) ---------------------------------- */
#define REFUEL_IN_ANSEL (ANSELBbits.ANSB12)
#define REFUEL_IN_TRIS (TRISBbits.TRISB12)
#define REFUEL_IN_PORT (PORTBbits.RB12)
#define REFUEL_BTN_CNPU (CNPUBbits.CNPUB12) /* Internal pull-up        */

bool Check4RefuelInput(void)
{
    static uint8_t debounce = 0;

    if (debounce < 100)
    {
        debounce++;
        return false;
    }

    static bool initializedRefuelIn = false;
    static bool lastInputState      = true; // true = unpressed (active low)
    /* One-time hardware setup (called on first event-checker invocation)  */
    if (!initializedRefuelIn)
    {
        // Initialize refuel input hardware
        REFUEL_IN_ANSEL     = 0; // RB12: digital input
        REFUEL_IN_TRIS      = 1; // RB12: input
        REFUEL_BTN_CNPU     = 1; // Set internal pullup high
        initializedRefuelIn = false;
    }

    bool currentInputState = (bool)REFUEL_IN_PORT; /* 1=high, 0=low    */

    /* Detect falling edge: was HIGH last tick, is LOW this tick           */
    if ((currentInputState == false) && (lastInputState == true))
    {
        lastInputState = currentInputState;

        ES_Event_t refuelEvent;
        refuelEvent.EventType  = ES_REFUEL_INPUT;
        refuelEvent.EventParam = 0;
        ES_PostAll(refuelEvent);

        return true;
    }

    lastInputState = currentInputState;
    return false;
}

/* ---- Shoot button (RB13, active LOW) ---------------------------------- */
#define SHOOT_BTN_ANSEL (ANSELBbits.ANSB13) // RB13: digital input
#define SHOOT_BTN_TRIS (TRISBbits.TRISB13)
#define SHOOT_BTN_PORT (PORTBbits.RB13)
#define SHOOT_BTN_CNPU (CNPUBbits.CNPUB13)      /* Internal pull-up        */
#define SHOOT_BTN_PRESSED (SHOOT_BTN_PORT == 0) /* Active LOW              */

bool Check4ShootButton(void)
{
    static bool initializedShootBtn = false;
    static bool lastButtonState     = true; // HIGH = unpressed (active LOW)
    /* One-time hardware setup (called on first event-checker invocation)  */
    if (!initializedShootBtn)
    {
        SHOOT_BTN_ANSEL     = 0; // RB13: digital input
        SHOOT_BTN_TRIS      = 1; /* RB13: digital input                      */
        SHOOT_BTN_CNPU      = 1; /* Enable internal pull-up on RB13          */
        initializedShootBtn = true;
    }

    bool currentButtonState = (bool)SHOOT_BTN_PORT; /* 1=high, 0=low    */

    /* Detect falling edge: was HIGH last tick, is LOW this tick           */
    if ((currentButtonState == false) && (lastButtonState == true))
    {
        DB_printf("\rShoot button pressed! Current state is: %d\r\n", currentButtonState);
        lastButtonState = currentButtonState;

        ES_Event_t shootEvent;
        shootEvent.EventType  = ES_SHOOT;
        shootEvent.EventParam = 0;
        PostMallardCommunicationService(shootEvent);

        return true;
    }

    lastButtonState = currentButtonState;
    return false;
}
