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
#include "FuelServoService.h"
#include "dbprintf.h"
#include "PIC32_AD_Lib.h"
#include <sys/attribs.h> // for interrupts

/*----------------------------- Module Defines ----------------------------*/
#define DEBUG_PRINT_COMMS
// #define SHOW_SENT_BYTES
// #define SHOW_RECEIVED_BYTES
// #define SHOW_ANALOG_VALS
// #define SHOW_FUEL_INPUT_VALS
#define SHOW_SHOOT_INPUT

#define FOUR_SECONDS 4000 // in milliseconds
#define SEND_UART_MS 200  // in milliseconds
#define AUTOPAIRING_MS 2000// in milliseconds

// Potentiometer Info
#define BOAT_ADC_MASK BIT4HI          // AN4 (RB2)
#define BOAT_ADC_ANSEL (ANSELBbits.ANSB2) // AN4
#define BOAT_ADC_TRIS (TRISBbits.TRISB2)  // AN4

// Pair Button (RB15, active LOW)
#define PAIR_BTN_TRIS  (TRISBbits.TRISB15)
#define PAIR_BTN_PORT  (PORTBbits.RB15)

// Status LEDs
// #define PWR_ON_LED_ANSEL (ANSELBbits.ANSB4) // Status LED 1 (RB4)
#define PWR_ON_LED_TRIS  (TRISBbits.TRISB4) // Status LED 1 (RB4)
#define PWR_ON_LED_LAT   (LATBbits.LATB4)   // write to LAT

// #define PAIRED_LED_ANSEL (ANSELAbits.ANSA4) // Paired LED 2 (RA4)
#define PAIRED_LED_TRIS  (TRISAbits.TRISA4) // Paired LED 2 (RA4)
#define PAIRED_LED_LAT   (LATAbits.LATA4)   // write to LAT

// #define DEBUG_LED_ANSEL (ANSELBbits.ANSB5) // Debug LED 3 (RB5)
#define DEBUG_LED_TRIS  (TRISBbits.TRISB5) // Debug LED 3 (RB5)
#define DEBUG_LED_LAT   (LATBbits.LATB5)   // write to LAT


// Joystick Info
#define JOY1_ADC_MASK BIT0HI          // AN0 (RA0)
#define JOY1_ANSEL (ANSELAbits.ANSA0) // AN0
#define JOY1_TRIS (TRISAbits.TRISA0)  // AN0

#define JOY2_ADC_MASK BIT1HI          // AN1 (RA1)
#define JOY2_ANSEL (ANSELAbits.ANSA1) // AN1
#define JOY2_TRIS (TRISAbits.TRISA1)  // AN1

#define JOY_DEAD_RANGE 5 // Deadband range around joystick midpoint (0-255) to prevent noise from causing unintended movement

#define NUM_ANALOG_INPUTS 3

// // UART2 Pins: Rx is RB8, Tx is RB9
//#define U2RX_ANSEL (ANSELBbits.ANSB8) // NO ANSEL
//#define U2TX_ANSEL (ANSELBbits.ANSB9) // NO ANSEL

#define U2RX_TRIS (TRISBbits.TRISB8)
#define U2TX_TRIS (TRISBbits.TRISB9)

#define U2RX_LAT (LATBbits.LATB8)
#define U2TX_LAT (LATBbits.LATB9)

#define BAUD_CONST 129 // 9600 baud with 20MHz PBCLK

// Message frame structure
#define DATA_FRAME_START 3 // Index where data frame starts (after start byte and length bytes)
// Message frame structure RECEIVED BY MALLARD MODULE (TRANSMITTED BY QUACKRAFT)
#define FRAME_SIZE_RX 10 // (Received by Mallard Module)
#define DATA_FRAME_RX_LENGTH (FRAME_SIZE_RX - DATA_FRAME_START - 1)
#define CHECKSUM_RX_INDEX (FRAME_SIZE_RX - 1)

// Message frame structure TRANSMITTED BY MALLARD MODULE (RECEIVED BY QUACKRAFT)
#define FRAME_SIZE_TX 13 // (Transmitted by Mallard Module)
#define DATA_FRAME_TX_LENGTH (FRAME_SIZE_TX - DATA_FRAME_START - 1)
#define CHECKSUM_TX_INDEX (FRAME_SIZE_TX - 1)

#define MY_BOAT_ADDRESS_LSB 0x81
#define BOAT2_ADDRESS_LSB   0x82
#define BOAT3_ADDRESS_LSB   0x83
#define BOAT4_ADDRESS_LSB   0x84
#define BOAT5_ADDRESS_LSB   0x86  // Because the Xbee of team 5 was broken, their Xbee was replaced

// Message bytes
#define START_BYTE 0x7E           // Byte 1
#define LENGTH_MSB_BYTE 0x00      // Byte 2
#define LENGTH_RX_LSB_BYTE 0x06   // Byte 3 (Received by Mallard Module)
#define LENGTH_TX_LSB_BYTE 0x09   // Byte 3 (Transmitted by Mallard Module)
#define API_ID_TX_BYTE 0x01       // Byte 4
#define API_ID_RX_BYTE 0x81       // Byte 4
#define FRAME_ID_BYTE 0x00        // Byte 5
#define DEST_ADD_RX_MSB_BYTE 0x21 // Byte 6  (Quackraft to Mallard Module)
#define DEST_ADD_TX_MSB_BYTE 0x20 // Byte 6  (Mallard Module to Quackraft)
#define MY_CONTROLLER_ADD_LSB_BYTE 0x81 // Byte 7  (Team 1 controller address)
#define OPT_BYTE 0x01             // Byte 8
#define STATUS_DRIVING_BYTE 0x00  // Byte 9
#define STATUS_CHARGING_BYTE 0x01 // Byte 9
#define STATUS_PAIRING_BYTE 0x02  // Byte 9

#define DIGI_SHOOT_BYTE    0x01
#define DIGI_NO_SHOOT_BYTE 0x00

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

static bool newMessageStarted  = false; // new start byte received flag
static bool newMessageComplete = false; // complete valid message received flag
static bool pairedStatus       = false;

static volatile uint8_t receivedByte;               // most recent UART byte received
static volatile uint8_t rxBuf[FRAME_SIZE_RX] = {0}; // Buffer to hold the most recent received bytes
static uint8_t txBuf[FRAME_SIZE_TX]          = {0}; // Buffer to hold the bytes to be transmitted

static uint8_t desiredAddressLSB;
static uint8_t Addresses[]    = {0x00, 0x81, 0x82, 0x83, 0x84, 0x85};
static uint8_t ChargeVal      = 0xFF; // Default initial value per comms protocol
static uint16_t JoyResolution = 255;  // max value, 8 bits
static uint16_t JoyMidPoint;

static uint8_t BoatPotVal = 0;  // raw 8-bit pot reading, updated each ADC scan

static uint32_t ADCResults[NUM_ANALOG_INPUTS];       // ADC results array (Joy1, Joy2, more?)
static uint8_t StatusVal, Joy1Val, Joy2Val, DigiVal; // Variables for data to be sent to boat
static volatile uint8_t CheckSumVal = 0;

static uint16_t ChargingBytesPending;
static uint16_t ShootingBytesPending;

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
    clrScrn();
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

    switch (ThisEvent.EventType)
    {
        // This event is run once at the end of service initialisation
        case ES_INIT:
        {
            DB_printf("\rES_INIT received in MallardCommunicationService, priority: %d\r\n",
                      MyPriority);
            // Initialize hardware for communication here
            InitUART();
            // Initialize ADC
            JOY1_ANSEL = 1; // Set as analog
            JOY1_TRIS  = 1; // Set as input
            JOY2_ANSEL = 1; // Set as analog
            JOY2_TRIS  = 1; // Set as input
            BOAT_ADC_ANSEL = 1; // RB2: analog input (AN4, boat selector pot)
            BOAT_ADC_TRIS = 1;  // RB2: input

            // Configure AN0 and AN1 (joystick) and AN44 (boat selector)for auto scan
            ADC_ConfigAutoScan(JOY1_ADC_MASK | JOY2_ADC_MASK | BOAT_ADC_MASK);

            // Initialize other hardware pins
            PAIR_BTN_TRIS = 1;    // pair button: digital input with pull-up
            // PWR_ON_LED_ANSEL = 0; // status LED 1: digital
            PWR_ON_LED_TRIS  = 0; // status LED 1: output
            PWR_ON_LED_LAT   = 1; // start with LED ON
            // PAIRED_LED_ANSEL = 0; // paired LED 2: digital
            PAIRED_LED_TRIS  = 0; // paired LED 2: output
            PAIRED_LED_LAT   = 0; // start with LED off
            // DEBUG_LED_ANSEL = 0;  // debug LED 3: digital
            DEBUG_LED_TRIS  = 0;  // debug LED 3: output
            DEBUG_LED_LAT   = 0;  // start with LED off

            // Initialize variables
            desiredAddressLSB    = MY_BOAT_ADDRESS_LSB;
            StatusVal            = STATUS_PAIRING_BYTE; // Start in pairing status
            JoyMidPoint          = JoyResolution / 2;
            Joy1Val              = JoyMidPoint;
            Joy2Val              = JoyMidPoint;
            DigiVal              = DIGI_NO_SHOOT_BYTE;
            ChargingBytesPending = 0;
            ShootingBytesPending = 0;
            DB_printf("\rMallardCommunicationService initialization complete\r\n");

            // // Uncomment the following for autopairing instead of manually
            // ES_Timer_InitTimer(SEND_MSG_TIMER, AUTOPAIRING_MS); // start autopairing
        }
        break;

        case ES_TIMEOUT:
        {
            if (ThisEvent.EventParam == SEND_MSG_TIMER)
            {
                // DB_printf("\rSEND_MSG_TIMER expired in MallardCommunicationService\r\n");
                // Read ADC values for joysticks
                ReadADCValues();
                // DB_printf("\rADC Readings - Joy1: %d, Joy2: %d\r\n", Joy1Val, Joy2Val);
                // Send MSG to Quackraft
                if (ShootingBytesPending > 0)
                {
                    DigiVal = DIGI_SHOOT_BYTE;
                    ShootingBytesPending --;
                } else
                {
                    DigiVal = DIGI_NO_SHOOT_BYTE;
                }
                #ifdef SHOW_SHOOT_INPUT
                DB_printf("DigiVal: 0x%x", DigiVal);
                #endif
                SendMsgToQuackraft(StatusVal, Joy1Val, Joy2Val, DigiVal);
                // Restart timer
                ES_Timer_InitTimer(SEND_MSG_TIMER, SEND_UART_MS);
            }
            if (ThisEvent.EventParam == UNPAIRING_TIMER)
            {
                pairedStatus  = false;
                PAIRED_LED_LAT = 0; // Turn off paired LED
                StatusVal     = STATUS_PAIRING_BYTE;
                DB_printf("\rUnpairing timeout: back to pairing\r\n");
            }
        }
        break;

        case ES_START_PAIRING:
        {
            ReadADCValues();
            // Read pot right now to get the freshest value, then map to address index.
            // 8-bit range 0-255 split into 5 equal bands of 51 counts each.
            uint8_t addrIndex;
            if      (BoatPotVal < 51)  addrIndex = 1;
            else if (BoatPotVal < 102) addrIndex = 2;
            else if (BoatPotVal < 153) addrIndex = 3;
            else if (BoatPotVal < 204) addrIndex = 4;
            else                       addrIndex = 5;

            desiredAddressLSB = Addresses[addrIndex];
            DB_printf("\rES_START_PAIRING: pot=%u -> boat index %u -> addr 0x%x\r\n",
                    BoatPotVal, addrIndex, desiredAddressLSB);

            StatusVal = STATUS_PAIRING_BYTE;
            ES_Timer_InitTimer(SEND_MSG_TIMER, SEND_UART_MS);
        }
        break;

        case ES_REFUEL_INPUT:
        {
            #ifdef SHOW_FUEL_INPUT_VALS
            DB_printf("\rES_REFUEL_INPUT received in MallardCommunicationService\r\n");
            #endif
            if (pairedStatus == true)
            {
                DEBUG_LED_LAT = 1; // Turn on Status LED 3
                StatusVal = STATUS_CHARGING_BYTE;
                
                ChargingBytesPending += 5; // Add 5 charging bytes to be sent (each charging input equals 5 messages)
                // TODO: Implement refuel input handling (maybe its own service for servo indicator)
                #ifdef SHOW_FUEL_INPUT_VALS
                DB_printf("\rES_REFUEL_INPUT received in MallardCommunicationService, ChargingBytesPending: %u\r\n",
                        ChargingBytesPending);
                #endif
            }
        }
        break;

        case ES_SHOOT:
        {
            #ifdef SHOW_SHOOT_INPUT
            DB_printf("\rES_SHOOT received in MallardCommunicationService\r\n");
            #endif

            if (pairedStatus == true)
            {
                ShootingBytesPending += 5;
            }
        }

        case ES_CHANGE_ADDR: // triggered by event checker
        {
            // Get new address from event parameter
            desiredAddressLSB = Addresses[ThisEvent.EventParam] & 0xFF;
            DB_printf("\rCHANGE_ADDR event received in MallardCommunicationService, new desired "
                      "address LSB: 0x%02X\r\n",
                      desiredAddressLSB);
        }
        break;

        case ES_RX_BYTE:
        {
            switch (ThisEvent.EventParam)
            {
                case START_BYTE:
                {
                    // clear rxBuf to start filling it up with new message
                    for (uint16_t i = 0; i < FRAME_SIZE_RX; i++)
                    {
                        rxBuf[i] = 0;
                    }
                    newMessageStarted = true; // indicate new message started, add to RxBuffer
                    rxBuf[FRAME_SIZE_RX - 1] = START_BYTE; // Store start byte in buffer
                }
                break;
                default:
                {
                    if (newMessageStarted)
                    {
                        // Shift bytes in buffer and add new byte
                        for (uint16_t i = 0; i < FRAME_SIZE_RX - 1; i++)
                        {
                            rxBuf[i] = rxBuf[i + 1];
                        }
                        rxBuf[FRAME_SIZE_RX - 1] = ThisEvent.EventParam;

                        // Check if we have received a complete message
                        if (START_BYTE == rxBuf[0])
                        {
                            if (ValidReceivedMessage())
                            {
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
        break;

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
    U2MODEbits.ON = 0; // Disable the UART

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
    U2TX_TRIS = 0;       // Set as output
    U2TX_LAT  = 1;       // start with TX high
    RPB9R     = 0b0010;  // 0010 = U2TX pg. 136 -> B9 is TX

    U2STAbits.URXEN = 1; // Enable the receiver
                         //  U2RX_ANSEL = 0;    // Set as digital NO ANSEL
    U2RX_TRIS = 1;       // Set as input
    U2RXR     = 0b0100;  // 0100 = RPB8 pg. 134 -> B8 is RX

    U2STAbits.URXISEL = 0b00;
    INTCONbits.MVEC   = 1; // Multivectored interrupts

    // Enable RX interrupt
    IEC1bits.U2RXIE = 1;
    IPC9bits.U2IP   = 7; // Priority 7
    IPC9bits.U2IS   = 0; // Subpriority 0
    IFS1bits.U2RXIF = 0; // Clear interrupt flag

    __builtin_enable_interrupts(); // Enable global interrupts

    // Enable Tx interrupts?
    // Interrupt when a character is transferred to the Transmit Shift Register
    U2STAbits.UTXISEL = 0;

    // Write the baud rate constant to the UxBRG register.
    U2BRG = BAUD_CONST; // (129) for 9600 baud rate with 20MHz PBCLK

    // Enable the UART by setting the ON bit in the UxMODE register.
    U2MODEbits.ON = 1; // Enable the UART
}

void __ISR(_UART_2_VECTOR, IPL7SOFT) U2RX_ISR(void)
{
    if (IFS1bits.U2RXIF)
    {
        while (U2STAbits.URXDA)
        {
            receivedByte = U2RXREG;
            ES_Event_t NewEvent;
            NewEvent.EventType  = ES_RX_BYTE;
            NewEvent.EventParam = receivedByte;
            PostMallardCommunicationService(NewEvent);
        }
        IFS1CLR = _IFS1_U2RXIF_MASK; // Clear interrupt flag
    }
}

static void ComputeCheckSum(uint8_t dataFrameLength)
{
    uint8_t sum = 0;
    bool isTx   = false;
    if (dataFrameLength == DATA_FRAME_TX_LENGTH)
    {
        isTx = true;
    }
    for (uint16_t i = DATA_FRAME_START; i < DATA_FRAME_START + dataFrameLength; i++)
    {
        if (isTx)
        {
            sum += txBuf[i];
        }
        else
        {
            sum += rxBuf[i];
        }
    }
    sum         = 0xFF - sum;
    CheckSumVal = sum;
}

static bool ValidReceivedMessage(void)
{
    bool ValidMessage = false;
    // Check Sum first
    ComputeCheckSum(DATA_FRAME_RX_LENGTH);
    if (rxBuf[CHECKSUM_RX_INDEX] != CheckSumVal)
    {
        return ValidMessage; // Invalid message due to checksum failure
    }
#ifdef SHOW_RECEIVED_BYTES
    DB_printf("\r Received Message bytes: \r\n");
    for (uint8_t i = 0; i < FRAME_SIZE_RX; i++)
    {
        DB_printf("0x%x ", rxBuf[i]);
    }
    DB_printf("\r\n");
#endif

    // Validate: start byte, length, API ID, checksum, and matching address
    if ((rxBuf[0] == START_BYTE) && (rxBuf[1] == LENGTH_MSB_BYTE) &&
        (rxBuf[2] == LENGTH_RX_LSB_BYTE) && (rxBuf[3] == API_ID_RX_BYTE) &&
        (rxBuf[4] == DEST_ADD_TX_MSB_BYTE))
    // (rxBuf[5] == pairedAddressLSB)      && // possible to not know it yet
    // (rxBuf[6] == RSSI_BYTE) && Rx signal strength in
    // (rxBuf[7] == OPT_BYTE))
    {
        ValidMessage = true; // Paired and address matches, valid message
    }
    return ValidMessage;
}

static void InterpretMessage(void)
{
    if ((pairedStatus == false) && (rxBuf[4] == DEST_ADD_TX_MSB_BYTE) &&
        (rxBuf[5] == desiredAddressLSB))
    {
        pairedStatus = true; // Update paired status on valid message receipt
        // Reset the timer on paired message
        ES_Timer_InitTimer(UNPAIRING_TIMER, FOUR_SECONDS);
                                
        PAIRED_LED_LAT = 1; // Turn on paired LED
        StatusVal    = STATUS_DRIVING_BYTE;
        ES_Event_t NewEvent;
        NewEvent.EventType = ES_PAIRED;
        ES_PostAll(NewEvent);
        DB_printf("\rValid message received in MallardCommunicationService, PAIRED!\r\n");        
    }
    else if ((pairedStatus == true) && (rxBuf[4] == DEST_ADD_TX_MSB_BYTE) &&
             (rxBuf[5] == desiredAddressLSB))
    {
        // Reset the timer on paired message
        ES_Timer_InitTimer(UNPAIRING_TIMER, FOUR_SECONDS);
        ChargeVal = rxBuf[8]; // Update charge value from message
        ES_Event_t NewEvent;
        NewEvent.EventType = ES_FUEL_VAL_RECEIVED;
        NewEvent.EventParam = ChargeVal;
        PostFuelServoService(NewEvent);
    }
}

static void SendMsgToQuackraft(uint8_t status, uint8_t joy1, uint8_t joy2, uint8_t digi)
{
    // Construct message frame
    txBuf[0] = START_BYTE;
    txBuf[1] = LENGTH_MSB_BYTE;
    txBuf[2] = LENGTH_TX_LSB_BYTE;
    txBuf[3] = API_ID_TX_BYTE;
    txBuf[4] = FRAME_ID_BYTE;
    txBuf[5] = DEST_ADD_TX_MSB_BYTE;
    txBuf[6] = desiredAddressLSB;
    txBuf[7] = OPT_BYTE;
    txBuf[8] = status;

    // Switch from charging to driving if there aren't pending charging bytes to send, so that we can send joystick commands again
    if ((status == STATUS_CHARGING_BYTE)&&(ChargingBytesPending == 0))
    {
        status = STATUS_DRIVING_BYTE; // After sending all charging messages, switch back to driving status
        txBuf[8] = status;
    }

    // Choosing type of message to send based on status
    if (status == STATUS_PAIRING_BYTE)
    {
        txBuf[9]  = DEST_ADD_RX_MSB_BYTE;
        txBuf[10] = MY_CONTROLLER_ADD_LSB_BYTE;
        txBuf[11] = 0x00; // No joystick or digi info in pairing message
    }
    else if (status == STATUS_CHARGING_BYTE)
    {
        // Per communications protocol
        txBuf[9]  = 0x00;
        txBuf[10] = 0x00;
        txBuf[11] = 0x00;
        ChargingBytesPending --; // Decrement pending charging bytes, send 1 charging message per timer expiration
        #ifdef SHOW_FUEL_INPUT_VALS
        DB_printf("/rChagingBytesPending: %u\r\n", ChargingBytesPending);
        #endif
        DEBUG_LED_LAT = 0; // Turn off Status LED 3
    }
    else
    {
        txBuf[9]  = joy1;
        txBuf[10] = joy2;
        txBuf[11] = digi;
    }

    ComputeCheckSum(DATA_FRAME_TX_LENGTH);
    txBuf[CHECKSUM_TX_INDEX] = CheckSumVal;

    // Transmit message byte by byte
    for (uint8_t i = 0; i < FRAME_SIZE_TX; i++)
    {
        while (U2STAbits.UTXBF)
        {
            ; // Wait until transmit buffer is not full
        }
        U2TXREG = txBuf[i]; // Write byte to transmit register
    }

#ifdef SHOW_SENT_BYTES
    DB_printf("\r Sent Message bytes: (to address 0x%x)\r\n", desiredAddressLSB);
    for (uint8_t i = 0; i < FRAME_SIZE_TX; i++)
    {
        DB_printf("0x%x ", txBuf[i]);
    }
    DB_printf("\r\n");
#endif
}

static void ReadADCValues(void)
{
    ADC_MultiRead(ADCResults);
    Joy1Val = (uint8_t)(ADCResults[0] >> 2); // fit the 10 bits to 8
    Joy2Val = (uint8_t)(ADCResults[1] >> 2); // fit the 10 bits to 8

    if ((Joy1Val < (JoyMidPoint + JOY_DEAD_RANGE)) && 
        (Joy1Val > (JoyMidPoint - JOY_DEAD_RANGE)))
    {
        Joy1Val = JoyMidPoint; // Remove deadband gap above midpoint
    }
    if ((Joy2Val < (JoyMidPoint + JOY_DEAD_RANGE)) && 
        (Joy2Val > (JoyMidPoint - JOY_DEAD_RANGE)))
    {
        Joy2Val = JoyMidPoint; // Remove deadband gap above midpoint
    }

    BoatPotVal = (uint8_t)(ADCResults[2] >> 2); // 8-bit, stored for pairing
    #ifdef SHOW_ANALOG_VALS
    DB_printf("\rADC Readings - Joy1: %d, Joy2: %d, BoatPot: %d\r\n", Joy1Val, Joy2Val, BoatPotVal);
    #endif
}

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/
