#include "BBQMonitor.h"
#include "TemperatureControl.h"
#include "MenuControl.h"
#include "BBQCure.h"
#include <Arduino.h>

void loopDisplayController(SystemStatus& sysStat, bool bbqControl, RotaryEncoder& encoder, Bounce& encoderButton, TFT_eSPI& tft) {
  if (sysStat.bbqTemperature > 0) {
    controlTemperature(sysStat);
  }

  if (sysStat.monitorMode) {
    monitor(sysStat, tft, encoderButton, encoder);
  } else if (sysStat.cureProcessMode) {
    bbqCureProcess(sysStat, tft, encoderButton, encoder);
  } else {
    displayMenu(sysStat, encoder, encoderButton, tft);
  }
}


const int16_t COLOR_BACKGROUND = TFT_BLACK;
const int16_t COLOR_TEXT = TFT_WHITE;
const int16_t COLOR_TEXT_INACTIVE = TFT_DARKGREY;
const int16_t COLOR_WARNING = TFT_RED;
const int16_t COLOR_OK = TFT_GREEN;
const int16_t COLOR_INFO = TFT_BLUE;

// Função para inicializar e desenhar as descrições
void initializeMonitorScreen(TFT_eSPI& tft) {
    tft.fillScreen(COLOR_BACKGROUND);
    tft.setTextSize(3);
    tft.setTextColor(COLOR_TEXT);

    tft.setCursor(10, 20);
    tft.print("BBQ Temp: ");
        
    tft.setCursor(10, 50);
    tft.print("Set Temp: ");
        
    tft.setCursor(10, 80);
    tft.print("Fire: ");
        
    tft.setCursor(10, 110);
    tft.print("AVG 3H: ");
}

// Função auxiliar para definir a cor baseada na comparação com Set Temp
auto setTempColor = [](TFT_eSPI& tft, int temp, int setTemp) {
    if (setTemp <= 0) {
        tft.setTextColor(COLOR_TEXT_INACTIVE);
        return;
    }
    if (temp < setTemp) {
        tft.setTextColor(COLOR_INFO);
    } else if (temp == setTemp) {
        tft.setTextColor(COLOR_OK);
    } else {
        tft.setTextColor(COLOR_WARNING);
    }
};


void drawCenteredText(TFT_eSPI& tft, int x, int y, int width, const char* text) {
    int16_t textWidth = tft.textWidth(text);
    int16_t textHeight = tft.fontHeight();
    int16_t xOffset = (width - textWidth) / 2;
    tft.setCursor(x + xOffset, y);
    tft.print(text);
}

void updateMonitorValues(SystemStatus& sysStat, TFT_eSPI& tft) {
    char tempBuffer[10];  // buffer to hold temperature string
    tft.setTextSize(3);
    int rectWidth = tft.width() - 180;
    int rectHeight = tft.fontHeight() * 3; // Considerando o tamanho 3 da fonte

    // Atualizar valor da BBQ Temp e sua cor
    snprintf(tempBuffer, sizeof(tempBuffer), "%dC", sysStat.calibratedTemp);
    tft.fillRect(180, 20, rectWidth, rectHeight, COLOR_BACKGROUND);
    setTempColor(tft, sysStat.calibratedTemp, sysStat.bbqTemperature);
    drawCenteredText(tft, 180, 20, rectWidth, tempBuffer);

    // Atualizar valor da Set Temp
    snprintf(tempBuffer, sizeof(tempBuffer), "%dC", sysStat.bbqTemperature);
    tft.fillRect(180, 50, rectWidth, rectHeight, COLOR_BACKGROUND);
    tft.setTextColor(COLOR_TEXT);
    drawCenteredText(tft, 180, 50, rectWidth, tempBuffer);

    // Atualizar status do fogo
    tft.fillRect(180, 80, rectWidth, rectHeight, COLOR_BACKGROUND);
    tft.setTextColor(sysStat.isRelayOn ? COLOR_WARNING : COLOR_TEXT_INACTIVE);
    drawCenteredText(tft, 180, 80, rectWidth, sysStat.isRelayOn ? "ON" : "OFF");

    // Atualizar valor da média de 3 horas e sua cor
    snprintf(tempBuffer, sizeof(tempBuffer), "%dC", sysStat.averageTemp);
    tft.fillRect(180, 110, rectWidth, rectHeight, COLOR_BACKGROUND);
    if (sysStat.averageTemp > 0) {
        setTempColor(tft, sysStat.averageTemp, sysStat.bbqTemperature);
        drawCenteredText(tft, 180, 110, rectWidth, tempBuffer);
    } else {
        tft.setTextColor(COLOR_TEXT_INACTIVE);
        drawCenteredText(tft, 180, 110, rectWidth, "--");
    }
}

void monitor(SystemStatus& sysStat, TFT_eSPI& tft, Bounce& encoderButton, RotaryEncoder& encoder) {
    static bool isMonitorInitialized = false; // Flag para verificar se a tela foi inicializada

    if (!isMonitorInitialized) {
        initializeMonitorScreen(tft);
        isMonitorInitialized = true;
    }

    if (sysStat.lastTempUpdateMonitor == 0 || millis() - sysStat.lastTempUpdateMonitor >= 1000) {
        sysStat.lastTempUpdateMonitor = millis();
        updateMonitorValues(sysStat, tft);
    }

    if (encoderButton.update() && encoderButton.fell()) {
        delay(200);
        tft.setCursor(10, 140);
        tft.setTextColor(COLOR_TEXT_INACTIVE);
        tft.print("Return Menu            ");
        delay(500);
        sysStat.monitorMode = false;
        isMonitorInitialized = false;
        returnMenu(sysStat, encoder, encoderButton, tft);
    }
}
