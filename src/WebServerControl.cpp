#include "WebServerControl.h"
#include "TemperatureControl.h"


WebServerControl::WebServerControl(SystemStatus& systemStatus) 
  : _systemStatus(systemStatus) {}

void WebServerControl::begin() {
  _server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
    serveWebPage(request);
  });

  _server.on("/setTemp", HTTP_POST, [this](AsyncWebServerRequest *request){
    if (request->hasParam("temp", true)) { 
      _systemStatus.bbqTemperature = request->getParam("temp", true)->value().toInt();
      Serial.println("Temperatura definida recebida: " + String(_systemStatus.bbqTemperature));
    }
    request->redirect("/");
  });

  _server.on("/getTemp", HTTP_GET, [this](AsyncWebServerRequest *request){
    String jsonResponse = "{ \"currentTemp\": " + String(_systemStatus.calibratedTemp) + ", \"setTemp\": " + String(_systemStatus.bbqTemperature) + " }";
    request->send(200, "application/json", jsonResponse);
  });

_server.on("/getRelayState", HTTP_GET, [this](AsyncWebServerRequest *request){
    String jsonResponse = "{ \"relayState\": \"" + (_systemStatus.isRelayOn ? String("ON") : String("OFF")) + "\" }";
    request->send(200, "application/json", jsonResponse);
});



   _server.on("/resetSystem", HTTP_POST, [this](AsyncWebServerRequest *request){
    resetSystem(_systemStatus);
    Serial.println("Sistema resetado");
    request->redirect("/");
  });

  _server.on("/getAvgTemp", HTTP_GET, [this](AsyncWebServerRequest *request){
    String jsonResponse = "{ \"avgTemp\": " + String(_systemStatus.averageTemp) + " }";
    request->send(200, "application/json", jsonResponse);
});


  _server.begin();
}

void WebServerControl::serveWebPage(AsyncWebServerRequest *request) {

String html = "<!DOCTYPE html>"
            "<html>"
            "<head>"
            "<meta name='viewport' content='width=device-width, initial-scale=1'>"
            "<link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css'>"
            "<style>"
            "body { background-color: #f8f9fa; }"
            ".container { max-width: 600px; }"
            "input[type='number'] { max-width: 100px; }"
            "</style>"
            "</head>"
            "<body>"
            "<div class='container py-5'>"
            "<h1 class='display-4'>Web BBQ Monitor</h1>" // Alterado para display-4
            "<p id='currentTemp' class='my-3 h2'></p>"
            "<p id='setTemp' class='my-3 h2'></p>"
            "<p id='relayState' class='my-3 h2'></p>"
            "<p id='avgTemp' class='my-3 h2'></p>"
            "<form action='/setTemp' method='post' class='input-group my-4'>"
            "<input class='form-control' type='number' name='temp' min='30' max='200'>"
            "<div class='input-group-append'>"
            "<span class='input-group-text'>C</span>"
            "<button class='btn btn-dark ml-2' type='submit'>Set Temperature</button>"
            "</div>"
            "</form>"
            "<form action='/resetSystem' method='post'>"
            "<button class='btn btn-danger mt-2' type='submit'>Reset BBQ</button>"
            "</form>"
            "</div>"
            "<script>"
            "function updateTempsAndRelayState() {"
            "fetch('/getTemp')"
            ".then(response => response.json())"
            ".then(data => {"
            "document.getElementById('currentTemp').textContent = 'BBQ Temperature: ' + data.currentTemp + ' C';"
            "document.getElementById('setTemp').textContent = 'Set Temperature: ' + data.setTemp + ' C';"
            "});"
            "fetch('/getRelayState')"
            ".then(response => response.json())"
            ".then(data => {"
            "document.getElementById('relayState').textContent = 'Fire: ' + data.relayState;" 
            "});"
            "fetch('/getAvgTemp')"
            ".then(response => response.json())"
            ".then(data => {"
            "document.getElementById('avgTemp').textContent = 'Average Temp 3hs: ' + data.avgTemp + ' C';"
            "});"
            "}"
            "setInterval(updateTempsAndRelayState, 1000);"
            "updateTempsAndRelayState();"
            "</script>"
            "</body>"
            "</html>";


  request->send(200, "text/html", html.c_str());
}
