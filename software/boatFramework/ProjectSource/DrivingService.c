/****************************************************************************
 Module
   DrivingService.c

 Revision
   1.0.1

 Description
   This file implements DrivingService as a simple service under the
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
#include "DrivingService.h"
#include "ES_Configure.h"
#include "ES_Framework.h"

#include "PIC32_PWM_Lib.h"
#include "dbprintf.h"
// #include <sys/attribs.h>
// #include "PIC32_IC_Lib.h"

/*----------------------------- Module Defines ----------------------------*/
// #define VERBOSE_MODE

#define DRIVING_MOTOR_TIMER _Timer2_
// Define the tris bits (input/output)
#define MOTOR_1_PWM_PIN_TRIS (TRISAbits.TRISA0)
#define MOTOR_2_PWM_PIN_TRIS (TRISAbits.TRISA1)

// Define the analog select for the pins which allow this
#define MOTOR_1_PWM_PIN_ANSEL (ANSELAbits.ANSA0)
#define MOTOR_2_PWM_PIN_ANSEL (ANSELAbits.ANSA1)

// Define pine names for PWM_Setup_MapChannelToOutputPin
#define Motor1PWMPinName PWM_RPA0
#define Motor2PWMPinName PWM_RPA1

#define MaxStepDutyCyclePercent 20 // max change in duty cycle per update for ramping

// TIMERS:
// This is the period of the PWM in timer ticks - for Timer2
// PBClock is 20 MHz, we want PRy to fit within the 16 bit timer register.
// PWM Values: Using prescaler 2.
// Formula: P_PWM   = (PRy + 1) * F_PBCLK * prescaler
//          1/F_PWM = (PRy + 1) * F_PBCLK * prescaler
//          PRy = (F_PBCLK / (F_PWM * prescaler)) - 1
#define F_PBCLK 20000000L // 20*10^6 Hz
#define prescaler 2
#define F_PWM_HZ 500         // Max is 500 Hz, chose 250Hz to be safe
#define P_PWM (1 / F_PWM_HZ) // in seconds
#define PWM_PERIOD_TICKS                                                                           \
    ((F_PBCLK / (F_PWM_HZ * prescaler)) - 1) // in ticks, 250Hz with prescaler 2 (Value for PRy)

#define MIN_PWM_DUTY_CYCLE_PERCENT 50
#define MAX_PWM_DUTY_CYCLE_PERCENT 100

// Used to initialise the PWM and also to select motor to command
typedef enum
{
    // Set to output compare channels 1 and 3
    Motor1ChannelOC = 1,
    Motor2ChannelOC = 2,
} MotorChannel_t;

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/
void _InitMotorPWM();
void _SetDutyCycleFromThrottleAndDirection(
    uint32_t Throttle,
    uint32_t Direction); // Duty Cycle note: 50 is reverse, 75 is stopped, 100 is forward
void _DriveMotor(MotorChannel_t MotorChannel, uint32_t DutyCyclePercent);
static float ClampFloat(float value, float min, float max);
static uint16_t RampDutyCycle(float received_duty_cycle,
                              uint16_t current_duty_cycle,
                              uint16_t max_step);

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;

static uint16_t CurrentThrottle =
    0; // value from 0 to 255 (8 bits, 0 to 127 negative for reverse, 128 to 255 positive for forward) maybe implement deadband
static uint16_t CurrentDirection         = 0; // value from 0 to 255
static uint16_t CurrentDutyCyclePercent1 = 0;
static uint16_t CurrentDutyCyclePercent2 = 0;

static uint16_t ThrottleResolution  = 255; // max value, 8 bits
static uint16_t DirectionResolution = 255; // max value, 8 bits
static uint16_t ThrottleMidPoint;
static uint16_t DirectionMidPoint;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitDrivingService

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
bool InitDrivingService(uint8_t Priority)
{
    ES_Event_t ThisEvent;

    MyPriority = Priority;
    /********************************************
   in here you write your initialization code
   *******************************************/
    // Announce initialisation of DrivingService
    clrScrn();
    DB_printf("\rStarting DrivingService: ");
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
     PostDrivingService

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
bool PostDrivingService(ES_Event_t ThisEvent)
{
    return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunDrivingService

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
ES_Event_t RunDrivingService(ES_Event_t ThisEvent)
{
    ES_Event_t ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

    switch (ThisEvent.EventType)
    {
        // This event is run once at the end of service initialisation
        case ES_INIT:
        {
            DB_printf("\rES_INIT received in DrivingService, priority: %d\r\n", MyPriority);
            // Initialize variables
            ThrottleMidPoint         = (ThrottleResolution)  / 2;
            DirectionMidPoint        = (DirectionResolution) / 2;
            CurrentThrottle          = ThrottleMidPoint;
            CurrentDirection         = DirectionMidPoint;
            CurrentDutyCyclePercent1 = 75;
            CurrentDutyCyclePercent2 = 75;

            // Initialize hardware
            _InitMotorPWM();
            _SetDutyCycleFromThrottleAndDirection(CurrentThrottle, CurrentDirection);
            _DriveMotor(Motor1ChannelOC, CurrentDutyCyclePercent1);
            _DriveMotor(Motor2ChannelOC, CurrentDutyCyclePercent2);
            // ES_Timer_InitTimer(DRIVING_TIMER, 6000);
        }
        break;

        case ES_CHARGING:
        {
            DB_printf("\rES_CHARGING event received in DrivingService, param: %d\r\n",
                      ThisEvent.EventParam);
            // Shut off all driving
            CurrentThrottle  = ThrottleMidPoint;
            CurrentDirection = DirectionMidPoint;
            _SetDutyCycleFromThrottleAndDirection(CurrentThrottle, CurrentDirection);
            _DriveMotor(Motor1ChannelOC, CurrentDutyCyclePercent1);
            _DriveMotor(Motor2ChannelOC, CurrentDutyCyclePercent2);
        }
        break;

        case ES_DRIVE:
        {
            // Extract throttle and direction from event parameter
            uint8_t Throttle  = ThisEvent.EventParam & 0xFF;        // lower byte
            uint8_t Direction = (ThisEvent.EventParam >> 8) & 0xFF; // upper byte
            DB_printf(
                "\rES_DRIVE event received in DrivingService, Throttle: %d, Direction: %d\r\n",
                Throttle,
                Direction);
            CurrentThrottle  = Throttle;
            CurrentDirection = Direction;
            _SetDutyCycleFromThrottleAndDirection(CurrentThrottle, CurrentDirection);
            DB_printf("\rThrottle: %d, Direction: %d, Duty cycle 1: %d, Duty cycle 2: %d\r\n",
                      CurrentThrottle,
                      CurrentDirection,
                      CurrentDutyCyclePercent1,
                      CurrentDutyCyclePercent2);
            _DriveMotor(Motor1ChannelOC, CurrentDutyCyclePercent1);
            _DriveMotor(Motor2ChannelOC, CurrentDutyCyclePercent2);
        }
        break;

        // unused
        case ES_TIMEOUT:
        {
            if (ThisEvent.EventParam == DRIVING_TIMER)
            {
                // DB_printf("\rDRIVING_TIMER expired in DrivingService\r\n");
                static uint8_t toggle = 0;
                if (!toggle)
                {
                    CurrentThrottle += 50;
                }
                else
                {
                    CurrentThrottle -= 50;
                }
                if (CurrentThrottle > ThrottleResolution)
                {
                    toggle = 1;
                    ; // wrap around to 0 after reaching max
                }
                if (CurrentThrottle < 55)
                {
                    toggle = 0; // wrap around to max after reaching min
                }

                _SetDutyCycleFromThrottleAndDirection(CurrentThrottle, CurrentDirection);
                _DriveMotor(Motor1ChannelOC, CurrentDutyCyclePercent1);
                _DriveMotor(Motor2ChannelOC, CurrentDutyCyclePercent2);
                ES_Timer_InitTimer(DRIVING_TIMER, 500);
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
void _InitMotorPWM()
{
    // Initialize Timer 2 for PWM
    PWM_Setup_ConfigureTimer(DRIVING_MOTOR_TIMER, PWM_PERIOD_TICKS + 1, PWM_PS_2);

    // Initialize OC1 for PWM1 output
    PWM_Setup_SetChannel(Motor1ChannelOC);
    PWM_Setup_AssignChannelToTimer(Motor1ChannelOC, _Timer2_);

    // Initialize OC2 for PWM2 output
    PWM_Setup_SetChannel(Motor2ChannelOC);
    PWM_Setup_AssignChannelToTimer(Motor2ChannelOC, _Timer2_);

    // Initialize pins to digital output
    MOTOR_1_PWM_PIN_ANSEL = 0;
    MOTOR_2_PWM_PIN_ANSEL = 0;
    MOTOR_1_PWM_PIN_TRIS  = 0;
    MOTOR_2_PWM_PIN_TRIS  = 0;

    // Assign output compares for both pwm channels to respective pins
    PWM_Setup_MapChannelToOutputPin(Motor1ChannelOC, Motor1PWMPinName);
    PWM_Setup_MapChannelToOutputPin(Motor2ChannelOC, Motor2PWMPinName);
}

void _SetDutyCycleFromThrottleAndDirection(uint32_t Throttle, uint32_t Direction)
{
    // Map the throttle value (0 to max value) to a duty cycle percentage (50 to 100)
    if (Throttle > ThrottleResolution)
    {
        Throttle = ThrottleResolution; // Cap throttle at max value
    }
    if (Direction > DirectionResolution)
    {
        Direction = DirectionResolution; // Cap direction at max value
    }

    const float BASE_DUTY = 75.0f; // neutral
    const float RANGE     = 25.0f; // max adjustment from neutral

    // Convert throttle to signed range: -25 to +25
    float throttleOffset =
        ((float)Throttle - (float)ThrottleMidPoint) * RANGE / (float)ThrottleMidPoint;

    // Convert direction to signed range: -25 to +25
    float directionOffset =
        ((float)Direction - (float)DirectionMidPoint) * RANGE / (float)DirectionMidPoint;

    // Differential steering mix
    float motor1 = BASE_DUTY + throttleOffset - directionOffset;
    float motor2 = BASE_DUTY + throttleOffset + directionOffset;

    // Clamp outputs
    motor1 = ClampFloat(motor1, 50.0f, 99.0f);
    motor2 = ClampFloat(motor2, 50.0f, 99.0f);

    CurrentDutyCyclePercent1 =
        RampDutyCycle(motor1, CurrentDutyCyclePercent1, MaxStepDutyCyclePercent);
    CurrentDutyCyclePercent2 =
        RampDutyCycle(motor2, CurrentDutyCyclePercent2, MaxStepDutyCyclePercent);
}

static float ClampFloat(float value, float min, float max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

static uint16_t RampDutyCycle(float received_duty_cycle,
                              uint16_t current_duty_cycle,
                              uint16_t max_step)
{
    if (received_duty_cycle > current_duty_cycle + max_step)
    {
        return current_duty_cycle + max_step;
    }

    if (received_duty_cycle < current_duty_cycle - max_step)
    {
        return current_duty_cycle - max_step;
    }

    return (uint16_t)received_duty_cycle;
}

void _DriveMotor(MotorChannel_t MotorChannel, uint32_t DutyCyclePercent)
{
    PWM_Operate_SetDutyOnChannel(DutyCyclePercent, MotorChannel);
}

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/
