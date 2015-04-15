// Version BTS touchard sans gps avec acquisition de chaine serie en provenance de la carte ST

#include <AD9850SPI.h>
#include <VHFBEACON.h>
#include <SPI.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>



// sounding ballon char array 
unsigned char   balloon[116]={'T'<<1,'M'<<1,'B'<<1,'S'<<1,'T'<<1,' '<<1,0x60,             //avant APTT4 7 octets (0-6) call spécial TM72BST ou TMBST
                           'F'<<1,'6'<<1,'K'<<1,'F'<<1,'I'<<1,' '<<1,('0' + 12) << 1,     //F6KFI-12 7 octets (7-13)
                           'W'<<1,'I'<<1,'D'<<1,'E'<<1,'1'<<1,' '<<1,('0' + 1) << 1,      //WIDE1-1 7 octets (14-20)
                           'W'<<1,'I'<<1,'D'<<1,'E'<<1,'2'<<1,' '<<1,('0' + 1) << 1 | 1 , //WIDE2-1   fin ssid lsb =1 7 octets (21-27)
                           0x03,0xf0,                                                     //ctrl, pid 2 octets (28-29)
                           // A partir d'ici le champ de données, construite et récupérée à partir de la carte ST
                           '/','1','5','0','4','5','2','h',      //heure 8 (30-37)
                           '4','7','5','3','.','4','1','N','/','0','0','0','1','6','.','6','0','E',      //lat, long 18 octets (38-55)
                           '>','1','8','0',    //Course degrees (56-59)
                           '/','0','0','5',    //Gps speed knots (60-63)
                           '/','A','=','0','7','2','9','6','6',  // Altitude (feet) (64-72)
                           '/','A','1','=','0','0','0',  // Télémetry 0 (73-79)
                           '/','A','2','=','0','0','0',  // Télémetry 1 (80-86)
                           '/','A','3','=','0','0','0',  // Télémetry 2 (87-93)
                           '/','A','4','=','0','0','0',  // Télémetry 3 (94-100)
                           '/','A','5','=','0','0','0',  // Télémetry 4 (101-107)
                           '/',' ','B','a','l','l','o','n'};   //comment 15 car octets (108-115)

//  tester la(les) chaine(s) suiavante(s) dans le moniteur série en modifiant les caracères :
//  /150452h4753.41N/00016.60E>180/005/A=072966/A1=000/A2=000/A3=000/A4=000/A5=000/ Ballon
//  /160020h4775.41N/00001.60W>190/010/A=010000/A1=000/A2=000/A3=000/A4=000/A5=000/ Ballon
//  /172025h4801.41N/00028.60E>270/020/A=020540/A1=255/A2=000/A3=000/A4=000/A5=000/ Ballon
//  /180452h5020.41N/00056.60W>090/030/A=030258/A1=000/A2=255/A3=000/A4=000/A5=000/ Ballon

//adapter le nombre et la longueur des telemesures à votre besoin en respectant la syntaxe (entre /A1 et / Ballon)
//attention plus la chaine est longue plus il risque d'y avoir des erreurs de transmission


String DataFromST;

int n;


int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}


void setup(){

Serial.begin(57600);

//Serial.print(F("Free RAM : "));      //pour info 
//Serial.println(freeRam());
 
DDS.begin(13,9,8);  // in order pin W_CLK, FQ_UD et RESET
DDS.calibrate(125001100);
 
 Beacon.begin(4,5,6,7); //in order pin sync, led (fix gps), mod et cde
 
 Beacon.modulation(1);    //level 1 to enable ics511
 
 Beacon.freq=(144650000)/8;    // Aprs fréquency for balloon
 Beacon.ddsPower(0);          // power down


}


/*boucle principale
1 En attente de données SERIE en provenance de la carte ST
attention données SERIES 0/5V compatible TTL
2 Lecture d'un chaine jusqu'a nouvelle ligne \n
3 copie dans le tableau d' unsigned char
4 purge serial+ interruption HS
5 Mise en route du dds
6 envoi des donnes
7 Dds off
8 interruption en service
9 info TXING 

remaque : l'intervale de temps conseillé (de 20 secondes) entre 2 emissions se fait grace à la carte ST
et la réception de données Séries

*/

void loop(){

 if (Serial.available()){
  DataFromST = Serial.readStringUntil('\n');  
  Serial.print(DataFromST.length());    //acq to ST data length
  
  for (n=0;n<DataFromST.length();n++) {
                                          balloon[30+n]=DataFromST[n];
                                          //Serial.print((char) balloon[30+n]);      //verify
                                        }
 //verifications
 /*  
 Serial.println(DataFromST);

 for(n=0;n<sizeof(balloon);n++) {      //print char array track (for info)
                                 Serial.print(balloon[n],HEX);
                                 Serial.print(":");
                                 Serial.print((char)balloon[n]);
                                 Serial.print(",");
                               }    
 Serial.println();
  */
 
  Serial.flush();                          //purge Serialout before txing
  cli();                                   //disable interrupt before txing
  Beacon.ddsPower(1);                      //dds Power on
  Beacon.sendpacket(balloon,sizeof(balloon));                                                 
  Beacon.ddsPower(0);                      //dds power off
  sei();                                   //enable interrupt for serials
  Serial.println("TX Done");               //Roger it's done (optional)

 }
 
}

  
  
  












