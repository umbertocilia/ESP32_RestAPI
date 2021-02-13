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
  this->_formatoCollo = 0;
  this->_presenzaLogica = false;
  this->_trigger = 0;
  this->_echoTrigger = 0;
}

