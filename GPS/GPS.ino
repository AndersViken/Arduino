#include <Wire.h>
#include <LGPS.h>

gpsSentenceInfoStruct info;

char buff[256];
int num;
unsigned long t = 3000;
unsigned long change;
byte scrno;
boolean main = true;

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

void parseGPGGA(const char* GPGGAstr) {
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
    Serial.println(buff);

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
    Serial.println(buff);

    sprintf(buff, "Lat=%dd %5.4fm", latdeg, latmin);
    Serial.println(buff);

    sprintf(buff, "Lon=%dd %5.4fm", londeg, lonmin);
    Serial.println(buff);

    tmp = getComma(8, GPGGAstr);
    dilution = getDoubleNumber(&GPGGAstr[tmp]);
    sprintf(buff, "Horizontal dilution = %10.4f ", dilution);
    Serial.println(buff);

    tmp = getComma(9, GPGGAstr);
    alt = getDoubleNumber(&GPGGAstr[tmp]);
    sprintf(buff, "Altitude (meters) = %10.4f ", alt);
    Serial.println(buff);

    tmp = getComma(11, GPGGAstr);
    hg = getDoubleNumber(&GPGGAstr[tmp]);
    sprintf(buff, "Height from mean sea level (geoid) = %10.4f", hg);
    Serial.println(buff);
    Serial.println("====================================================================");

    if (millis() - change > t) {
      if (main) {
//        lcd.clear();
        sprintf(buff, "Lat: %dd %5.4fm", latdeg, latmin);
        Serial.print(buff);
        sprintf(buff, "Lon: %dd %5.4fm", londeg, lonmin);
        Serial.print(buff);

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
            Serial.print(buff);
            sprintf(buff, "H Dilution: %2.3f", dilution);
//            lcd.setCursor(0, 1);
            Serial.print(buff);
            break;
          case 2:
            sprintf(buff, "Altitude: %5.4f", alt);
            Serial.print(buff);
//            lcd.setCursor(0, 1);
            sprintf(buff, "UTC: %2d-%2d-%2d", hour, minute, second);
            Serial.print(buff);
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
    Serial.println("Not get data");
//    lcd.clear();
    Serial.print("No satellite");
  }
}

void setup() {
//  lcd.begin(16, 2);
  Serial.begin(115200);
  LGPS.powerOn();
  Serial.println("LGPS Power on, and waiting ...");
  delay(3000);
}

void loop() {
  LGPS.getData(&info);

  Serial.println("LGPS loop");
  Serial.print("Raw GPS Data: ");
  Serial.println((char*)info.GPGGA);

  parseGPGGA((const char*)info.GPGGA);
  delay(2000);
}
