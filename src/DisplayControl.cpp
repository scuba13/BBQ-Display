#include <DisplayControl.h>
#include <WiFi.h>
#include <logo.h>

// Constantes
const int WELCOME_SCREEN_DELAY = 2000;
const int TYPEWRITER_EFFECT_DELAY = 100;
const String APP_VERSION = "0.1.1";
const String AUTHOR_NAME = "Eduardo Nascimento";

TFT_eSPI tft = TFT_eSPI();

// Funções auxiliares
void centerText(TFT_eSPI& tft, const String& text, int y) {
    int textWidth = tft.textWidth(text);
    tft.setCursor((tft.width() - textWidth) / 2, y);
}

void showScreenWithInstructions(const String& title, const String& instruction, uint16_t titleColor, uint16_t instructionColor) {
    tft.fillScreen(TFT_BLACK); // Limpa a tela
    tft.setTextSize(2);

    // Título
    tft.setTextColor(titleColor);
    centerText(tft, title, tft.height() / 2 - 30);
    tft.println(title);

    // Instrução
    tft.setTextColor(instructionColor);
    centerText(tft, instruction, tft.height() / 2);
    tft.println(instruction);
}

void showConnectingWiFiScreen() {
    showScreenWithInstructions("Conectando ao Wi-Fi...", "", TFT_DARKGREY, TFT_DARKGREY);
}

void showFailedWiFiScreen() {
    showScreenWithInstructions("Falha na Conexão WiFi", "Conecte-se à rede 'BBQController' para Setup", TFT_RED, TFT_WHITE);
}

void typewriterEffect(TFT_eSPI& tft, const String& text) {
    int x = tft.getCursorX();
    int y = tft.getCursorY();
    for (int i = 0; i < text.length(); i++) {
        tft.print(text.charAt(i));
        delay(TYPEWRITER_EFFECT_DELAY);
    }
}



void welcomeScreen() {
    tft.fillScreen(TFT_BLACK); // Limpa a tela
   

    // Exibe a imagem
    int imageWidth = 170;
    int imageHeight = 170;
    int displayWidth = tft.width();
    int displayHeight = tft.height();
    int x = (displayWidth - imageWidth) / 2;  
    int y = (displayHeight - imageHeight) / 2; 
    tft.pushImage(x, y, imageWidth, imageHeight, artmill);
    delay(WELCOME_SCREEN_DELAY);

    tft.fillScreen(TFT_BLACK); // Limpa a tela

    int lineHeight = tft.height() / 4;

    // Linhas
    tft.setTextSize(3);
    centerText(tft, "Artmill", (lineHeight - 36) * 0.5);
    tft.println("Artmill");

    tft.setTextSize(2);
    
    centerText(tft, "BBQ Controller", lineHeight + (lineHeight - 16) * 0.5);
    tft.println("BBQ Controller");

    centerText(tft, "Vamos Defumar o Mundo!", 2 * lineHeight + (lineHeight - 16) * 0.5);
    typewriterEffect(tft, "Vamos Defumar o Mundo!");


    if (WiFi.status() == WL_CONNECTED) {
        String ipText = "IP: " + WiFi.localIP().toString();
        centerText(tft, ipText, 3 * lineHeight + (lineHeight - 16) * 0.5);
        tft.println(ipText);
    }

    delay(WELCOME_SCREEN_DELAY);
}

void displayInformation(SystemStatus& sysStat, RotaryEncoder& encoder, Bounce& encoderButton, TFT_eSPI& tft) {
    tft.fillScreen(TFT_WHITE);  // Limpa a tela
    tft.setTextColor(TFT_BLACK); // Cor do texto

    // IP
    tft.setCursor(10, 20);
    tft.print("IP: ");
    tft.setCursor(100, 20);
    tft.print(WiFi.localIP().toString());

    // Versão
    tft.setCursor(10, 50);
    tft.print("Ver: ");
    tft.setCursor(100, 50);
    tft.print(APP_VERSION);

    // Autor
    tft.setCursor(10, 80);
    tft.print("Author: ");
    tft.setCursor(100, 80);
    tft.print(AUTHOR_NAME);

    // Aguarda botão ser pressionado
    unsigned long startTime = millis();
    while (true) {
        if (encoderButton.update() && encoderButton.fell()) {
            delay(200);
            // Aqui, suponho que a função 'returnMenu' esteja definida em algum lugar do seu projeto.
            returnMenu(sysStat, encoder, encoderButton, tft); 
            break;
        }

        // Timeout de 10 segundos (ajuste conforme necessário)
        if (millis() - startTime > 10000) {
            break;
        }
    }
}
