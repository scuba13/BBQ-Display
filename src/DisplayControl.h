#ifndef DisplayControl_h
#define DisplayControl_h

#include <TFT_eSPI.h>
#include "MenuControl.h"  // Isso é necessário? Se não for, você pode remover


extern TFT_eSPI tft;



void showConnectingWiFiScreen();

void typewriterEffect(TFT_eSPI& tft, const String& text);

void welcomeScreen();

void showFailedWiFiScreen();

void displayInformation(SystemStatus& sysStat, RotaryEncoder& encoder, Bounce& encoderButton, TFT_eSPI& tft);

#endif
