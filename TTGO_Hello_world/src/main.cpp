#include <Arduino.h>
#include <WiFi.h>
#include <aREST.h>
#include <Station.h>


// WiFi parameters
const char* ssid = "WINNIE_THE_POOH";
const char* password = "dk7ll98jt9f2o0nuoiffppp9";

// Create an instance of the server
WiFiServer server(80);

// Create aREST instance
aREST rest = aREST();
Station staList[32];
//32 bit var to store events
long events = 0;

//Rest Station data 
int Id = 0;
String Barcode = "";
byte FormatoCollo = 0;
boolean PresenzaLogica = false;
byte Trigger = 0;
byte EchoTrigger = 0;
byte EventType = 0;




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



void
InitStations(){

  for(int i = 0; i < 100; i++){
      staList[i].SetId(i+1);
    }

}

int 
GetStatioToObserve(String command) {
  
  int index = command.toInt()-1;
  if(index < 0) return 0;
  Id = staList[index].GetId();

  Serial.println((String)"Get station data station: " + String(Id));

  Barcode = staList[index].GetBarcode();
  FormatoCollo = staList[index].GetFormatoCollo();
  PresenzaLogica = staList[index].GetPresenzaLogica();
  Trigger = staList[index].GetTrigger();
  EchoTrigger = staList[index].GetEchoTrigger();
  EventType = staList[index].GetEventType();
  
  return 1;
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
  rest.variable("EventType",&EventType);
  rest.variable("Events",&events);
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
BalanceTrigger(String command){
  
  String staId = getValue(command,'|',0);
  String trigger = getValue(command,'|',1);

  Serial.println("BalanceTrigger");
  Serial.println((String)"    Sta: " + staId);
  Serial.println((String)"    Trigger " + trigger);

  staList[staId.toInt()-1].BalanceTrigger(staId.toInt(),trigger.toInt());


  int staIndex = staId.toInt() - 1;
  //abbasso evento
  bitWrite(events,staIndex,0);
  return 1;

}


void 
MapRestFunctions(){
  
  //Imposta stazione da inviare 
  rest.function("GetStation",GetStatioToObserve);
  //Carica stazioni da file
  rest.function("SetBarcode",SetBarcode);
  //
  rest.function("BalanceTrigger",BalanceTrigger);

}

void
RandomStaEvent(){

  int staIndex = random(31);
  Serial.println((String)"Random event station:  " + String(staIndex));

  String randBarcode = String(random(10000000, 19999999));
  Serial.println((String)"    barcode " + randBarcode);

  staList[staIndex].SetBarcode(randBarcode);

  bitWrite(events,staIndex,1);
  Serial.println((String)"    set event bit" );

  staList[staIndex].IncreaseTrigger();
  Serial.println((String)"    increase trigger:  " + String(staList[staIndex].GetTrigger()));

  staList[staIndex].SetEventType(1);
  Serial.println((String)"    set event type:  " + String(1));

}

void 
setup()
{

  //Inizializza Serial COM
  Serial.begin(115200);

  InitWiFi();
  InitStations();
  MapRestVars();
  MapRestFunctions();
  

}
 
void loop() {
 
  WiFiClient client = server.available();
  if (client) {
 
    while(!client.available()){
      delay(5);
    }
    rest.handle(client);
  }

  if(!events){
    RandomStaEvent();
  }

}


