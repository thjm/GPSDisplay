# About

This is the project GPSDisplay. The initial goal of the project is to
have a similar device like the one described in FA 4/09 pg. 403 [1].

The original device is based on a PIC 18F252 and uses a Navilock NL-504ETTL or
NL-501ETTL (Sirf3) as GPS module. By pressing a button one can schange the
text displayed at the LCD between different display modes.

The GPS decoding routines stem mainly from the WhereAVR project of G.Dion 
(N4TXI) - see also ref. [2].

The LCD routines are taken from the library of P.Fleury [3].

The used GPS module is a Garmin GOS25-LVC (12-channel receiver) [4].

The second version is using the NAVILOCK NL303P GPS module, its pinout is 
described here [5] (and in this file).

Pin  Pin name                Description
 1   GND (black)             Ground signal for power and data
 2   VCC +5V (red)           Power supply voltage to GPS +5V
 3   Data TTL Rx (blue)      Data to GPS, TTL levels
 4   Data RS-232 Rx (white)  Data to GPS, RS-232 levels
 5   Data RS-232 Tx (green)  Data from GPS, RS-232 levels
 6   Data TTL Tx (yellow)    Data from GPS, TTL levels

# References

 [1] - www.funkamateur.de
 
 [2] - http://garydion.com/projects/whereavr/; WhereAVR_18JUL2005.zip

 [3] - http://www.jump.to/fleury

 [4] - GPS 25 Series, Technical Specification, Rev. G, Mar, 2000,
       https://buy.garmin.com/shop/store/manual.jsp?product=010-00124-00&cID=170&pID=70
       https://buy.garmin.com/shop/shop.do?cID=170&pID=70&ra=true,
       http://www.gpscity.com/item-garmin-oem-25-lvc/oem25lvc.htm

 [5] - http://pinouts.ru/GPS/navilock_gps_pinout.shtml


## Usage

First you have to install a cross-build environment for AVR micro controllers.
