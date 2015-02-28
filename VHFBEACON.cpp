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
 

#include <VHFBEACON.h>
#include <AD9850SPI.h>
#include <avr/pgmspace.h>


VHFBEACON Beacon;


VHFBEACON::VHFBEACON(){
 sentence_status=0;      //0: recherche $, 1:recherche GPxxx, 2:GPGGA trouvé
 ptr=0;                  //ptr for cahr arrays
 comma_count=0;          //count , into sentences 
 GPGGA.nbSat=0;
 GPGGA.debug=false;
 GPGGA.dumpNmea=false;
}

void VHFBEACON::begin(int p_sync, int p_led, int p_mod, int p_cde) {
 sync=p_sync;
 led=p_led;
 mod=p_mod;
 cde=p_cde;

 pinMode(sync, OUTPUT);
 pinMode(led, OUTPUT);
 pinMode(mod, OUTPUT);
 pinMode(cde, OUTPUT);
 
 digitalWrite(cde, LOW);      //dds off
 digitalWrite(mod, LOW);      //mod off
 digitalWrite(led, LOW);
 digitalWrite(sync, LOW); 
  
}


/********************************************************
 * Power management
 ********************************************************/
void VHFBEACON::ddsPower(int powDds){
 if(powDds == 1)
 {
  digitalWrite(cde, LOW);
  DDS.powerOn();
 }
 else if(powDds == 0)
 {
  digitalWrite(cde, HIGH);
  DDS.powerOff();
 }
}

/********************************************************
 * ICS511 modulation
 ********************************************************/
void VHFBEACON::modulation(int modIcs){
 if(modIcs == 1)
 {
  digitalWrite(mod, HIGH);
  }
 else if(modIcs == 0)
 {
  digitalWrite(mod, LOW);
 }
}

/********************************************************
 * Send a bit into an FM modulation
 ********************************************************/
 
void VHFBEACON::send_bit()
{
//sw=(sw^1)&1;
//digitalWrite(5, sw); 
// 1/2200=454µs 830µ  tempo 125
// 1/1200=833µs 830µs  tempo 325
 
 #define calibre 1
 #define t2200   125*calibre
 #define t1200   325*calibre
 #define a2200   300
 #define a1200   150
 

if (f1200==0)
                  {
                  freq_sin=freq+a2200;
                   DDS.setfreq(freq_sin,0); // fréquence en Hz (type double)
                   delayMicroseconds(t2200);

                    freq_sin=freq-a2200;
                  DDS.setfreq(freq_sin,0); // fréquence en Hz (type double)
                  delayMicroseconds(t2200); 

                  freq_sin=freq+a2200;
                   DDS.setfreq(freq_sin,0); // fréquence en Hz (type double)
                   delayMicroseconds(t2200);

                    freq_sin=freq-a2200;
                  DDS.setfreq(freq_sin,0); // fréquence en Hz (type double)
                  delayMicroseconds(t2200); 
                  }
                  else
                  {
freq_sin=freq+a1200;
DDS.setfreq(freq_sin,0); // fréquence en Hz (type double)
delayMicroseconds(t1200);

freq_sin=freq-a1200;
DDS.setfreq(freq_sin,0); // fréquence en Hz (type double)
delayMicroseconds(t1200); 
                  }
}

/********************************************************
 * AX25 routines
 ********************************************************/

void VHFBEACON::flipout(void)
 {		       
	stuff = 0;     //since this is a 0, reset the stuff counter
	if (f1200==0) f1200=1; else f1200=0;
}

void VHFBEACON::fcsbit(unsigned short tbyte)
{
  crc ^= tbyte;
  if (crc & 1)
    crc = (crc >> 1) ^ 0x8408;  // X-modem CRC poly
  else
    crc = crc >> 1;
     
 }
 

void VHFBEACON::sendbyte (unsigned char inbyte)
{
   unsigned char k, bt;
   for (k=0;k<8;k++)
	{                                                         //do the following for each of the 8 bits in the byte
     bt = inbyte & 0x01;                                          //strip off the rightmost bit of the byte to be sent (inbyte)
     if ((fcsflag==0) & (flag==0)) (VHFBEACON::fcsbit(bt));                 //do FCS calc, but only if this is not a flag or fcs byte
     if (bt==0) (VHFBEACON::flipout());  			                // if this bit is a zero, flip the output state
        else {                          			//otherwise if it is a 1, do the following:
           stuff++;    				                //increment the count of consequtive 1's 
            if ((flag==0) & (stuff==5))
				{   	                        //stuff an extra 0, if 5 1's in a row
                                       		                 //flip the output state to stuff a 0
                               VHFBEACON::send_bit();
                                VHFBEACON::flipout();  
                        }                                      
        }
     inbyte = inbyte>>1;          		                //go to the next bit in the byte
VHFBEACON::send_bit();
  }//fin pour
}


void VHFBEACON::sendpacket(unsigned char buffer[], unsigned char size_array)
{
	unsigned char i;
    crc=0xffff;
	stuff=0;
   
    f1200=0;      //init
   VHFBEACON::send_bit();
   
   flag = 1;             //The variable flag is true if you are transmitted flags (7E's) false otherwise.
   fcsflag = 0;       //The variable fcsflag is true if you are transmitting FCS bytes, false otherwise.

   for (i=0;i<100;i++) VHFBEACON::sendbyte(0x7E);	        //Sends 100 flag bytes.
   flag = 0;          			        //done sending flags
   for(i=0;i<size_array;i++) VHFBEACON::sendbyte(buffer[i]);       //send the packet bytes
   fcsflag = 1;       		//about to send the FCS bytes
   VHFBEACON::sendbyte((crc ^ 0xff));	// Send the CRC
   crc >>= 8;
   VHFBEACON::sendbyte((crc ^ 0xff));
   fcsflag = 0;		//done sending FCS
   flag = 1;  	//about to send flags
  for (i=0;i<100;i++) VHFBEACON::sendbyte(0x7E);	        //Sends 100 flag bytes.
   
  }
  
  
 void VHFBEACON::gpsnmea(char byteGPS)
{
 if( GPGGA.dumpNmea==true)Serial.print((char)byteGPS);
 switch (byteGPS)
{
  case '$' : if (sentence_status==0) {
                                      sentence_status=1;
                                      ptr=0;
                                      if (GPGGA.debug==true) Serial.print('$');
                                     }
              break;
  case ',' : if (sentence_status==1) {
                                         sentenceType[ptr++]=0;
                                         if (strcmp(sentenceType,"GPGGA")==0) 
                                                  {
                                                   if (GPGGA.debug==true) Serial.println(F("gga found"));
                                                   sentence_status=2;    //can extend sentence_status to 3,4 etc for another GPxxx sentences
                                                  }
                                                  else sentence_status=0;
                                      } 
              if (sentence_status==2) {
                                       switch (comma_count)
                                       {
                                      case 1 :   GPGGA.hour[ptr]=0;
                                                 if (GPGGA.fix==1) {
																	GPGGA.secondes=(GPGGA.hour[4]-'0')*10+GPGGA.hour[5]-'0';
																	if (GPGGA.debug==true) {Serial.print(F("Hour :")); Serial.println(GPGGA.hour);}
																	if ((GPGGA.secondes%GPGGA.pperiod)==0) GPGGA.sync=1; else GPGGA.sync=0;
																	}
												break; 
                                      case 2 :   GPGGA.Latitude[ptr]=0;
												 break; 
                                      case 4 :   GPGGA.Longitude[ptr]=0;
												 break; 
                                      case 6 :  if (GPGGA.fix==1) digitalWrite(led, HIGH); else {
																									digitalWrite(led, digitalRead(led)^1);
																								    if (GPGGA.debug==true)		{
																																	Serial.print(F("Nb sat :"));
																																	Serial.println(GPGGA.sat);
																																	}
																								}
												break; 
									  case 7 : GPGGA.sat[ptr]=0;
											   GPGGA.nbSat=atoi(GPGGA.sat);
                                               break;            
                                      case 9 :   GPGGA.altitude[ptr]=0;
									          GPGGA.altidudeMeters=atol(GPGGA.altitude);
									          GPGGA.altidudeFeet=(long) GPGGA.altidudeMeters*328/100;
											  ltoa(GPGGA.altidudeFeet,GPGGA.feet,10);
											  break; 
                                      }
                                    }
              ptr=0;
              comma_count++;
              break;
  case '*' :  sentence_status=0;
              comma_count=0;
			  break;
  default:
   if (sentence_status==1) sentenceType[ptr++]=byteGPS;
   if (sentence_status==2) {
                             switch (comma_count)
                             {
                              case 1 :   if (ptr<6) GPGGA.hour[ptr++]=byteGPS;
                                         break;
                              case 2 :   GPGGA.Latitude[ptr++]=byteGPS;
                                         break;            
                              case 3 :   GPGGA.NS=byteGPS;
                                         break;            
                              case 4 :   GPGGA. Longitude[ptr++]=byteGPS;
                                         break;            
                              case 5 :   GPGGA.EO=byteGPS;
                                         break;            
                              case 6 :   GPGGA.fix=byteGPS-'0';
                                         break;            
                              case 7 :   GPGGA.sat[ptr++]=byteGPS;
                                         break;            
							  case 9 :   GPGGA.altitude[ptr++]=byteGPS;
                                         break;            
                             }
                       }
 }
}
 
  
