#ifndef WebServerControl_h
#define WebServerControl_h

#include <ESPAsyncWebServer.h>
#include "SystemStatus.h"


class WebServerControl {
public:
  WebServerControl(SystemStatus& systemStatus);
  void begin();
  void serveWebPage(AsyncWebServerRequest *request);
private:
  SystemStatus& _systemStatus;
  AsyncWebServer _server{80};
  
};

#endif
