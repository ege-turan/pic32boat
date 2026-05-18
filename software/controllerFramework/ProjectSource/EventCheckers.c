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
#define PAIR_BTN_TRIS    (TRISBbits.TRISB15)
#define PAIR_BTN_PORT    (PORTBbits.RB15)
#define PAIR_BTN_CNPU    (CNPUBbits.CNPUB15)  /* Internal pull-up        */
#define PAIR_BTN_PRESSED (PAIR_BTN_PORT == 0)  /* Active LOW              */
 
/* ---- Boat selector pot (RB2 / AN4) ----------------------------------- */
/* RB2 is AN4 on the PIC32MX170F256B.                                     */
#define POT_TRIS         (TRISBbits.TRISB2)
#define POT_ANSEL        (ANSELBbits.ANSB2)
#define POT_ADC_MASK     BIT4HI    /* AN4 bit mask for ADC_ConfigAutoScan */
 
/* ---- Boat address thresholds (pot 0-1023, 5 equal bands) ------------- */
/* Each band is 1024/5 ≈ 205 counts wide.                                 */
#define BOAT_THRESH_1    205u   /* 0    – 204  → Boat 1                   */
#define BOAT_THRESH_2    409u   /* 205  – 408  → Boat 2                   */
#define BOAT_THRESH_3    614u   /* 409  – 613  → Boat 3                   */
#define BOAT_THRESH_4    819u   /* 614  – 818  → Boat 4                   */
                                /* 819  – 1023 → Boat 5                   */
 
/* Number of boat addresses (matches Addresses[] in MallardCommService)   */
/* Index 0 is unused (0x00), indices 1-5 are boats 1-5.                   */
#define BOAT_INDEX_MIN   1u
#define BOAT_INDEX_MAX   5u
 
/*------------------------ Module Variables ------------------------------*/
static bool lastButtonState = true;   /* HIGH = not pressed (unpressed)   */
static bool potInitialized  = false;  /* Track whether pot ADC is set up  */

/*----------------------- Private Prototypes ----------------------------*/
static void     InitPotADC(void);
static uint8_t  ReadBoatIndex(void);

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
    /* One-time hardware setup (called on first event-checker invocation)  */
    if (!potInitialized)
    {
        PAIR_BTN_TRIS = 1;    /* RB15: digital input                      */
        PAIR_BTN_CNPU = 1;    /* Enable internal pull-up on RB15          */
        InitPotADC();
        potInitialized = true;
    }
 
    bool currentButtonState = (bool)PAIR_BTN_PORT;   /* 1=high, 0=low    */
 
    /* Detect falling edge: was HIGH last tick, is LOW this tick           */
    if ((currentButtonState == false) && (lastButtonState == true))
    {
        lastButtonState = currentButtonState;
 
        /* -- Read pot to determine which boat to target ------------------ */
        uint8_t boatIndex = ReadBoatIndex();
        DB_printf("[PairBtn] Pressed. Pot → Boat index %u\r\n", boatIndex);
 
        /* -- 1. Update target address ------------------------------------ */
        ES_Event_t addrEvent;
        addrEvent.EventType  = ES_CHANGE_ADDR;
        addrEvent.EventParam = (uint16_t)boatIndex;
        PostMallardCommunicationService(addrEvent);
 
        /* -- 2. Begin pairing ------------------------------------------- */
        ES_Event_t pairEvent;
        pairEvent.EventType  = ES_START_PAIRING;
        pairEvent.EventParam = 0;
        PostMallardCommunicationService(pairEvent);
 
        return true;
    }
 
    lastButtonState = currentButtonState;
    return false;
}

/*--------------------------- Private Functions --------------------------*/
 
/****************************************************************************
 Function   InitPotADC
 Description
   Configures RB2/AN4 as an analog input for the boat-selector pot.
   Uses the same ADC_ConfigAutoScan pattern already in use for the
   joystick channels (AN0, AN1) in MallardCommunicationService.
 
   IMPORTANT: Call ADC_ConfigAutoScan with the combined mask of ALL analog
   inputs your project uses (joystick AN0/AN1 + pot AN4). If you call it
   here with only AN4, it will override the joystick scan config.
   The safest approach is to add BIT4HI to the mask in
   MallardCommunicationService's ES_INIT and leave this function as a
   pin-only setup.
****************************************************************************/
static void InitPotADC(void)
{
    POT_ANSEL = 1;   /* RB2: analog input (AN4)                           */
    POT_TRIS  = 1;   /* RB2: input direction                              */
 
    /*
     * NOTE: Do NOT call ADC_ConfigAutoScan here — it resets the scan mask
     * and would break the joystick ADC already configured in ES_INIT.
     *
     * Instead, update the ADC_ConfigAutoScan call in
     * MallardCommunicationService InitMallardCommunicationService() to:
     *
     *   ADC_ConfigAutoScan(JOY1_ADC_MASK | JOY2_ADC_MASK | BIT4HI);
     *
     * And update ReadADCValues() to read ADCResults[2] for the pot,
     * since AN4 will appear as the third result in the scan.
     *
     * We only set ANSEL and TRIS here so the pin is ready before
     * ADC_ConfigAutoScan is called in ES_INIT.
     */
    DB_printf("[PairBtn] RB2/AN4 configured as analog input for boat selector.\r\n");
}
 
/****************************************************************************
 Function   ReadBoatIndex
 Description
   Reads the boat-selector pot (AN4 / RB2) using a single blocking
   ADC conversion and maps the result to a boat index (1-5).
 
   This uses ADC_MultiRead, which reads all configured auto-scan channels.
   The pot result is in ADCResults[2] when the scan mask is
   (JOY1 | JOY2 | POT) = (AN0 | AN1 | AN4).
 
   Mapping (0-1023 → boat index 1-5):
     0   – 204  → 1
     205 – 408  → 2
     409 – 613  → 3
     614 – 818  → 4
     819 – 1023 → 5
****************************************************************************/
static uint8_t ReadBoatIndex(void)
{
    uint32_t adcResults[3];           /* Joy1, Joy2, Pot                  */
    ADC_MultiRead(adcResults);
    uint32_t potVal = adcResults[2];  /* AN4 is third in the scan order   */
 
    DB_printf("[PairBtn] Pot ADC = %lu\r\n", potVal);
 
    if      (potVal < BOAT_THRESH_1) return 1;
    else if (potVal < BOAT_THRESH_2) return 2;
    else if (potVal < BOAT_THRESH_3) return 3;
    else if (potVal < BOAT_THRESH_4) return 4;
    else                             return 5;
}
