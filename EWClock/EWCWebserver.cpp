#include "EWCWebserver.h"
#include "EWCConfig.h"

EWCWebserver::EWCWebserver()
{

}

EWCWebserver::~EWCWebserver()
{
  
}


//format bytes
String EWCWebserver::_formatBytes(size_t bytes) 
{
  if (bytes < 1024){
    return String(bytes)+"B";
  } else if(bytes < (1024 * 1024)){
    return String(bytes/1024.0)+"KB";
  } else if(bytes < (1024 * 1024 * 1024)){
    return String(bytes/1024.0/1024.0)+"MB";
  } else {
    return String(bytes/1024.0/1024.0/1024.0)+"GB";
  }
}

String EWCWebserver::_getContentType(String filename)
{
  if(_webserver.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool EWCWebserver::_handleFileRead(String path) 
{
  Console.println("_handleFileRead: " + path);
  if(path.endsWith("/")) path += "index.htm";
  String contentType = _getContentType(path);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = _webserver.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void EWCWebserver::_handleFileUpload()
{
  if(_webserver.uri() != "/edit") return;
  HTTPUpload& upload = _webserver.upload();
  if(upload.status == UPLOAD_FILE_START){
    String filename = upload.filename;
    if(!filename.startsWith("/")) filename = "/"+filename;
    Console.print("_handleFileUpload Name: "); Console.println(filename);
    _fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if(upload.status == UPLOAD_FILE_WRITE){
    //Console.print("_handleFileUpload Data: "); Console.println(upload.currentSize);
    if(_fsUploadFile)
      _fsUploadFile.write(upload.buf, upload.currentSize);
  } else if(upload.status == UPLOAD_FILE_END){
    if(_fsUploadFile)
      _fsUploadFile.close();
    Console.print("_handleFileUpload Size: "); Console.println(upload.totalSize);
  }
}

void EWCWebserver::_handleFileDelete()
{
  if(_webserver.args() == 0) return _webserver.send(500, "text/plain", "BAD ARGS");
  String path = _webserver.arg(0);
  Console.println("_handleFileDelete: " + path);
  if(path == "/")
    return _webserver.send(500, "text/plain", "BAD PATH");
  if(!SPIFFS.exists(path))
    return _webserver.send(404, "text/plain", "FileNotFound");
  SPIFFS.remove(path);
  _webserver.send(200, "text/plain", "");
  path = String();
}

void EWCWebserver::_handleFileCreate()
{
  if(_webserver.args() == 0)
    return _webserver.send(500, "text/plain", "BAD ARGS");
  String path = _webserver.arg(0);
  Console.println("_handleFileCreate: " + path);
  if(path == "/")
    return _webserver.send(500, "text/plain", "BAD PATH");
  if(SPIFFS.exists(path))
    return _webserver.send(500, "text/plain", "FILE EXISTS");
  File file = SPIFFS.open(path, "w");
  if(file)
    file.close();
  else
    return _webserver.send(500, "text/plain", "CREATE FAILED");
  _webserver.send(200, "text/plain", "");
  path = String();
}

void EWCWebserver::_handleFileList()
{
  if(!_webserver.hasArg("dir")) {_webserver.send(500, "text/plain", "BAD ARGS"); return;}
  
  String path = _webserver.arg("dir");
  Console.println("_handleFileList: " + path);
  Dir dir = SPIFFS.openDir(path);
  path = String();

  String output = "[";
  while(dir.next()){
    File entry = dir.openFile("r");
    if (output != "[") output += ',';
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir)?"dir":"file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }
  
  output += "]";
  _webserver.send(200, "text/json", output);
}

void EWCWebserver::_handleDisplayCurrent()
{
  uint8_t dId = _display->getDisplay();

  RgbColor c = _display->getColor();

  String output = "{\"displayId\":";
  output += dId;
  output += ",";
  output += "\"color\":\"#";
  String chex = String(c.R, HEX);
  if (chex.length() == 1) {
    output += "0";
  }
  output += chex;
  chex = String(c.G, HEX);
  if (chex.length() == 1) {
    output += "0";
  }
  output += chex;
  chex = String(c.B, HEX);
  if (chex.length() == 1) {
    output += "0";
  }
  output += chex;
  output += "\"}";
  _webserver.send(200, "text/json", output);
}

void EWCWebserver::_handleDisplayList()
{
  DEBUG_MSG("begin");
  const char **l = _display->getDisplayList();
  String output = "[";
  int i = 0;
  while(l[i]) {
    if (output != "[") {
      output += ",";
    }
    output += "{\"id\":";
    output += i;
    output += ", \"name\":\"";
    output += l[i];
    output += "\"}";
    i++;
  }
  output = String("{\"displays\":") + output;
  output += "]}";
  _webserver.send(200, "text/json", output);
  DEBUG_MSG("output: " + output);
}

void EWCWebserver::_handleDisplayUpdate()
{
  DEBUG_MSG("begin");
  String buf = _webserver.arg(0);
  DEBUG_MSG("buf: " + buf);
  yield();
  StaticJsonBuffer<80> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf);
  if (!json.success()) {
    DEBUG_MSG("[Failed to parse input");
    _webserver.send(500, "text/json", "{\"error\":\"Failed to parse input\"}");
    return;
  }
  uint8_t displayId = json["id"];
  DEBUG_MSG("displayId: " + String(displayId));
  yield();
  _display->setDisplay(displayId);
  yield();
  _webserver.send(200, "text/json", "{}");
  DEBUG_MSG("end");
}

void EWCWebserver::_handleDisplayColorUpdate()
{
  DEBUG_MSG("begin");
  String buf = _webserver.arg(0);
  DEBUG_MSG("buf: " + buf);
  yield();
  StaticJsonBuffer<80> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf);
  if (!json.success()) {
    DEBUG_PRINTLN("[EWCWebserver::_handleDisplayColorUpdate] Failed to parse input");
    _webserver.send(500, "text/json", "{\"error\":\"Failed to parse input\"}");
    return;
  }
  const char* color = json["color"];
  uint32_t colorCode = strtol( &color[1], NULL, 16);
  DEBUG_MSG("colorCode: " + String(colorCode));
  yield();
  _display->setColor(colorCode >> 16, colorCode >> 8 & 0xFF, colorCode & 0xFF);
  yield();
  _webserver.send(200, "text/json", "{}");
  DEBUG_MSG("end");
}

void EWCWebserver::_handleConfigDisplay()
{
  uint8_t dId = 6;
  RgbColor c = _display->getColor();
    
  String output = "{\"defaultDisplayID\":";
  output += dId;
  output += ",";
  output += "\"defaultColor\":\"#";
  String chex = String(c.R, HEX);
  if (chex.length() == 1) {
    output += "0";
  }
  output += chex;
  chex = String(c.G, HEX);
  if (chex.length() == 1) {
    output += "0";
  }
  output += chex;
  chex = String(c.B, HEX);
  if (chex.length() == 1) {
    output += "0";
  }
  output += chex;
  output += "\"}";
  _webserver.send(200, "text/json", output);
}

void EWCWebserver::begin(int port, EWCDisplay &display)
{
  SPIFFS.begin();
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {    
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Console.printf("FS File: %s, size: %s\n", fileName.c_str(), _formatBytes(fileSize).c_str());
    }
    Console.printf("\n");
  }
  _webserver = ESP8266WebServer(port);
  _display = &display;

  //list directory
  _webserver.on("/list", HTTP_GET, std::bind(&EWCWebserver::_handleFileList, this));
  //load editor
  _webserver.on("/edit", HTTP_GET, [&](){
    if(!_handleFileRead("/edit.htm")) _webserver.send(404, "text/plain", "FileNotFound");
  });
  //create file
  _webserver.on("/edit", HTTP_PUT, std::bind(&EWCWebserver::_handleFileCreate, this));
  //delete file
  _webserver.on("/edit", HTTP_DELETE, std::bind(&EWCWebserver::_handleFileDelete, this));
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  _webserver.on("/edit", HTTP_POST, [&](){ _webserver.send(200, "text/plain", ""); }, std::bind(&EWCWebserver::_handleFileUpload, this));

  //called when the url is not defined here
  //use it to load content from SPIFFS
  _webserver.onNotFound([&](){
    if(!_handleFileRead(_webserver.uri()))
      _webserver.send(404, "text/plain", "FileNotFound");
  });

  _webserver.on("/api/display/current", HTTP_GET, std::bind(&EWCWebserver::_handleDisplayCurrent, this));
  _webserver.on("/api/display/list", HTTP_GET, std::bind(&EWCWebserver::_handleDisplayList, this));
  _webserver.on("/api/display", HTTP_PUT, std::bind(&EWCWebserver::_handleDisplayUpdate, this));
  _webserver.on("/api/display/color", HTTP_PUT, std::bind(&EWCWebserver::_handleDisplayColorUpdate, this));
  _webserver.on("/api/config/display", HTTP_GET, std::bind(&EWCWebserver::_handleConfigDisplay, this));
    
  //get heap status, analog input value and all GPIO statuses in one json call
  _webserver.on("/all", HTTP_GET, [&](){
    String json = "{";
    json += "\"heap\":"+String(ESP.getFreeHeap());
    json += ", \"analog\":"+String(analogRead(A0));
    json += ", \"gpio\":"+String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16)));
    json += "}";
    _webserver.send(200, "text/json", json);
    json = String();
  });

  //_webserver.on("/data", HTTP_GET, std::bind(&EWCWebserver::handleGetData, this));
  _webserver.begin();
}

/*
void EWCWebserver::handleGetData()
{
  _webserver.send(200, "text/plain", "xxx");
}
*/
void EWCWebserver::handle()
{
  _webserver.handleClient();
}

EWCWebserver Webserver = EWCWebserver();
