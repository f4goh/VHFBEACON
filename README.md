# Arduino VHF multimodes beacon library#
F4GOJ Christophe f4goj@free.fr<br>
F4GOH Anthony f4goh@orange.fr

October 2014

Use this library freely.

## Installation ##
To use the HFBEACON library:  
- Go to https://github.com/F4GOJ/HFBEACON, click the [Download ZIP](https://github.com/F4GOJ/HFBEACON/archive/master.zip) button and save the ZIP file to a convenient location on your PC.
- Uncompress the downloaded file.  This will result in a folder containing all the files for the library, that has a name that includes the branch name, usually HFBEACON-master.
- Rename the folder to  HFBEACON.
- Copy the renamed folder to the Arduino sketchbook\libraries folder.


## Usage notes ##

The HFBEACON library instantiates a Beacon object, the user does not need to do this.

To use the HFBEACON library, the AD9850SPI and SPI libraries must also be included.


```c++
#include <HFBEACON.h>  // http://github.com/F4GOJ/HFBEACON
#include <AD9850SPI.h> // http://github.com/F4GOJ/AD9850SPI
#include <SPI.h>       // http://arduino.cc/en/Reference/SPI (included with Arduino IDE)
```
