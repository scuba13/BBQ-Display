#include <WiFiManager.h>
#include <RotaryEncoder.h>
#include "SystemStatus.h"
#include "BBQMonitor.h"
#include "PinDefinitions.h"
#include "WebServerControl.h"
#include "TemperatureControl.h"
#include <Arduino.h>
#include "DisplayControl.h"


RotaryEncoder encoder(ENCODER_PIN1, ENCODER_PIN2, RotaryEncoder::LatchMode::TWO03);
Bounce encoderButton = Bounce();
SystemStatus sysStat;
WebServerControl webServerControl(sysStat);

void getCalibratedTempTask(void * parameter) {
  while(true) {
    getCalibratedTemp(thermocouple, sysStat);
    vTaskDelay(pdMS_TO_TICKS(200)); // A cada 200ms (ou o intervalo que você desejar).
  }
}

void getCalibratedTempPTask(void * parameter) {
  while(true) {
    getCalibratedTempP(thermocoupleP, sysStat);
    vTaskDelay(pdMS_TO_TICKS(200)); // A cada 200ms (ou o intervalo que você desejar).
  }
}

void setup() {
    tft.begin();
    tft.setRotation(1); // Defina a rotação do display conforme necessário

    encoder.setPosition(1);
    Serial.begin(115200);

    showConnectingWiFiScreen();

    WiFiManager wifiManager;
    if(wifiManager.autoConnect("BBQController")) {
        Serial.println("Conexão WiFi estabelecida!");
        Serial.println("O IP da ESP32 é: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("Falha na conexão WiFi");
        showFailedWiFiScreen();
    }

    encoderButton.attach(ENCODER_BUTTON, INPUT_PULLUP);
    encoderButton.interval(5);
    pinMode(RELAY_PIN, OUTPUT);
    webServerControl.begin();

// Criando uma nova tarefa usando a função xTaskCreatePinnedToCore do FreeRTOS. Coleta de Temperatura e Calibração em nucleo separado.

xTaskCreatePinnedToCore(
        getCalibratedTempTask,     // Função que será executada pela tarefa. Esta função irá obter e calibrar a temperatura.
        "TempTask",                // Nome da tarefa (útil para fins de depuração).
        2000,                      // Tamanho da pilha da tarefa. Reserva espaço para 2000 entradas.
        NULL,                      // Parâmetros que são passados para a função da tarefa. No caso, nenhum parâmetro é passado.
        1,                         // Prioridade da tarefa. Aqui, a tarefa tem uma prioridade de 1.
        NULL,                      // Pode armazenar o identificador da tarefa, mas não estamos armazenando aqui.
        0                          // Núcleo em que a tarefa será executada. Neste caso, o núcleo 0.
    );
xTaskCreatePinnedToCore(
        getCalibratedTempPTask,     // Função que será executada pela tarefa. Esta função irá obter e calibrar a temperatura.
        "TempPTask",                // Nome da tarefa (útil para fins de depuração).
        2000,                       // Tamanho da pilha da tarefa. Reserva espaço para 2000 entradas.
        NULL,                      // Parâmetros que são passados para a função da tarefa. No caso, nenhum parâmetro é passado.
        1,                         // Prioridade da tarefa. Aqui, a tarefa tem uma prioridade de 1.
        NULL,                      // Pode armazenar o identificador da tarefa, mas não estamos armazenando aqui.
        0                          // Núcleo em que a tarefa será executada. Neste caso, o núcleo 0.
    );


    welcomeScreen();
}

void loop() {
  loopDisplayController(sysStat, true, encoder, encoderButton, tft);
}

