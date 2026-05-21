/****************************************************************************

  Header file for template service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef ServFuelServo_H
#define ServFuelServo_H

#include "ES_Types.h"
#include "ES_Events.h"
// Public Function Prototypes

bool InitFuelServoService(uint8_t Priority);
bool PostFuelServoService(ES_Event_t ThisEvent);
ES_Event_t RunFuelServoService(ES_Event_t ThisEvent);

#endif /* ServFuelServo_H */

