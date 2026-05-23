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
#include "CommunicationService.h"
#include "DrivingService.h"
#include "BoatActionsService.h"
#include "PairedServoService.h"
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "dbprintf.h"
#include <sys/attribs.h> // for interrupts

/*----------------------------- Module Defines ----------------------------*/
#define DEBUG_PRINT_COMMS
// #define SHOW_SENT_BYTES
// #define SHOW_RECEIVED_BYTES
#define SHOW_CHARGE

#define FOUR_SECONDS 4000 // in milliseconds
#define SEND_UART_MS 200  // in milliseconds

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

// Message frame structure RECEIVED BY QUACKRAFT
#define FRAME_SIZE_RX 13 // (Received by Quackraft)
#define DATA_FRAME_RX_LENGTH (FRAME_SIZE_RX - DATA_FRAME_START - 1)
#define CHECKSUM_RX_INDEX (FRAME_SIZE_RX - 1)

// Message frame structure TRANSMITTED BY QUACKRAFT
#define FRAME_SIZE_TX 10 // (Transmitted by Quackraft)
#define DATA_FRAME_TX_LENGTH (FRAME_SIZE_TX - DATA_FRAME_START - 1)
#define CHECKSUM_TX_INDEX (FRAME_SIZE_TX - 1)

// Message bytes
#define START_BYTE 0x7E                 // Byte 1
#define LENGTH_MSB_BYTE 0x00            // Byte 2
#define LENGTH_RX_LSB_BYTE 0x09         // Byte 3 (Received by Quackraft)
#define LENGTH_TX_LSB_BYTE 0x06         // Byte 3 (Transmitted by Quackraft)
#define API_ID_TX_BYTE 0x01             // Byte 4
#define API_ID_RX_BYTE 0x81             // Byte 4
#define FRAME_ID_BYTE 0x00              // Byte 5
#define DEST_ADD_RX_MSB_BYTE 0x20       // Byte 6  (Mallard Module to Quackraft)
#define DEST_ADD_TX_MSB_BYTE 0x21       // Byte 6  (Quackraft to Mallard Module)
#define MY_CONTROLLER_ADD_LSB_BYTE 0x81 // Byte 7  (UNUSED, should match controller/mallard)
#define OPT_BYTE 0x01                   // Byte 8
#define STATUS_DRIVING_BYTE 0x00        // Byte 9
#define STATUS_CHARGING_BYTE 0x01       // Byte 9
#define STATUS_PAIRING_BYTE 0x02        // Byte 9

#define JOY_MIDPOINT 127   // Midpoint value for joystick inputs (0-255 range)
#define MAX_CHARGE_VAL 200 // Maximum charge value for the boat
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
static void SendMsgToMallardModule(uint8_t);

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;

static bool newMessageStarted  = false; // new start byte received flag
static bool newMessageComplete = false; // complete valid message received flag
static bool pairedStatus       = false;
static bool InitializeFuel     = false;

static volatile uint8_t receivedByte; // Variable to hold the most recent byte received from UART
static volatile uint8_t rxBuf[FRAME_SIZE_RX] = {0}; // Buffer to hold the most recent received bytes
static uint8_t txBuf[FRAME_SIZE_TX]          = {0}; // Buffer to hold the bytes to be transmitted

static uint8_t pairedAddressLSB;
static uint8_t Addresses[]          = {0x00, 0x81, 0x82, 0x83, 0x84, 0x86};
static uint8_t ChargeVal            = 0xFF; // Default initial value per comms protocol
static volatile uint8_t CheckSumVal = 0;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitCommunicationService

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
bool InitCommunicationService(uint8_t Priority)
{
    ES_Event_t ThisEvent;

    MyPriority = Priority;
    /********************************************
   in here you write your initialization code
   *******************************************/
    // Announce initialisation of CommunicationService
    DB_printf("\rStarting CommunicationService: ");
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
     PostCommunicationService

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
bool PostCommunicationService(ES_Event_t ThisEvent)
{
    return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunCommunicationService

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
ES_Event_t RunCommunicationService(ES_Event_t ThisEvent)
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
            DB_printf("\rES_INIT received in CommunicationService, priority: %d\r\n", MyPriority);
            // Initialize hardware for communication here
            InitUART();
            DB_printf("\rCommunicationService initialization complete\r\n");
        }
        break;

        case ES_TIMEOUT:
        {
            if (ThisEvent.EventParam == UNPAIRING_TIMER)
            {
                DB_printf("\rUNPAIRING_TIMER expired in CommunicationService\r\n");
                // Example: Send periodic status update to Quackraft
                pairedStatus = false; // Simulate unpairing for testing
                ES_Event_t NewEvent;
                NewEvent.EventType = ES_UNPAIRED;
                PostPairedServoService(NewEvent);
            }
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
                                // DB_printf("\rValid message received in CommunicationService\r\n");
                                // Reply with acknowledgment message
                                SendMsgToMallardModule(ChargeVal);
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
    U2STAbits.UTXISEL =
        0; // Interrupt when a character is transferred to the Transmit Shift Register

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
            PostCommunicationService(NewEvent);
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
    if ((pairedStatus == false) && (rxBuf[8] == STATUS_PAIRING_BYTE) &&
        (rxBuf[9] == DEST_ADD_TX_MSB_BYTE))
    {
        // Source address LSB of requesting pairing Note: rxBuf[5] and rxBuf[10] should be the same
        pairedAddressLSB = rxBuf[10];
        pairedStatus     = true;
        InitializeFuel   = true; // Set flag to initialize fuel value on next charging message
        DB_printf("\rValid message received in CommunicationService, PAIRED! Address: 0x%x\r\n",
                  pairedAddressLSB);
        ES_Event_t NewEvent;
        NewEvent.EventType = ES_PAIRED;
        PostPairedServoService(NewEvent);
    }
    else if ((pairedStatus == true) && (rxBuf[4] == DEST_ADD_TX_MSB_BYTE) &&
             (rxBuf[5] == pairedAddressLSB))
    {
        ES_Timer_InitTimer(UNPAIRING_TIMER, FOUR_SECONDS);
        if (InitializeFuel)
        {
            ChargeVal      = 200;   // Initialize fuel value on first message after pairing
            InitializeFuel = false; // Clear flag after initialization
        }
        if (rxBuf[8] == STATUS_CHARGING_BYTE)
        {
            // Increment charge value by 8 for each charging message received (each charging input equals 8 fuel)
            ChargeVal += 8;
            ES_Event_t NewEvent;
            NewEvent.EventType  = ES_CHARGING;
            NewEvent.EventParam = receivedByte;
            ES_PostAll(NewEvent);
        }
        else if (rxBuf[8] == STATUS_DRIVING_BYTE)
        {
            // Extract throttle and direction from message and post to DrivingService
            uint8_t Throttle  = rxBuf[9];  // Assuming throttle is in byte 9
            uint8_t Direction = rxBuf[10]; // Assuming direction is in byte 10
            uint8_t Digi      = rxBuf[11]; // Assuming digital input (e.g., shoot command) is in byte 11

            if (ChargeVal == 0)
            {
                Throttle  = JOY_MIDPOINT; // If out of charge, set throttle to neutral
                Direction = JOY_MIDPOINT; // If out of charge, set direction to neutral
            } else
            {
                if (((Throttle != JOY_MIDPOINT) || (Direction != JOY_MIDPOINT)) && (ChargeVal > 0))
                {
                    ChargeVal--; // Decrement charge value by 1 for each drive message received (each drive input equals 1 fuel)
                }

                if ((Digi == DIGI_SHOOT_BYTE) && (ChargeVal > 0))
                {
                    ChargeVal --; // Decrement charge value by 1 for shoot command
                    ES_Event_t ShootEvent;
                    ShootEvent.EventType = ES_CANNON_START;
                    PostBoatActionsService(ShootEvent);
                }
                if (Digi == DIGI_NO_SHOOT_BYTE)
                {
                    ES_Event_t NoShootEvent;
                    NoShootEvent.EventType = ES_CANNON_STOP;
                    PostBoatActionsService(NoShootEvent);
                }
            }

            uint16_t DriveParam = (Direction << 8) | Throttle; // Combine into single parameter
            ES_Event_t NewEvent;
            NewEvent.EventType  = ES_DRIVE;
            NewEvent.EventParam = DriveParam;
            PostDrivingService(NewEvent);


            // TODO: Check if rxBuf[11] has info to actuate something else
        }
    }
}

static void SendMsgToMallardModule(uint8_t charge)
{
    // Construct message frame
    txBuf[0] = START_BYTE;
    txBuf[1] = LENGTH_MSB_BYTE;
    txBuf[2] = LENGTH_TX_LSB_BYTE;
    txBuf[3] = API_ID_TX_BYTE;
    txBuf[4] = FRAME_ID_BYTE;
    txBuf[5] = DEST_ADD_TX_MSB_BYTE;
    txBuf[6] = pairedAddressLSB;
    txBuf[7] = OPT_BYTE;
    txBuf[8] = charge;
    ComputeCheckSum(DATA_FRAME_TX_LENGTH);
    txBuf[CHECKSUM_TX_INDEX] = CheckSumVal;

    #ifdef SHOW_CHARGE
    DB_printf("\r ChargeVal Sent: 0x%x", ChargeVal);
    #endif

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
    DB_printf("\r Sent Message bytes: \r\n");
    for (uint8_t i = 0; i < FRAME_SIZE_TX; i++)
    {
        DB_printf("0x%x ", txBuf[i]);
    }
    DB_printf("\r\n");
#endif
}

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/
