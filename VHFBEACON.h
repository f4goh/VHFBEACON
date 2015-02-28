/********************************************************************************************
 * VHF multimodes beacon Arduino library
 * Created 29/12/2014
 * Anthony LE CREN f4goh@orange.fr 
 * Modified 25/02/2015
 * Use this library freely
 *
 * Instance :
 *
 * Functions :
 *
 *******************************************************************************************/
 
 
#ifndef VHFBEACON_H
#define VHFBEACON_H
#include <Arduino.h>
#include <SoftwareSerial.h>

class VHFBEACON
{
  public:
    VHFBEACON();
   
   
   
   void begin(int p_sync, int p_led, int p_mod, int p_cde);
   void ddsPower(int powDds);
   void modulation(int modIcs);
   void sendpacket(unsigned char buffer[], unsigned char size_array);
   void gpsnmea(char byteGPS);


   double freq;
   int sync;
   int led;
   int mod;
   int cde;

   typedef struct  {
  char hour[6+1];
  char Latitude[9+1];
  char NS;
  char Longitude[10+1];
  char EO;
  byte fix;
  char sat[2+1];
  char altitude[7+1];
  byte secondes;     //secondes in byte from hour
  byte pperiod;      //second order
  byte sync;     //flag to send (matching for secondes%pperiod==0)
  boolean debug;
  boolean dumpNmea;
  int nbSat;
  long altidudeMeters;
  long altidudeFeet;
  char feet[15];
  } GGAstruct;
  GGAstruct GPGGA;    //declare la structure
   
  
  private:
  
  void sendbyte (unsigned char inbyte);
  void fcsbit(unsigned short tbyte);
  void flipout(void);
  void send_bit();
  
  unsigned char stuff,flag,fcsflag;
  unsigned char flip,f1200;
  unsigned short crc;
  double freq_sin;
  
  int sentence_status;      //0: recherche $, 1:recherche GPxxx, 2:GPGGA trouvé
  char sentenceType[5+1];     //GPxxx
  int ptr;                  //ptr for cahr arrays
  int comma_count;          //count , into sentences 

  
};

extern VHFBEACON Beacon;

#endif
