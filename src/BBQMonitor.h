#ifndef BBQ_MONITOR_H
#define BBQ_MONITOR_H

#include <RotaryEncoder.h>
#include "SystemStatus.h"
#include "DisplayControl.h"
#include <Bounce2.h>



void monitor(SystemStatus& sysStat, TFT_eSPI& tft, Bounce& encoderButton, RotaryEncoder& encoder);
void loopDisplayController(SystemStatus& sysStat, bool bbqControl, RotaryEncoder& encoder, Bounce& encoderButton, TFT_eSPI& tft);
#endif  // BBQ_MONITOR_H
