#ifndef DISPLAYUI_H
#define DISPLAYUI_H
#include <TFT_eSPI.h> 
#include <SPI.h>


class DisplayUI {
  private:
    TFT_eSPI tft = TFT_eSPI(135, 240); 
  public:
    DisplayUI();
    
    void DrawBarcode(String barcode);
    void DrawStation(String st);
    void DrawTrigger(String trg,String echoTrg);
    void DrawBox();
    void DrawPanel();
    void DrawIP(String ip);

    };

    #endif