/****************************************************************************
 Module
   MallardCommunicationService.c

 Revision
   1.0.1

 Description
   This file implements MallardCommunicationService as a simple service under the
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
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "MallardCommunicationService.h"
#include "dbprintf.h"
#include "PIC32_AD_Lib.h"
#include <sys/attribs.h> // for interrupts

/*----------------------------- Module Defines ----------------------------*/
#define DEBUG_PRINT_COMMS

#define FOUR_SECONDS 4000 // in milliseconds
#define SEND_UART_MS 200  // in milliseconds

// Joystick Info
#define JOY1_ADC_MASK BIT0HI // AN0 (RA0)
#define JOY1_ANSEL (ANSELAbits.ANSA0) // AN0
#define JOY1_TRIS (TRISAbits.TRISA0) // AN0

#define JOY2_ADC_MASK BIT1HI // AN1 (RA1)
#define JOY2_ANSEL (ANSELAbits.ANSA1) // AN1
#define JOY2_TRIS (TRISAbits.TRISA1) // AN1

#define NUM_ANALOG_INPUTS 2


// // UART2 Pins: Rx is RB8, Tx is RB9 
//#define U2RX_ANSEL (ANSELBbits.ANSB8) // NO ANSEL
//#define U2TX_ANSEL (ANSELBbits.ANSB9) // NO ANSEL

#define U2RX_TRIS (TRISBbits.TRISB8)
#define U2TX_TRIS (TRISBbits.TRISB9)

#define U2RX_LAT (LATBbits.LATB8)
#define U2TX_LAT (LATBbits.LATB9)

#define BAUD_CONST 129 // 9600 baud with 20MHz PBCLK

// Message frame structure
#define DATA_FRAME_START   3 // Index where data frame starts (after start byte and length bytes)
// Message frame structure RECEIVED BY MALLARD MODULE (TRANSMITTED BY QUACKRAFT)
#define FRAME_SIZE_RX      10 // (Received by Mallard Module)
#define DATA_FRAME_RX_LENGTH  (FRAME_SIZE_RX - DATA_FRAME_START - 1)
#define CHECKSUM_RX_INDEX     (FRAME_SIZE_RX - 1)

// Message frame structure TRANSMITTED BY MALLARD MODULE (RECEIVED BY QUACKRAFT)
#define FRAME_SIZE_TX      13 // (Transmitted by Mallard Module)
#define DATA_FRAME_TX_LENGTH  (FRAME_SIZE_TX - DATA_FRAME_START - 1)
#define CHECKSUM_TX_INDEX     (FRAME_SIZE_TX - 1)

// Message bytes
#define START_BYTE            0x7E // Byte 1
#define LENGTH_MSB_BYTE       0x00 // Byte 2
#define LENGTH_RX_LSB_BYTE    0x06 // Byte 3 (Received by Mallard Module)
#define LENGTH_TX_LSB_BYTE    0x09 // Byte 3 (Transmitted by Mallard Module)
#define API_ID_BYTE           0x01 // Byte 4
#define FRAME_ID_BYTE         0x00 // Byte 5
#define DEST_ADD_RX_MSB_BYTE  0x21 // Byte 6  (Quackraft to Mallard Module)
#define DEST_ADD_TX_MSB_BYTE  0x20 // Byte 6  (Mallard Module to Quackraft)
#define MY_DEST_ADD_LSB_BYTE  0x85 // Byte 7  (CHECK THIS!!)
#define OPT_BYTE              0x01 // Byte 8
#define STATUS_DRIVING_BYTE   0x00 // Byte 9
#define STATUS_CHARGING_BYTE  0x01 // Byte 9
#define STATUS_PAIRING_BYTE   0x02 // Byte 9

           
/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/
static void InitUART(void);
static void ComputeCheckSum(uint8_t);
static bool ValidReceivedMessage(void);
static void InterpretMessage(void);
static void SendMsgToQuackraft(uint8_t status, uint8_t joy1, uint8_t joy2, uint8_t digi);
static void ReadADCValues(void);

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;

static bool newMessageStarted = false; // Flag to indicate a new start byte has been received
static bool newMessageComplete = false; // Flag to indicate a complete valid message has been received
static bool pairedStatus = false;

static volatile uint8_t receivedByte; // Variable to hold the most recent byte received from UART
static volatile uint8_t rxBuf[FRAME_SIZE_RX] = {0}; // Buffer to hold the most recent received bytes
static uint8_t txBuf[FRAME_SIZE_TX] = {0}; // Buffer to hold the bytes to be transmitted

static uint8_t desiredAddressLSB;
static uint8_t Addresses[] = {0x00, 0x81, 0x82, 0x83, 0x84, 0x85};
static uint8_t ChargeVal = 0xFF; // Default initial value per comms protocol
static uint16_t JoyResolution = 255; // max value, 8 bits
static uint16_t JoyMidPoint;
// ADC results array
static uint32_t ADCResults[NUM_ANALOG_INPUTS]; // Joy1, Joy2, more?
static uint8_t StatusVal, Joy1Val, Joy2Val, DigiVal; // Variables to hold data to be sent to Quackraft
static volatile uint8_t CheckSumVal = 0;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitMallardCommunicationService

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
bool InitMallardCommunicationService(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  MyPriority = Priority;
  /********************************************
   in here you write your initialization code
   *******************************************/
  // Announce initialisation of MallardCommunicationService
  DB_printf("\rStarting MallardCommunicationService: ");
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
     PostMallardCommunicationService

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
bool PostMallardCommunicationService(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunMallardCommunicationService

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
ES_Event_t RunMallardCommunicationService(ES_Event_t ThisEvent)
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
        DB_printf("\rES_INIT received in MallardCommunicationService, priority: %d\r\n", MyPriority);
        // Initialize hardware for communication here
        InitUART();
        // Initialize ADC
        JOY1_ANSEL = 1;    // Set as analog
        JOY1_TRIS = 1;    // Set as input
        JOY2_ANSEL = 1;    // Set as analog
        JOY2_TRIS = 1;    // Set as input
        ADC_ConfigAutoScan(JOY1_ADC_MASK | JOY2_ADC_MASK); // Configure AN0 and AN1 for auto scan
        // Initialize vairables
        desiredAddressLSB = 0x85; // Default or Quackraft
        StatusVal = STATUS_PAIRING_BYTE; // Start in pairing status
        JoyMidPoint = JoyResolution / 2;
        Joy1Val = JoyMidPoint;
        Joy2Val = JoyMidPoint;
        DigiVal = 0x00;
        DB_printf("\rMallardCommunicationService initialization complete\r\n");
      }
      break;

      case ES_TIMEOUT:
      {
        if (ThisEvent.EventParam == SEND_MSG_TIMER) {
          DB_printf("\rSEND_MSG_TIMER expired in MallardCommunicationService\r\n");
          // Read ADC values for joysticks
          ReadADCValues();
          // DB_printf("\rADC Readings - Joy1: %d, Joy2: %d\r\n", Joy1Val, Joy2Val);
          // Send MSG to Quackraft
          SendMsgToQuackraft(StatusVal, Joy1Val, Joy2Val, DigiVal);
          DB_printf("/r checksum: 0x%x\r\n", CheckSumVal);
          // Restart timer
          ES_Timer_InitTimer(SEND_MSG_TIMER, SEND_UART_MS);
        }
      }
      break;

      case ES_START_PAIRING: // triggered by event checker
      {
        StatusVal = STATUS_PAIRING_BYTE;
        ES_Timer_InitTimer(SEND_MSG_TIMER, SEND_UART_MS);
      }
      break;

      case ES_CHANGE_ADDR: // triggered by event checker
      {
        desiredAddressLSB = Addresses[ThisEvent.EventParam] & 0xFF; // Get new address from event parameter
        DB_printf("\rCHANGE_ADDR event received in MallardCommunicationService, new desired address LSB: 0x%02X\r\n", desiredAddressLSB);
      }
      break;

      case ES_RX_BYTE:
      {
        switch (ThisEvent.EventParam) {
          case START_BYTE:
          {
            // clear rxBuf to start filling it up with new message
            for (uint16_t i = 0; i < FRAME_SIZE_RX; i++) {
              rxBuf[i] = 0;
            }
            newMessageStarted = true; // indicate new message started, add to RxBuffer
            rxBuf[FRAME_SIZE_RX - 1] = START_BYTE; // Store start byte in buffer
          }
            break;
          default:
          {
            if (newMessageStarted) {
              // Shift bytes in buffer and add new byte
              for (uint16_t i = 0; i < FRAME_SIZE_RX - 1; i++) {
                rxBuf[i] = rxBuf[i + 1];
              }
              rxBuf[FRAME_SIZE_RX - 1] = ThisEvent.EventParam;

              // Check if we have received a complete message
              if (START_BYTE == rxBuf[0]) {
                if (ValidReceivedMessage()) {
                newMessageComplete = true;
                InterpretMessage(); // Post to necessary services based on message content
                newMessageStarted = false; // Reset for next message
                }
              }
              
            }
          }
            break;
        }
      }
      default:
        break;
      
    }
  return ReturnEvent;
}

/***************************************************************************
 private functions
 ***************************************************************************/
static void InitUART(void)
{
  U2MODEbits.ON = 0;  // Disable the UART  

  // U2Mode register:
  // Configure the UxMODE register to clear the SIDL, IREN, RTSMD, UEN, WAKE, LPBACK, ABAUD, & RXINV bits.
  U2MODE = 0;
  // Configure the UxMODE register to choose the high or low speed baud clock
  U2MODEbits.BRGH = 0; // Standard Speed mode
  // Configure the UxMODE register to choose the number of data bits, stop bits and parity.
  U2MODEbits.PDSEL = 0; // 8 bits, no parity
  U2MODEbits.STSEL = 0; // 1 stop bit
  // U2STA register:
  // Configure the UxSTA register to clear the UTXINV, UTXBRK & ADDEN bits.
  U2STA = 0;
  // Set the UTXEN & URXEN bits in UxSTA register to enable the transmitter & receiver.
  U2STAbits.UTXEN = 1; // Enable the transmitter
//  U2TX_ANSEL = 0;    // Set as digital NO ANSEL
  U2TX_TRIS = 0;    // Set as output
  U2TX_LAT = 1; // start with TX high
  RPB9R  = 0b0010;    // 0010 = U2TX pg. 136 -> B9 is TX

  U2STAbits.URXEN = 1; // Enable the receiver
//  U2RX_ANSEL = 0;    // Set as digital NO ANSEL
  U2RX_TRIS = 1;    // Set as input
  U2RXR  = 0b0100;    // 0100 = RPB8 pg. 134 -> B8 is RX

  U2STAbits.URXISEL = 0b00;
  INTCONbits.MVEC = 1;      // Multivectored interrupts

  // Enable RX interrupt
  IEC1bits.U2RXIE = 1;      
  IPC9bits.U2IP = 7;        // Priority 7
  IPC9bits.U2IS = 0;        // Subpriority 0
  IFS1bits.U2RXIF = 0;      // Clear interrupt flag

  __builtin_enable_interrupts();  // Enable global interrupts

  // Enable Tx interrupts?
  U2STAbits.UTXISEL = 0; // Interrupt when a character is transferred to the Transmit Shift Register

  // Write the baud rate constant to the UxBRG register.
  U2BRG = BAUD_CONST; // (129) for 9600 baud rate with 20MHz PBCLK
  
  // Enable the UART by setting the ON bit in the UxMODE register.
  U2MODEbits.ON = 1;  // Enable the UART  
}


void __ISR(_UART_2_VECTOR, IPL7SOFT) U2RX_ISR(void) {
  if (IFS1bits.U2RXIF) {
    while (U2STAbits.URXDA) {
      receivedByte = U2RXREG;
      ES_Event_t NewEvent;
      NewEvent.EventType  = ES_RX_BYTE;
      NewEvent.EventParam = receivedByte;
      PostMallardCommunicationService(NewEvent);
    }
    IFS1CLR = _IFS1_U2RXIF_MASK;  // Clear interrupt flag
  }
}


static void ComputeCheckSum(uint8_t dataFrameLength) {
  uint8_t sum = 0;
  bool isTx = false;
  if (dataFrameLength == DATA_FRAME_TX_LENGTH) {
    isTx = true;
  }
  for (uint16_t i = DATA_FRAME_START; i < DATA_FRAME_START + dataFrameLength; i++) {
    if (isTx) {
      sum += txBuf[i];
    } else {
    sum += rxBuf[i];
    }
  }
  sum = 0xFF - sum;
  CheckSumVal = sum;
}
static bool ValidReceivedMessage(void) {
  bool ValidMessage = false;
  // Check Sum first
  ComputeCheckSum(DATA_FRAME_RX_LENGTH);
  if (rxBuf[CHECKSUM_RX_INDEX] != CheckSumVal) {
    return ValidMessage; // Invalid message due to checksum failure
  }
  // Validate: start byte, length, API ID, checksum, and matching address
  if ((rxBuf[0] == START_BYTE)           &&
      (rxBuf[1] == LENGTH_MSB_BYTE)      && 
      (rxBuf[2] == LENGTH_RX_LSB_BYTE)   &&
      (rxBuf[3] == API_ID_BYTE)          &&
      (rxBuf[4] == FRAME_ID_BYTE)        &&
      (rxBuf[5] == DEST_ADD_RX_MSB_BYTE) &&
      (rxBuf[6] == MY_DEST_ADD_LSB_BYTE) && //  message for me
      (rxBuf[7] == OPT_BYTE)) {
    ValidMessage = true; // Paired and address matches, valid message   
    
    if (pairedStatus == false) {
      pairedStatus = true; // Update paired status on valid message receipt
      ES_Event_t NewEvent;
      NewEvent.EventType  = ES_PAIRED;
      ES_PostAll(NewEvent);
      DB_printf("\rValid message received in MallardCommunicationService, PAIRED!\r\n");
    }
  }
  return ValidMessage;
 }

static void InterpretMessage(void) {
  if (rxBuf[8] == STATUS_PAIRING_BYTE) {
    
  } else {
    
  }    
 }

static void SendMsgToQuackraft(uint8_t status, uint8_t joy1, uint8_t joy2, uint8_t digi){
  // Construct message frame
  txBuf[0] = START_BYTE;
  txBuf[1] = LENGTH_MSB_BYTE;
  txBuf[2] = LENGTH_TX_LSB_BYTE;
  txBuf[3] = API_ID_BYTE;
  txBuf[4] = FRAME_ID_BYTE;
  txBuf[5] = DEST_ADD_TX_MSB_BYTE;
  txBuf[6] = desiredAddressLSB;
  txBuf[7] = OPT_BYTE;
  txBuf[8] = status;
  if (status == STATUS_PAIRING_BYTE)
  {
    txBuf[9] = DEST_ADD_RX_MSB_BYTE; 
    txBuf[10] = MY_DEST_ADD_LSB_BYTE;
  } else if (status == STATUS_CHARGING_BYTE) {
    // Per communications protocol
    txBuf[9] =  0x00;
    txBuf[10] = 0x00;
    txBuf[11] = 0x00;
  } else 
  {
    txBuf[9] = joy1;
    txBuf[10] = joy2;
    txBuf[11] = digi;
  }
  ComputeCheckSum(DATA_FRAME_TX_LENGTH);
  txBuf[CHECKSUM_TX_INDEX] = CheckSumVal;

  // Transmit message byte by byte
  for (uint8_t i = 0; i < FRAME_SIZE_TX; i++) {
    while (U2STAbits.UTXBF) {
      ; // Wait until transmit buffer is not full
    }
    U2TXREG = txBuf[i]; // Write byte to transmit register
  }
}

static void ReadADCValues(void) {
  ADC_MultiRead(ADCResults);
  Joy1Val = (uint8_t)(ADCResults[0] >> 2); // fit the 10 bits to 8
  Joy2Val = (uint8_t)(ADCResults[1] >> 2); // fit the 10 bits to 8
}

 /*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/

