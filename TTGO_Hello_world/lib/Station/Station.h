#ifndef STATION_H
#define STATION_H
#include <Arduino.h>



class Station {
  private:
    int _id;
    byte _trigger;
    byte _echoTrigger;
    String _barcode;
    byte _formatoCollo;
    boolean _presenzaLogica;
    
  public:
    Station();

    
    void SetId(int prog);
    void SetBarcode(String barcode);


    int GetId();
    byte GetTrigger();
    byte GetEchoTrigger();
    String GetBarcode();
    byte GetFormatoCollo();
    boolean GetPresenzaLogica();

    void Reset();
    
    

    };

    #endif