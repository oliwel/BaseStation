#include "Arduino.h"
#include "EggTimer.h"
#include "PacketRegister.h"

#ifndef PushButton_h
#define PushButton_h

struct PushButton{
  static EggTimer buttonTimer;
  static volatile RegisterList *mRegs;
  static void init(volatile RegisterList *, int cab);
  static void process(int snum);
  static void button(int btn);
};

#endif

