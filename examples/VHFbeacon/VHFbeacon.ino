/********************************************************************************************
 * VHF multimodes beacon Arduino exemple
 * Created 28/02/2015
 * Anthony LE CREN f4goh@orange.fr 
 * Use this code freely
 *******************************************************************************************/

#include <AD9850SPI.h>
#include <VHFBEACON.h>
#include <SPI.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>

#define txPin 3      //tx pin in GPS connection
#define rxPin 2      //rx pin in GPS connection

SoftwareSerial gps = SoftwareSerial(rxPin, txPin);


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


// track char array
unsigned  char     track[72]={'F'<<1,'4'<<1,'G'<<1,'O'<<1,'H'<<1,' '<<1,0x60,              //avant APTT4 7 octets (0-6)
                           'F'<<1,'4'<<1,'G'<<1,'O'<<1,'H'<<1,' '<<1,('0' + 12) << 1,     //F4GOH-11 7 octets (7-13)
                           'W'<<1,'I'<<1,'D'<<1,'E'<<1,'1'<<1,' '<<1,('0' + 1) << 1,      //WIDE1-1 7 octets (14-20)
                           'W'<<1,'I'<<1,'D'<<1,'E'<<1,'2'<<1,' '<<1,('0' + 1) << 1 | 1 , //WIDE2-1   fin ssid lsb =1 7 octets (21-27)
                           0x03,0xf0,                                                     //ctrl, pid 2 octets (28-29)
                           '/','1','5','0','4','5','2','h',      //heure 8 (30-37)
                           '4','8','5','1','.','2','0','N','/','0','0','2','2','0','.','9','2','E',      //lat, long 18 octets (38-55)
                           '>','7','3',' ','A','n','t','h','o','n','y',' ',' ',' ',' ',' '};               //commentaire 15 car octets (56-71)


// sounding ballon char array 
unsigned char   balloon[116]={'F'<<1,'4'<<1,'G'<<1,'O'<<1,'H'<<1,' '<<1,0x60,              //avant APTT4 7 octets (0-6)
                           'F'<<1,'6'<<1,'K'<<1,'F'<<1,'I'<<1,' '<<1,('0' + 11) << 1,     //F4GOH-11 7 octets (7-13)
                           'W'<<1,'I'<<1,'D'<<1,'E'<<1,'1'<<1,' '<<1,('0' + 1) << 1,      //WIDE1-1 7 octets (14-20)
                           'W'<<1,'I'<<1,'D'<<1,'E'<<1,'2'<<1,' '<<1,('0' + 1) << 1 | 1 , //WIDE2-1   fin ssid lsb =1 7 octets (21-27)
                           0x03,0xf0,                                                     //ctrl, pid 2 octets (28-29)
                           '/','1','5','0','4','5','2','h',      //heure 8 (30-37)
                           '4','8','5','1','.','2','0','N','/','0','0','2','2','0','.','9','2','E',      //lat, long 18 octets (38-55)
                           '>','1','8','0',    //Course degrees (56-59)
                           '/','0','0','5',    //Gps speed knots (60-63)
                           '/','A','=','0','7','2','9','6','6',  // Altitude (feet) (64-72)
                           '/','A','1','=','0','0','0',  // Télémetry 0 (73-79)
                           '/','A','2','=','0','0','0',  // Télémetry 1 (80-86)
                           '/','A','3','=','0','0','0',  // Télémetry 2 (87-93)
                           '/','A','4','=','0','0','0',  // Télémetry 3 (94-100)
                           '/','A','5','=','0','0','0',  // Télémetry 4 (101-107)
                           '/',' ','B','a','l','l','o','n'};   //comment 15 car octets (108-115)



int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}


void setup(){

Serial.begin(57600);

pinMode(rxPin, INPUT);          //
pinMode(txPin, OUTPUT);

Serial.print(F("Free RAM : "));
Serial.println(freeRam());
 
 DDS.begin(13,9,8);  // in order pin W_CLK, FQ_UD et RESET
 DDS.calibrate(125001100);
 
 Beacon.begin(4,5,6,7); //in order pin sync, led (fix gps), mod et cde
 
 Beacon.GPGGA.pperiod=10;  //interval between two tx
 Beacon.GPGGA.debug=true;  //allow debug print char
 Beacon.GPGGA.dumpNmea=false;  //allow print nmea sentence
 
 Beacon.modulation(1);    //level 1 to enable ics511
// delay(100); 
 
 Beacon.freq=(144800000)/8;    // Aprs fréquency
 Beacon.ddsPower(0);          // power down
 
// delay(100); 


gps.begin(4800);        //set up baud speed for GPS
//gps.begin(9600);


}



void loop(){

  
if (gps.available()) Beacon.gpsnmea(gps.read());    //if char into software serial decode nmea sentence

if (Beacon.GPGGA.sync==1)        // test if is time to txing
                  {
                   Beacon.GPGGA.sync=0;    //txing sequence, so sync done
                   /*
                   Serial.println( Beacon.GPGGA.hour);          //print some GPS info
                   Serial.print( Beacon.GPGGA.Latitude);
                   Serial.println( Beacon.GPGGA.NS);
                   Serial.print( Beacon.GPGGA.Longitude);
                   Serial.println( Beacon.GPGGA.EO);
                   Serial.println( Beacon.GPGGA.fix);
                   Serial.println( Beacon.GPGGA.altitude);        //altitude meter in char array
                   Serial.println( Beacon.GPGGA.altidudeMeters);  //altitude meter in long
                   Serial.println( Beacon.GPGGA.altidudeFeet);    //altitude feet in long
                   Serial.println( Beacon.GPGGA.feet);            //altitude feet in char array
                   */
 
                  memcpy(track+31,Beacon.GPGGA.hour,6);          //prepare char array track to send
                  memcpy(track+38,Beacon.GPGGA.Latitude,7);      //beware index number from char array
                  track[45]=Beacon.GPGGA.NS;
                  memcpy(track+47,Beacon.GPGGA.Longitude,8);
                  track[55]=Beacon.GPGGA.EO;



                  //for(int n=0;n<sizeof(track);n++) {Serial.print(track[n],HEX);Serial.print(",");}    //print char array track (for info)
                  //Serial.println();


   
 
                  Serial.flush();                          //purge Serialout before txing
                  cli();                                   //disable interrupt before txing
                  Beacon.ddsPower(1);                      //dds Power on
                  Beacon.sendpacket(track,sizeof(track));  //send packet
                  Beacon.ddsPower(0);                      //dds power off
                  sei();                                   //enable interrupt for serials
                  Serial.println("TX Done");               //Roger it's done
                  }
}

  
  
  












