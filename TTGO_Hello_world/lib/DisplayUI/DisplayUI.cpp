#include "DisplayUI.h"
   
DisplayUI::DisplayUI() {
  this->tft.init();
  this->tft.setRotation(1);
  this->tft.fillScreen(TFT_BLACK);
  this->tft.setTextSize(2);
  this->tft.setTextColor(TFT_WHITE);
  this->tft.setCursor(0, 0);
  this->tft.setTextDatum(MC_DATUM);
  this->tft.setTextSize(1);

  if (TFT_BL > 0) { // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    pinMode(TFT_BL, OUTPUT); // Set backlight pin to output mode
    digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
  }
  this->tft.setSwapBytes(true);
  }
    
void 
DisplayUI::DrawBarcode(String barcode){
  this->tft.fillRect(130,10,100,40,TFT_BLACK);
  this->tft.setTextColor(TFT_WHITE);

  this->tft.setTextSize(1);
  this->tft.setCursor(130,10);      
  this->tft.print("BARCODE");

  this->tft.setTextSize(2);
  this->tft.setCursor(130,25);      
  this->tft.print(barcode);
}

void 
DisplayUI::DrawIP(String ip){
  this->tft.drawRect(135,115,92,13,TFT_BLACK);
  this->tft.setTextColor(TFT_WHITE);
  this->tft.setTextSize(1);
  this->tft.setCursor(140,118);      
  this->tft.print(ip);
}

void
DisplayUI::DrawTrigger(String trg,String echoTrg){
  this->tft.fillRect(15,60,60,30,TFT_BLACK);
  this->tft.setTextColor(TFT_GREENYELLOW);

  this->tft.setTextSize(1);
  this->tft.setCursor(15,60);      
  this->tft.print("TRIGGER");

  this->tft.setTextColor(TFT_WHITE);
  this->tft.setTextSize(2);
  this->tft.setCursor(15,70);      
  this->tft.print(trg);

  this->tft.fillRect(15,90,60,35,TFT_BLACK);
  this->tft.setTextColor(TFT_GREENYELLOW);

  this->tft.setTextSize(1);
  this->tft.setCursor(15,100);      
  this->tft.print("ECHOTRG");

  this->tft.setTextColor(TFT_WHITE);
  this->tft.setTextSize(2);
  this->tft.setCursor(15,110);      
  this->tft.print(echoTrg);



}

void 
DisplayUI::DrawStation(String st){
  this->tft.drawRect(6,6,70,38,TFT_WHITE);
  this->tft.fillRect(10,10,60,30,TFT_BLACK);
  this->tft.setTextColor(TFT_WHITE);

  this->tft.setTextSize(1);
  this->tft.setCursor(15,10);      
  this->tft.print("STATION");

  this->tft.setTextSize(2);
  this->tft.setCursor(15,25);      
  this->tft.print(st);
}

void 
DisplayUI::DrawBox()
{
  this->tft.fillRect(90,10,30,30,TFT_ORANGE);
  this->tft.fillRect(90,10,15,30,TFT_YELLOW);
  this->tft.drawRect(86,6,38,38,TFT_WHITE);
}

void 
DisplayUI::DrawPanel()
{
  //tft.fillRect(10,60,195,95,TFT_CYAN);
  this->tft.drawRect(6,50,225,80,TFT_WHITE);
} 
