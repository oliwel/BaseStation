
#include "PushButton.h"
#include "SerialCommand.h"
#include "DCCpp_Uno.h"
#include "EggTimer.h"


enum {
  STOP=0,
  F_LANGSAM=30,
  F_MITTEL=80,
  F_SCHNELL=120,
  WARTE=128,    
  LIGHT=1000,
  TELEXH,
  G_BETRIEB,
  G_PFEIFE,
  PANTO1,
  PANTO2,
  L_FUEHRER,
  G_SANDEN,
  ABV,
  G_BREMSENAUS,
  L_MASCHINE,
  G_DRUCKLUFT,
  L_SLWROT,
  L_SLWAUS,
  G_LEITER,
  L_FALSCHFAHRT,
  G_KABINENFUNK,
  G_KABINENFUNK2,
  G_HAUPTSCHALTER,
  G_TUERE,
  G_RANGIERBAHNHOF,
  G_KOMPRESSOR,
  G_RANGIERPFIFF,
  G_LUEFTER,
  G_SCHAFFNERPFIFF,
  RANGIERFAHRT,
  G_BAHNHOFANSAGE,
  G_HALTESTELLENANSAGE,
  G_WARTUNGSTRUPP,
  G_SCHEIBENWISCHER,
  G_TRAFO,
  TELEXV, 
};

int playbook[] = {
    LIGHT,
    PANTO1,
    WARTE, 2,
    F_LANGSAM,
    WARTE, 5,
    -G_BETRIEB,
    WARTE, 1,
    -PANTO1,
    STOP
};

volatile RegisterList *PushButton::mRegs;
int myCab;
uint32_t funcState;


void PushButton::init(volatile RegisterList *_mRegs, int cab) {
    mRegs = _mRegs;
    myCab = cab;
    funcState = 0;
}
  
void PushButton::button(int btn){
    digitalWrite(SIGNAL_ENABLE_PIN_MAIN,HIGH);
    buttonTimer.setEvent(1000,0);
}

void PushButton::process(int pos){    
    Serial.print("Running command ");
    Serial.println(pos);
    uint32_t val = abs(playbook[pos]); 
    bool reverse = playbook[pos] < 0;
    // Speed
    char cmd[20];
    int delay = 250;
    if (val < 127) {
        sprintf(cmd, "1 %01d %01d %01d", myCab, val, (reverse ? 1 : 0));
        // mRegs->setThrottle(cmd);
        Serial.print("Speed-Command is ");
        Serial.println(cmd);
    } else if (val == 128) {
        delay = playbook[++pos] * 500;
        Serial.print("Adding delay ");
        Serial.println(delay);
    } else if (val >= 1000 && val <= 1031) {
        val -= 1000;
        if (reverse) {
            funcState &= ~(1 << val);
        } else {
            funcState |= (1 << val);
        }

        // Function F0 to F12 in blocks with Offset in Byte 1
        // F0 - F4 = +128
        if (val <= 4) {            
            sprintf(cmd, "%01d %01d", myCab, 128 + (funcState & 31));            
        // F5 - F8 = +176
        } else if (val <= 8)  {
            sprintf(cmd, "%01d %01d", myCab, 176 + ((funcState >> 5) & 15));
        // F9 - F12 = +160
        } else if (val <= 12)  {
            sprintf(cmd, "%01d %01d", myCab, 160 + ((funcState >> 9) & 15));        
        // Function F13 to F28 with static value in byte 1 and bits in Byte 2
        // F13 - F20 = 222  
        } else if (val <= 20)  {            
            sprintf(cmd, "%01d 222 %01d", myCab, ((funcState >> 17) & 255));
        // F21 - F28 = 222 
        } else {
            sprintf(cmd, "%01d 223 %01d", myCab, ((funcState >> 25) & 255));
        } 
        Serial.print("F-Command is ");
        Serial.println(cmd); 
        mRegs->setFunction(cmd);

    }  else {
        //unknown
    }
    if (pos < sizeof(playbook)/sizeof(int)) {        
        buttonTimer.setEvent(delay, pos+1);
    } else {
        Serial.println("end of Playbook");
    }
}

EggTimer PushButton::buttonTimer(PushButton::process);  
