#include <Arduino.h>
#include <WiFi.h>
#include <aREST.h>
#include <Station.h>
#include "SPIFFS.h"

File file;

// WiFi parameters
const char* ssid = "WINNIE_THE_POOH";
const char* password = "dk7ll98jt9f2o0nuoiffppp9";

// Create an instance of the server
WiFiServer server(80);

// Create aREST instance
aREST rest = aREST();
Station staList[100];

//Rest Station data 
int Id = 0;
String Barcode = "";
byte FormatoCollo = 0;
boolean PresenzaLogica = false;
byte Trigger = 0;
byte EchoTrigger = 0;




//HELPER
 String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}


void
InitWiFi(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());
 
  server.begin();
}


int
LoadFromFile(String command){

  if(!SPIFFS.exists("/data.dat")) return 0;

  file = SPIFFS.open("/data.dat", FILE_READ);
 
  if (!file || !SPIFFS.exists("/data.dat")) {
    Serial.println("There was an error opening the file for writing");
    return 0;
  }

  file.read((uint8_t *)&staList, sizeof(staList));

  Serial.println("Status loaded");

  Serial.println("File size:");
  Serial.println(file.size());
  file.close();

  return 1;
}

int 
SaveToFile(String command){

  file = SPIFFS.open("/data.dat", FILE_WRITE);

  if (!file) {
    Serial.println("There was an error opening the file for writing");
    return 0;
  }

  if (file.write((const uint8_t *)&staList, sizeof(staList))) {
    Serial.println("File was written");
  } else {
    Serial.println("File write failed");
    return 0;
  }
 
  Serial.println("File size:");
  Serial.println(file.size());
  file.close();

  return 1;
}


void
InitStations(){

  if(!LoadFromFile("")){

    Serial.println("Station file not found, init stations.");

    for(int i = 0; i < 100; i++){
      staList[i].SetId(i+1);
    }

    SaveToFile("");
  }

  for(int i = 0; i < 100; i++){
      staList[i].SetId(i+1);
    }

    SaveToFile("");

}

int 
GetStatioToObserve(String command) {
  
  int index = command.toInt()-1;
  Id = staList[index].GetId();

  Serial.println((String)"Get station data station: " + String(Id));

  Barcode = staList[index].GetBarcode();
  FormatoCollo = staList[index].GetFormatoCollo();
  PresenzaLogica = staList[index].GetPresenzaLogica();
  Trigger = staList[index].GetTrigger();
  EchoTrigger = staList[index].GetEchoTrigger();
  
  return 0;
} 


void
MapRestVars(){
  //Mappiamo le varabili per la REST API
  rest.variable("Id",&Id);
  rest.variable("Barcode",&Barcode);
  rest.variable("FormatoCollo",&FormatoCollo);
  rest.variable("PresenzaLogica",&PresenzaLogica);
  rest.variable("Trigger",&Trigger);
  rest.variable("EchoTrigger",&EchoTrigger);
}

int 
formatEspData(String command){
  bool formatted = SPIFFS.format();
  if(formatted){
    Serial.println("\n\nSuccess formatting");
  }else{
    Serial.println("\n\nError formatting");
    return 0;
  }
  return 1;
}




int
SetBarcode(String command){
  
  String staId = getValue(command,'|',0);
  String barcode = getValue(command,'|',1);

  Serial.println("SetBarcode");
  Serial.println((String)"    Sta: " + staId);
  Serial.println((String)"    Barcode " + barcode);

  staList[staId.toInt()-1].SetBarcode(barcode);

  
  return 1;
}

int
ListAllFiles(String command){
 
  File root = SPIFFS.open("/");
 
  File f = root.openNextFile();
 
  while(f){
 
      Serial.print("FILE: ");
      Serial.println(f.name());
 
      f = root.openNextFile();
  }
 return 1;
}


void 
MapRestFunctions(){
  
  //Imposta stazione da inviare 
  rest.function("GetStation",GetStatioToObserve);
  //Carica stazioni da file
  rest.function("LoadFromFile",LoadFromFile);
  //Salva stazioni da file
  rest.function("SaveToFile",SaveToFile);
  //Formatta memoria
  rest.function("Format",formatEspData);
  //Stampa lista file su seriale
  rest.function("ListFile",ListAllFiles);
  //Carica stazioni da file
  rest.function("SetBarcode",SetBarcode);

}

void 
setup()
{

  //Inizializza Serial COM
  Serial.begin(115200);

  // Launch SPIFFS file system  
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
 
  InitStations();
  MapRestVars();
  MapRestFunctions();
  InitWiFi();

}
 
void loop() {
 
  WiFiClient client = server.available();
  if (client) {
 
    while(!client.available()){
      delay(5);
    }
    rest.handle(client);
  }
}


