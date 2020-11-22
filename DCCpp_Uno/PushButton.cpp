
#include "PushButton.h"
#include "SerialCommand.h"
#include "DCCpp_Uno.h"
#include "EggTimer.h"


enum {
  STOP=0,
  SLOW=60,    
  LIGHT=1001,
  PANTO1,
  PANTO2,
};

int playbook[] = {
    SLOW,
    LIGHT,
    PANTO1,
    PANTO2
};

volatile RegisterList *PushButton::mRegs;
int myCab;

void PushButton::init(volatile RegisterList *_mRegs, int cab) {
    mRegs = _mRegs;
    myCab = cab;
}
  
void PushButton::button(int btn){
    digitalWrite(SIGNAL_ENABLE_PIN_MAIN,HIGH);
    buttonTimer.setEvent(1000,0);
}

void PushButton::process(int pos){    
    Serial.println("Running command");
    int val = abs(playbook[pos]); 
    bool reverse = playbook[pos] < 0;
    // Speed
    char cmd[20];
    int delay = 250;
    if (val < 127) {
        sprintf(cmd, "1 %01d %01d %01d", myCab, val, reverse ? 1 : 0);
        mRegs->setThrottle(cmd);
    } else if (val == 128) {
        val = playbook[++pos] * 500;
    } else if (val >= 1000 && val <= 1032) {
        val -= 1000; 
        sprintf(cmd, "%01d %01d", myCab, 128 + val);
        mRegs->setFunction(cmd);

    }  else {
        //unknown
    }
    if (pos < sizeof(playbook)) {
        buttonTimer.setEvent(delay, pos+1);
    } 
}

EggTimer PushButton::buttonTimer(PushButton::process);  
