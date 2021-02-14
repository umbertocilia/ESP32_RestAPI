#include <Arduino.h>
#include <WiFi.h>
#include <aREST.h>
#include <Station.h>
#include <TFT_eSPI.h> 
#include <SPI.h>


#define BLACK_SPOT

// Switch position and size
#define FRAME_X 0
#define FRAME_Y 0
#define FRAME_W 220
#define FRAME_H 110

// Red zone size
#define REDBUTTON_X FRAME_X
#define REDBUTTON_Y FRAME_Y
#define REDBUTTON_W (FRAME_W/2)
#define REDBUTTON_H FRAME_H

// Green zone size
#define GREENBUTTON_X (REDBUTTON_X + REDBUTTON_W)
#define GREENBUTTON_Y FRAME_Y
#define GREENBUTTON_W (FRAME_W/2)
#define GREENBUTTON_H FRAME_H




// WiFi parameters
const char* ssid = "WINNIE_THE_POOH";
const char* password = "dk7ll98jt9f2o0nuoiffppp9";

// Create an instance of the server
WiFiServer server(80);

// Create aREST instance
aREST rest = aREST();
Station staList[32];
TFT_eSPI tft = TFT_eSPI(135, 240); 

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


void drawBarcode(String barcode){
  tft.fillRect(130,10,100,20,TFT_BLACK);
  tft.setTextColor(TFT_WHITE);

  tft.setTextSize(1);
  tft.setCursor(130,10);      
  tft.print("BARCODE");

  tft.setTextSize(2);
  tft.setCursor(130,25);      
  tft.print(barcode);
}


void drawStation(String st){
  tft.drawRect(6,6,70,38,TFT_WHITE);
  tft.fillRect(10,10,60,30,TFT_BLACK);
  tft.setTextColor(TFT_WHITE);

  tft.setTextSize(1);
  tft.setCursor(15,10);      
  tft.print("STATION");

  tft.setTextSize(2);
  tft.setCursor(15,25);      
  tft.print(st);
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

   
  drawBarcode(randBarcode);
  drawStation(String(staIndex+1));

}


//! Long time delay, it is recommended to use shallow sleep, which can effectively reduce the current consumption
void espDelay(int ms)
{   
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH,ESP_PD_OPTION_ON);
    esp_light_sleep_start();
}



void drawFrame()
{
  tft.drawRect(FRAME_X, FRAME_Y, FRAME_W, FRAME_H, TFT_COLMOD);
}

void drawBox()
{
  tft.fillRect(90,10,30,30,TFT_ORANGE);
  tft.fillRect(90,10,15,30,TFT_YELLOW);
  tft.drawRect(86,6,38,38,TFT_WHITE);
}


void drawPanel()
{
  //tft.fillRect(10,60,195,95,TFT_CYAN);
  tft.drawRect(6,50,225,80,TFT_WHITE);
}



void 
setup()
{

  //Inizializza Serial COM
  Serial.begin(115200);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0, 0);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(1);

  if (TFT_BL > 0) { // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    pinMode(TFT_BL, OUTPUT); // Set backlight pin to output mode
    digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
  }

  tft.setSwapBytes(true);
  //espDelay(5000);

  InitWiFi();
  InitStations();
  MapRestVars();
  MapRestFunctions();
  
  drawBox();
  drawPanel();
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


