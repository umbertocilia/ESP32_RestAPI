#include "Station.h"
  
Station::Station() {
}

void 
Station::SetBarcode(String barcode){
  this->_barcode = barcode;
}

void 
Station::SetId(int id){
  this->_id = id;
}

void 
Station::SetEventType(byte type){
  this->_eventType = type;
}

int
Station::GetId(){
  return this->_id;
}
  
byte 
Station::GetTrigger(){
  return this->_trigger;
}

byte 
Station::GetEchoTrigger(){
  return this->_echoTrigger;
}

byte
Station::GetEventType(){
  return this->_eventType;
}
    
String 
Station::GetBarcode(){
  return this->_barcode;
}

byte 
Station::GetFormatoCollo(){
 return this->_formatoCollo;
}

boolean 
Station::GetPresenzaLogica(){
 return  this->_presenzaLogica;
}
    
void 
Station::Reset(){
  this->_id= 0;
  this->_barcode = "XXXXXXXXXXXXXXXXXXXX";
  this->_eventType = 0;
  this->_formatoCollo = 0;
  this->_presenzaLogica = false;
  this->_trigger = 0;
  this->_echoTrigger = 0;
}

void
Station::IncreaseTrigger(){
  this->_trigger += 1;
}

void 
Station::BalanceTrigger(int id, int trig){

  if(this->_trigger = trig){
    this->_echoTrigger = trig;
  }

}

