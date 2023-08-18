#include <ELECHOUSE_CC1101_SRC_DRV.h>


#define CCGDO0 33
#define CCGDO2 38
#define CCSCK 36
#define CCMISO 37
#define CCMOSI 35
#define CCCSN 34


void CCSetTx() {
  pinMode(CCGDO0,OUTPUT);
  ELECHOUSE_cc1101.SetTx();
}

void CCInit() {

  ELECHOUSE_cc1101.setSpiPin(CCSCK, CCMISO, CCMOSI, CCCSN);
  ELECHOUSE_cc1101.Init();                // must be set to initialize the cc1101!
  ELECHOUSE_cc1101.setGDO(CCGDO0, CCGDO2);
  ELECHOUSE_cc1101.setMHZ(433.92);
                                          //The lib calculates the frequency automatically (default = 433.92).
                                          //The cc1101 can do: 300-348 MHZ, 387-464MHZ and 779-928MHZ. Read More info from datasheet.
  ELECHOUSE_cc1101.setClb(1,13,15);
  ELECHOUSE_cc1101.setClb(2,16,19);
  ELECHOUSE_cc1101.setModulation(2);      // set modulation mode. 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK.
  ELECHOUSE_cc1101.setDRate(512);         // Set the Data Rate in kBaud. Value from 0.02 to 1621.83. Default is 99.97 kBaud!
  ELECHOUSE_cc1101.setRxBW(256);          // Set the Receive Bandwidth in kHz. Value from 58.03 to 812.50. Default is 812.50 kHz.
  ELECHOUSE_cc1101.setPktFormat(3);       // Format of RX and TX data. 0 = Normal mode, use FIFOs for RX and TX. 
}

void CCSetRx() {
  pinMode(CCGDO0,INPUT);  
  ELECHOUSE_cc1101.SetRx();
}
 

#define BAVGSIZE 11
byte bavg[BAVGSIZE];
byte pb = 0;
byte cres = 0;

byte CCAvgRead() {
  cres -= bavg[pb];
  bavg[pb] = digitalRead(CCGDO0);
  cres += bavg[pb];
  pb++;
  if (pb >= BAVGSIZE) pb = 0;
  if (cres > BAVGSIZE/2) return 1;
  return 0;
}

void CCWrite(int b) {
  digitalWrite(CCGDO0,b);
}
