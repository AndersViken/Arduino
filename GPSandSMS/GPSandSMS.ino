#include <LGSM.h> 
#include <Wire.h>
#include <LGPS.h>


gpsSentenceInfoStruct info;

char buff[256];
int num;
unsigned long t = 3000;
unsigned long change;
byte scrno;
boolean main = true;

#define LED_BUILTIN 13

static unsigned char getComma(unsigned char num, const char *str) {
  unsigned char i, j = 0;
  int len = strlen(str);
  for (i = 0; i < len; i ++)
  {
    if (str[i] == ',')
      j++;
    if (j == num)
      return i + 1;
  }
  return 0;
}

static double getDoubleNumber(const char *s) {
  char buf[10];
  unsigned char i;
  double rev;

  i = getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev = atof(buf);
  return rev;
}

static double getIntNumber(const char *s) {
  char buf[10];
  unsigned char i;
  double rev;

  i = getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev = atoi(buf);
  return rev;
}

int getNumSatellites(const char* GPGGAstr) {
  double latitude;
  double longitude;
  double dilution, alt, hg;
  int tmp, hour, minute, second;
  if (GPGGAstr[0] == '$')
  {
    tmp = getComma(1, GPGGAstr);
    hour     = (GPGGAstr[tmp + 0] - '0') * 10 + (GPGGAstr[tmp + 1] - '0');
    minute   = (GPGGAstr[tmp + 2] - '0') * 10 + (GPGGAstr[tmp + 3] - '0');
    second    = (GPGGAstr[tmp + 4] - '0') * 10 + (GPGGAstr[tmp + 5] - '0');

    sprintf(buff, "UTC timer %2d-%2d-%2d", hour, minute, second);

    tmp = getComma(2, GPGGAstr);
    latitude = getDoubleNumber(&GPGGAstr[tmp]);
    tmp = getComma(4, GPGGAstr);
    longitude = getDoubleNumber(&GPGGAstr[tmp]);

    int latdeg = latitude / 100;
    int londeg = longitude / 100;
    double latmin = latitude - (((double)latdeg) * 100);
    double lonmin = longitude - (((double)londeg) * 100);

    tmp = getComma(7, GPGGAstr);
    return getIntNumber(&GPGGAstr[tmp]);
  }
}

void sendSMS(const char* GPGGAstr)
{
  Serial.println("Sending SMS.\n");
  

  double latitude;
  double longitude;
  double dilution, alt, hg;
  int tmp, hour, minute, second;
  if (GPGGAstr[0] == '$') {
    tmp = getComma(1, GPGGAstr);
    hour     = (GPGGAstr[tmp + 0] - '0') * 10 + (GPGGAstr[tmp + 1] - '0');
    minute   = (GPGGAstr[tmp + 2] - '0') * 10 + (GPGGAstr[tmp + 3] - '0');
    second    = (GPGGAstr[tmp + 4] - '0') * 10 + (GPGGAstr[tmp + 5] - '0');

    sprintf(buff, "UTC timer %2d-%2d-%2d", hour, minute, second);
    LSMS.println(buff);

    tmp = getComma(2, GPGGAstr);
    latitude = getDoubleNumber(&GPGGAstr[tmp]);
    tmp = getComma(4, GPGGAstr);
    longitude = getDoubleNumber(&GPGGAstr[tmp]);

    int latdeg = latitude / 100;
    int londeg = longitude / 100;
    double latmin = latitude - (((double)latdeg) * 100);
    double lonmin = longitude - (((double)londeg) * 100);

    tmp = getComma(7, GPGGAstr);
    num = getIntNumber(&GPGGAstr[tmp]);
    sprintf(buff, "satellites number = %d ", num);
    LSMS.println(buff);



    sprintf(buff, "Hei på deg.");
    LSMS.println(buff);

    sprintf(buff, "Lat=%dd %5.4fm", latdeg, latmin);
    LSMS.println(buff);

    sprintf(buff, "Lon=%dd %5.4fm", londeg, lonmin);
    LSMS.println(buff);

    tmp = getComma(8, GPGGAstr);
    dilution = getDoubleNumber(&GPGGAstr[tmp]);
    sprintf(buff, "Horizontal dilution = %10.4f ", dilution);
    LSMS.println(buff);

    tmp = getComma(9, GPGGAstr);
    alt = getDoubleNumber(&GPGGAstr[tmp]);
    sprintf(buff, "Altitude (meters) = %10.4f ", alt);
    LSMS.println(buff);

    tmp = getComma(11, GPGGAstr);
    hg = getDoubleNumber(&GPGGAstr[tmp]);
    sprintf(buff, "Height from mean sea level (geoid) = %10.4f", hg);
    //Serial.println(buff);
    LSMS.println("====================================================================");

    if (millis() - change > t) {
      if (main) {
//        lcd.clear();
        sprintf(buff, "Lat: %dd %5.4fm", latdeg, latmin);
        LSMS.print(buff);
        sprintf(buff, "Lon: %dd %5.4fm", londeg, lonmin);
        LSMS.print(buff);

        t = 3000;
        scrno++;
        if (scrno > 2) {
          scrno = 1;
        }
        main = false;
      }
      else {
//        lcd.clear();
        switch (scrno) {
          case 1:
            sprintf(buff, "Satellites: %d", num);
            LSMS.print(buff);
            sprintf(buff, "H Dilution: %2.3f", dilution);
//            lcd.setCursor(0, 1);
            LSMS.print(buff);
            break;
          case 2:
            sprintf(buff, "Altitude: %5.4f", alt);
            LSMS.print(buff);
//            lcd.setCursor(0, 1);
            sprintf(buff, "UTC: %2d-%2d-%2d", hour, minute, second);
            LSMS.print(buff);
            break;
        }
        t = 1000;
        main = true;
      }
      change = millis();
    }
  }
  else
  {
    LSMS.println("Not get data");
//    lcd.clear();
    LSMS.print("No satellite");
  }

  LSMS.endSMS();
  
}
void parseGPGGA(const char* GPGGAstr)
{
  double latitude;
  double longitude;
  double dilution, alt, hg;
  int tmp, hour, minute, second;
  if (GPGGAstr[0] == '$') {
    tmp = getComma(1, GPGGAstr);
    hour     = (GPGGAstr[tmp + 0] - '0') * 10 + (GPGGAstr[tmp + 1] - '0');
    minute   = (GPGGAstr[tmp + 2] - '0') * 10 + (GPGGAstr[tmp + 3] - '0');
    second    = (GPGGAstr[tmp + 4] - '0') * 10 + (GPGGAstr[tmp + 5] - '0');

    sprintf(buff, "UTC timer %2d-%2d-%2d", hour, minute, second);
    //Serial.println(buff);

    tmp = getComma(2, GPGGAstr);
    latitude = getDoubleNumber(&GPGGAstr[tmp]);
    tmp = getComma(4, GPGGAstr);
    longitude = getDoubleNumber(&GPGGAstr[tmp]);

    int latdeg = latitude / 100;
    int londeg = longitude / 100;
    double latmin = latitude - (((double)latdeg) * 100);
    double lonmin = longitude - (((double)londeg) * 100);

    tmp = getComma(7, GPGGAstr);
    num = getIntNumber(&GPGGAstr[tmp]);
    sprintf(buff, "satellites number = %d ", num);
    //Serial.println(buff);

    sprintf(buff, "Lat=%dd %5.4fm", latdeg, latmin);
    //Serial.println(buff);

    sprintf(buff, "Lon=%dd %5.4fm", londeg, lonmin);
    //Serial.println(buff);

    tmp = getComma(8, GPGGAstr);
    dilution = getDoubleNumber(&GPGGAstr[tmp]);
    sprintf(buff, "Horizontal dilution = %10.4f ", dilution);
    //Serial.println(buff);

    tmp = getComma(9, GPGGAstr);
    alt = getDoubleNumber(&GPGGAstr[tmp]);
    sprintf(buff, "Altitude (meters) = %10.4f ", alt);
    //Serial.println(buff);

    tmp = getComma(11, GPGGAstr);
    hg = getDoubleNumber(&GPGGAstr[tmp]);
    sprintf(buff, "Height from mean sea level (geoid) = %10.4f", hg);
    //Serial.println(buff);
    //Serial.println("====================================================================");

    if (millis() - change > t) {
      if (main) {
//        lcd.clear();
        sprintf(buff, "Lat: %dd %5.4fm", latdeg, latmin);
        //Serial.print(buff);
        sprintf(buff, "Lon: %dd %5.4fm", londeg, lonmin);
        //Serial.print(buff);

        t = 3000;
        scrno++;
        if (scrno > 2) {
          scrno = 1;
        }
        main = false;
      }
      else {
//        lcd.clear();
        switch (scrno) {
          case 1:
            sprintf(buff, "Satellites: %d", num);
            //Serial.print(buff);
            sprintf(buff, "H Dilution: %2.3f", dilution);
//            lcd.setCursor(0, 1);
            //Serial.print(buff);
            break;
          case 2:
            sprintf(buff, "Altitude: %5.4f", alt);
            //Serial.print(buff);
//            lcd.setCursor(0, 1);
            sprintf(buff, "UTC: %2d-%2d-%2d", hour, minute, second);
            //Serial.print(buff);
            break;
        }
        t = 1000;
        main = true;
      }
      change = millis();
    }
  }
  else
  {
    //Serial.println("Not get data");
//    lcd.clear();
    //Serial.print("No satellite");
  }
}




// Include Linkit ONE GSM library
String startBuffer;                                               // Stores data entered to start the Send SMS program
String messageConfirm = "What do you want to send?";              // Asks user for input and stores it in the messageConfirm variable
String message;                                                   // Variable to store message to be sent
String confirmSend = "Do you want to send the SMS (Yes or No)?";  // Stores confirmation for the SMS to be sent
String confirmReply;                                              // Variable to store users input to confirm sending SMS

void setup()
{
  
  // put your setup code here, to run once:
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
//  Serial.begin(115200);                 // Initializes serial port at 115200 bauds
  
//  while (Serial.available() == 0) {}    // Wait until user inputs data and start the program when data is received
//  startBuffer = Serial.readString();    // Save the users input in startBuffer variable

  Serial.println("Starting Send SMS!"); // Start the Send SMS program
  
  while (!LSMS.ready())                 // Wait for the sim to initialize
  {
    delay(1000);                        // Wait for a second and then try again
  }
  
  Serial.println("Sim initialized");    // When SIM is started, print "Sim initialized" in the serial port

  LSMS.beginSMS("97404115");          // Saves the number where user wants to send SMS. To be changed before uploading sketch


  LGPS.powerOn();
  Serial.println("LGPS Power on, and waiting ...");
  delay(3000);
}

bool bLedOn = false;
void toggleLed()
{
  if(bLedOn)
  {
    Serial.println("LedOff.\n");
    bLedOn = false;
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  }
  else
  {
    Serial.println("LedOn.\n");
    bLedOn = true;
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  }
   
}
bool bSmsSent = false;
int second = 0;
void loop()
{

  LGPS.getData(&info);

  //Serial.println("LGPS loop");
  //Serial.print("Raw GPS Data: ");
  //Serial.println((char*)info.GPGGA);
  char secondText[100];
  sprintf(secondText, "%d", second++);
  //Serial.println(secondText);
  const int nNumSatellites = getNumSatellites((const char*)info.GPGGA);
  if(second % 60 == 0)//nNumSatellites > 0)
  {
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
        parseGPGGA((const char*)info.GPGGA);
        if(1)//bSmsSent == false)
        {
            bSmsSent = true;
            sendSMS((const char*)info.GPGGA);
        }
  }
  else
  {
    toggleLed();
  }
    
  delay(500);
}
