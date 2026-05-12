/****************************************************************************

  Header file for template service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef ServComms_H
#define ServComms_H

#include "ES_Types.h"
#include "ES_Events.h"
// Public Function Prototypes

bool InitCommunicationService(uint8_t Priority);
bool PostCommunicationService(ES_Event_t ThisEvent);
ES_Event_t RunCommunicationService(ES_Event_t ThisEvent);

#endif /* ServComms_H */

