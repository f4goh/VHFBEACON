/********************************************************************************************
 * VHF multimodes beacon Arduino exemple
 * Created 28/02/2015
 * Anthony LE CREN f4goh@orange.fr 
 * Use this code freely
 * send APRS wx sentence every minutes
 * ctn is connected to A0
 *		(Ground) ---- (10k-Resister) -------|------- (Thermistor 10K) ---- (+5v)
 *                                                  |								
 *                                              Analog Pin A0
 *******************************************************************************************/

#include <AD9850SPI.h>
#include <VHFBEACON.h>
#include <SPI.h>
#include <Wire.h>
#include <avr/pgmspace.h>

#define THERM_PIN   A0  // 10ktherm & 10k resistor as divider.

#include <Thermistor.h>
Thermistor temp(THERM_PIN);


unsigned long pausetime = 1*60*1000L;    //every minutes



// weather char array
unsigned char     wx[90]={'F'<<1,'4'<<1,'G'<<1,'O'<<1,'H'<<1,' '<<1,0x60,                //avant APTT4
                           'F'<<1,'4'<<1,'G'<<1,'O'<<1,'H'<<1,' '<<1,('0' + 11) << 1,     //F4GOH-11
                           'W'<<1,'I'<<1,'D'<<1,'E'<<1,'1'<<1,' '<<1,('0' + 1) << 1,      //WIDE1-1
                           'W'<<1,'I'<<1,'D'<<1,'E'<<1,'2'<<1,' '<<1,('0' + 1) << 1 | 1 , //WIDE2-1   fin ssid lsb =1
                           0x03,0xf0,                                                     //ctrl, pid
                           '!','4','8','5','1','.','2','0','N','/','0','0','2','2','0','.','9','2','E',      //lat, long
                           '_','.','.','.',            // direction du vent en degrés // info weather APRS101.pdf p74
                           '/','.','.','.',            // vitesse du vent m/s a confirmer
                           'g','.','.','.',            //g = gust (peak wind speed in mph in the last 5 minutes).
                           't','.','.','.',            //t = temperature (in degrees Fahrenheit). ex 025,  Temperatures below zero are expressed as -01 to -99.
                           'r','.','.','.',            //r = rainfall (in hundredths of an inch) in the last hour.
			   'p','.','.','.',            //p =  rainfall (in hundredths of an inch) in the last 24 hours.
                           'P','.','.','.',            //P =  rainfall (in hundredths of an inch) since midnight.
                           'h','.','.',                //h = humidity (in %. 00= 100%).
                           'b','1','0','2','0','2',    //b = barometric pressure (in tenths of millibars/tenths of hPascal). décipascal
                           'A','L','E','C'};           //code id du fabricant de la station météo 4 car max

/*Other parameters that are available on some weather station units include:
L = luminosity (in watts per square meter) 999 and below.
l (lower-case letter “L”) = luminosity (in watts per square meter)
1000 and above.
(L is inserted in place of one of the rain values).
s = snowfall (in inches) in the last 24 hours.
# = raw rain counter
*/

int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}



void setup() {
   Serial.begin(57600);
   Serial.print(F("Free RAM : "));
   Serial.println(freeRam());
   DDS.begin(13, 9, 8);   // in order pin W_CLK, FQ_UD et RESET
    DDS.calibrate(125001100);
   Beacon.begin(4, 5, 6, 7);   //in order pin sync, led (fix gps), mod et cde
    Beacon.mode =0;   //0: fm mode  1:ssb mode 
    Beacon.modulation(1);   //level 1 to enable ics511
    Beacon.freq = (144800000) /8;   // Aprs fréquency
    Beacon.ddsPower(0);   // power down
 }

void loop() {
   NTCsensor();
   //for (int n=0;n<sizeof(wx);n++) {Serial.print(wx[n],HEX);Serial.print(",");}    //print char array track (for  info)
    //Serial.println();
    Serial.flush();   //purge Serialout before txing
    cli();   //disable interrupt before txing
    Beacon.ddsPower(1);   //dds Power on
    Beacon.sendpacket(wx, sizeof(wx));   //send packet
    Beacon.ddsPower(0);   //dds power off
    sei();   //enable interrupt for  serials
    Serial.println("TX Done");   //Roger it's done
    delay(pausetime);
}

void NTCsensor() {
   int cent,diz,uni;
   int therm = temp.getTemp();
   Serial.print(therm);
   therm = therm * 9 /5 + 32;   //to Fahrenheit
    if (therm <0) {      //sign test
       wx[62] = '-';
      therm *=-1;
   } else {
      cent = therm /100;
      wx[62] = cent + '0';
   }
   diz = (therm - (cent *100)) /10;
   uni = therm - (cent *100) - (diz *10);
   wx[63] = diz + '0';
   wx[64] = uni + '0';
}














