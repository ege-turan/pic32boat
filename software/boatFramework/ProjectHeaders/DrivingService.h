/****************************************************************************

  Header file for template service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef DrivingService_H
#define DrivingService_H

#include "ES_Types.h"

// Public Function Prototypes

bool InitDrivingService(uint8_t Priority);
bool PostDrivingService(ES_Event_t ThisEvent);
ES_Event_t RunDrivingService(ES_Event_t ThisEvent);

#endif /* DrivingService_H */

