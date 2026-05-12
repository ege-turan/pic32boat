/****************************************************************************

  Header file for MallardCommunicationService
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef MallardCommunicationService_H
#define MallardCommunicationService_H

#pragma once

#include "ES_Events.h"
#include "ES_Types.h"

// Public Function Prototypes

bool InitMallardCommunicationService(uint8_t Priority);
bool PostMallardCommunicationService(ES_Event_t ThisEvent);
ES_Event_t RunMallardCommunicationService(ES_Event_t ThisEvent);

#endif /* MallardCommunicationService_H */
