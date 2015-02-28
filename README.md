# Arduino VHF beacon library#
F4GOH Anthony f4goh@orange.fr <br>

February 2015

Use this library freely.

## Installation ##
To use the VHFBEACON library:  
- Go to https://github.com/f4goh/VHFBEACON, click the [Download ZIP](https://github.com/f4goh/VHFBEACON/archive/master.zip) button and save the ZIP file to a convenient location on your PC.
- Uncompress the downloaded file.  This will result in a folder containing all the files for the library, that has a name that includes the branch name, usually VHFBEACON-master.
- Rename the folder to  VHFBEACON.
- Copy the renamed folder to the Arduino sketchbook\libraries folder.

- you must add Arduino SPI library for AD9850 : F4GOJ Christophe f4goj@free.fr

- Go to https://github.com/F4GOJ/AD9850SPI, click the [Download ZIP](https://github.com/F4GOJ/AD9850SPI/archive/master.zip) button and save the ZIP file to a convenient location on your PC.

## Usage notes ##


To use the VHFBEACON library, the AD9850SPI, SoftwareSerial and SPI libraries must also be included.


```c++
#include <AD9850SPI.h>  // http://github.com/F4GOJ/AD9850SPI
#include <VHFBEACON.h>  //https://github.com/f4goh/VHFBEACON
#include <SPI.h>        // http://arduino.cc/en/Reference/SPI (included with Arduino IDE)
#include <SoftwareSerial.h>
```
