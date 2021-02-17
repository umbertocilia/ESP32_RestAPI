#include <Arduino.h>
#include <WiFi.h>
#include <aREST.h>
#include <Station.h>
#include <WiFiMulti.h>
#include <DisplayUI.h>
#include "esp_adc_cal.h"


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

#define ADC_PIN         34
int vref = 1100;

DisplayUI GUI;

WiFiMulti wifiMulti;


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

void showVoltage()
{
    static uint64_t timeStamp = 0;
    if (millis() - timeStamp > 3000) {
        timeStamp = millis();
        uint16_t v = analogRead(ADC_PIN);
        float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
        String voltage = "Volt:" + String(battery_voltage) + "V";
        Serial.println(voltage);
    }

}


void
InitWiFi(){
  wifiMulti.addAP("whyphy", "esp82666");

  Serial.println("Connecting Wifi...");
  if(wifiMulti.run() == WL_CONNECTED) {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
    }
  
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

  GUI.DrawTrigger(String(staList[staId.toInt()-1].GetTrigger()) , String(staList[staId.toInt()-1].GetEchoTrigger()));

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

  
  GUI.DrawStation(String(staIndex+1));
  GUI.DrawBarcode(randBarcode);
  GUI.DrawTrigger(String(staList[staIndex].GetTrigger()) , String(staList[staIndex].GetEchoTrigger()));
  


}



void 
setup()
{

  //Inizializza Serial COM
  Serial.begin(115200);

  InitWiFi();

  server.begin();

  InitStations();
  MapRestVars();
  MapRestFunctions();



  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize((adc_unit_t)ADC_UNIT_1, (adc_atten_t)ADC1_CHANNEL_6, (adc_bits_width_t)ADC_WIDTH_BIT_12, 1100, &adc_chars);
  //Check type of calibration value used to characterize ADC
  if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
      Serial.printf("eFuse Vref:%u mV", adc_chars.vref);
      vref = adc_chars.vref;
  } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        Serial.printf("Two Point --> coeff_a:%umV coeff_b:%umV\n", adc_chars.coeff_a, adc_chars.coeff_b);
  } else {
        Serial.println("Default Vref: 1100mV");
  }
  
 
}
 
void loop() {
  
  if(wifiMulti.run() != WL_CONNECTED) {
    Serial.println("WiFi not connected!");
    delay(1000);
  }else{
    WiFiClient client = server.available();
    if (client) {
      while(!client.available()){
      delay(5);
    }
      rest.handle(client);
    }

    if(!events){
      RandomStaEvent();
      
      GUI.DrawBox();
      GUI.DrawPanel();
      GUI.DrawIP(WiFi.localIP().toString().c_str());
    }
  }

}


