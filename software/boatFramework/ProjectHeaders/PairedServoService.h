/****************************************************************************

  Header file for template service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef ServPairedServo_H
#define ServPairedServo_H

#include "ES_Types.h"
#include "ES_Events.h"
// Public Function Prototypes

bool InitPairedServoService(uint8_t Priority);
bool PostPairedServoService(ES_Event_t ThisEvent);
ES_Event_t RunPairedServoService(ES_Event_t ThisEvent);

#endif /* ServPairedServo_H */

