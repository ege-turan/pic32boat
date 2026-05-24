/****************************************************************************

  Header file for template service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef ServBoatActions_H
#define ServBoatActions_H

#pragma once

#include "ES_Events.h"
#include "ES_Types.h"

// Public Function Prototypes

bool InitBoatActionsService(uint8_t Priority);
bool PostBoatActionsService(ES_Event_t ThisEvent);
ES_Event_t RunBoatActionsService(ES_Event_t ThisEvent);

#endif /* ServBoatActions_H */
