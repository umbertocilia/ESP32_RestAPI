#ifndef STATION_H
#define STATION_H
#include <Arduino.h>



class Station {
  private:
    int _id;
    byte _trigger;
    byte _echoTrigger;
    byte _eventType;
    String _barcode;
    byte _formatoCollo;
    boolean _presenzaLogica;
    
  public:
    Station();

    
    void SetId(int prog);
    void SetBarcode(String barcode);
    void SetEventType(byte type);


    int GetId();
    byte GetTrigger();
    byte GetEchoTrigger();
    String GetBarcode();
    byte GetFormatoCollo();
    byte GetEventType();
    boolean GetPresenzaLogica();

    void IncreaseTrigger();
    void BalanceTrigger(int id, int trig);
    void Reset();
    
    

    };

    #endif