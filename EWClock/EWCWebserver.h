#ifndef EWC_WEBSERVER_H
#define EWC_WEBSERVER_H

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FS.h>

class EWCWebserver
{
  public:
    EWCWebserver();
    ~EWCWebserver();
    void begin(int port);
    void handleGetData(void);
    void handle();
     
  private:
    ESP8266WebServer _webserver;
    //holds the current upload
    File _fsUploadFile;
    String _formatBytes(size_t bytes);
    String _getContentType(String filename);
    bool _handleFileRead(String path);
    void _handleFileUpload();
    void _handleFileDelete();
    void _handleFileCreate();
    void _handleFileList();
};

extern EWCWebserver Webserver;

#endif

