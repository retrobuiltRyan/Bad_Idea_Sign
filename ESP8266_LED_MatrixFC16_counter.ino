//Counts time in hours (with one and tenths of an hour) for a MAX72XX LED matrix display 
//ultilizes [poorly] the MD_Parola library.
//    Compiled on Arduino IDE 1.8.10
//    June 9, 2023
// v0.9 because it's working, but not written very well
// MD_MAX72XX library can be found at https://github.com/MajicDesigns/MD_MAX72XX


 //global variable
char secondsDecimal[16] = {0}; // oversized buffer to store our data
char hoursDecimal[16] = {0}; 
char minutesDecimal[16] = {0};

char everythingData[32] = {0}; //way oversized
char printThis[32] = {0}; //final data to be pushed to screen

String leadZeroHr = "";
String leadZeroMin = ""; //fricking ' and " got me on this one for 2hrs
String leadZeroSec = ""; 

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN   D5 // or SCK   // old config 5,7,8
#define DATA_PIN  D7 // or MOSI
#define CS_PIN    D8 // or SS


// Hardware SPI connection
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// Arbitrary output pins
// MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

long seconds;
long secondsR;
long seconds_Base100;
long minutes;
long minutesR;
long minutes_Base100;
long hours;

void setup(){
  Serial.begin(9600);
  // put your setup code here, to run once:
mx.begin(); //initialize display for MAX72xx library (needed for the wipe animation)
P.begin();
P.setIntensity(1);  //0-15; 15 is max brights. has to go afer initialize (P.begin())
stripe();
}


void loop(){ //Clock counting math here BASE 60 for clock time HH:MM:SS. limits are xxx:59:59
seconds = millis()/1000; //total seconds elasped since power on
secondsR = seconds % 60; // display seconds as 0-59
seconds_Base100 = map(secondsR,0,59,0,99); //conterting to base100 (a percentage) for time keeping.  this feels wrong
minutes = seconds/60; //total minutes elapsed
minutesR = minutes % 60; //display minutes as 0-59
minutes_Base100 = map(minutesR,0,59,0,99); //conterting to base100 (a percentage) for time keeping. meh
hours = minutes / 60 ;

//debug check: display as HH:MM:SS.000
Serial.print(hours); Serial.print(":");Serial.print(minutesR); Serial.print(":");Serial.print(secondsR); Serial.print(".");Serial.println(millis()%1000); 


//==LEADING ZERO fixes===============
if (minutes_Base100 <10) {
  (leadZeroHr = '0'); } //lead zero string contains a leading zero for human reading
else {
leadZeroHr = '\0'; } //null character for string

 
 

//=WARNING= At this point variables get turned into strings, so integer math and comparisions are broken!
//Trying to display time as hours with the decimal in base 100.
String hoursDecimal = String(minutes_Base100); // hours string diplays as 0-99. A divisive choice
String hoursString = String(hours);

String minutesDecimal = String(seconds_Base100); // minutes string displays as 0-99
String minutesString = String(minutes);

String secondsDecimal = String(millis()%1000); // seconds string displays as 0-99 (base 100)
String secondsString = String(seconds);

//=====Push To MATRIX===================================================================================
String everythingString = " " + hoursString + "." + leadZeroHr + hoursDecimal + "   ";
//
everythingString.toCharArray(printThis, sizeof(printThis));
P.print(printThis); //p.print works with decimals and chars! neat!

//ultra debug  because numbers are hard
//Serial.print(hours);Serial.print(".");    Serial.print(leadZeroHr);  Serial.print(hoursDecimal);Serial.print("  ");
//Serial.print(minutes);Serial.print(".");  Serial.print(leadZeroMin); Serial.print(minutesDecimal);Serial.print("  ");
//Serial.print(seconds);Serial.print(".");  Serial.print(leadZeroSec); Serial.println(secondsDecimal);

 

}
//-----------Subroutines-----------------------------------------
void stripe()
// Demonstrates animation of a diagonal stripe moving across the display
// with points plotted outside the display region ignored.
{
  const uint16_t maxCol = MAX_DEVICES*ROW_SIZE;
  const uint8_t  stripeWidth = 10;
  mx.clear();
  for (uint16_t col=0; col<maxCol + ROW_SIZE + stripeWidth; col++)
  {
    for (uint8_t row=0; row < ROW_SIZE; row++)
    {mx.setPoint(row, col-row, true);
      mx.setPoint(row, col-row - stripeWidth, false);
    }
    delay(20);
  }
   mx.update();
}
