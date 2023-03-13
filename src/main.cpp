#include <Arduino.h>
#include <Wire.h>

// bq76925 I2C register address macros
#define I2C_GROUP_ADDR 0x04
// Register addresses 0x00 - 0x05 are Read / Write.
// Register addresses 0x07 and 0x10 - 0x16 are read only.
#define STATUS 0x00
#define CELL_CTL 0x01
#define BAL_CTL 0x02
#define CONFIG_1 0x03
#define CONFIG_2 0x04
#define POWER_CTL 0x05
#define CHIP_ID 0x07
#define VREF_CAL 0x10
#define VC1_CAL 0x11
#define VC2_CAL 0x12
#define VC3_CAL 0x13
#define VC4_CAL 0x14
#define VC5_CAL 0x15
#define VC6_CAL 0x16

char ADDRESS;
float VIOUTsensen;

void setup()
{
  // ADC setup pin A7
  pinMode(A7, INPUT);
  // ADC setup pin A7
  pinMode(A6, INPUT);
  // adc external reference voltage 3.3V
  // put your setup code here, to run once:
  // set up Wire1
  Wire1.begin();
  // set up serial
  Serial.begin(9600);
  // set up bq76925
  Wire1.beginTransmission((I2C_GROUP_ADDR << 3) + CELL_CTL);
  Wire1.write(0x10);
  Wire1.endTransmission();
  delay(10);

  // Wire1.beginTransmission((I2C_GROUP_ADDR << 3) + CONFIG_1);
  // Wire1.write(byte(0xE0));
  // Wire1.endTransmission();
  // delay(10);

  Wire1.beginTransmission((I2C_GROUP_ADDR << 3) + CONFIG_1);
  Wire1.write(0x00);
  Wire1.endTransmission();
  delay(5000);
  int adc = analogRead(A6);
  Serial.print("ADC: ");
  Serial.print(adc);
  VIOUTsensen = adc * (3.3 / 1023.0);
  Serial.print("  VIOUTsensen: ");
  Serial.print(VIOUTsensen);

  delay(50);

  Wire1.beginTransmission((I2C_GROUP_ADDR << 3) + CONFIG_2);
  Wire1.write(0x11);
  Wire1.endTransmission();
  delay(10);

  Wire1.beginTransmission((I2C_GROUP_ADDR << 3) + POWER_CTL);
  Wire1.write(B01011101);
  Wire1.endTransmission();
  delay(10);
  int test;
  Wire1.requestFrom((I2C_GROUP_ADDR << 3) + CONFIG_1, 1); // request 6 bytes from slave device #8
  while (Wire1.available())
  {                     // slave may send less than requested
    test = Wire1.read(); // receive a byte as character
  }

  delay(50);
  while (test != 0x00)
  {
    Wire1.requestFrom((I2C_GROUP_ADDR << 3) + CONFIG_1, 1); // request 6 bytes from slave device #8
    while (Wire1.available())
    {                     // slave may send less than requested
      test = Wire1.read(); // receive a byte as character
    }
    delay(50);
    Serial.print("test: ");
    Serial.print(test);
    Serial.println("  rip");
  }
  
  Wire1.beginTransmission((I2C_GROUP_ADDR << 3) + CONFIG_1);
  Wire1.write(0x05);
  Wire1.endTransmission();
  delay(10);
}

void loop()
{
  // adc read and print to serial

  // convert to voltage
  float VCOUT;
  float VIOUTsensep;
  float Vsense;
  float Isense;
  float Rsense = 0.001;
  float VC1, VC2, VC3, VC4;
  float VC1cal, VC2cal, VC3cal, VC4cal;
  float Vref;
  float GCvc1, GCvc2, GCvc3, GCvc4;
  float OCvc1, OCvc2, OCvc3, OCvc4;
  float GCref;
  float VCtotal;

  // Serial.println(VCOUT);
  // read from bq76925 on ADDRESS
  int gain;
  Wire1.requestFrom((I2C_GROUP_ADDR << 3) + CONFIG_1, 1); // request 6 bytes from slave device #8
  while (Wire1.available())
  {              // slave may send less than requested
    gain = Wire1.read(); // receive a byte as character
  }

  delay(50);
  /*
    Wire1.requestFrom((I2C_GROUP_ADDR << 3) + VREF_CAL, 1); // request 6 bytes from slave device #8
    while (Wire1.available())
    {                     // slave may send less than requested
      Vref = Wire1.read(); // receive a byte as character
    }
    delay(50);

    Wire1.requestFrom((I2C_GROUP_ADDR << 3) + POWER_CTL, 1); // request 6 bytes from slave device #8
    while (Wire1.available())
    {              // slave may send less than requested
      Wire1.read(); // receive a byte as character
    }
    delay(50);

    Wire1.beginTransmission((I2C_GROUP_ADDR << 3) + CELL_CTL);
    Wire1.write(0x10);
    Wire1.endTransmission();
    Wire1.requestFrom((I2C_GROUP_ADDR << 3) + CELL_CTL, 1); // request 6 bytes from slave device #8
    while (Wire1.available())
    {              // slave may send less than requested
      Wire1.read(); // receive a byte as character
    }
  */

  delay(100);

  int adc = analogRead(A6);
  Serial.print("ADC: ");
  Serial.print(adc);
  VIOUTsensep = adc * (3.3 / 1023.0);
  Serial.print("  VIOUTsensep: ");
  Serial.print(VIOUTsensep);
  if (gain == 0x00)
  {
    Vsense = -(VIOUTsensep - VIOUTsensen) / 4.0;
  }
  else if (gain == 0x04)
  {
    Vsense = -(VIOUTsensep - VIOUTsensen) / 8.0;
  }
  
  Serial.print("  Vsense: ");
  Serial.print(Vsense);
  Isense = Vsense / Rsense;
  Serial.print("  Isense: ");
  Serial.println(Isense);

  delay(50);
  /*
  Wire1.beginTransmission((I2C_GROUP_ADDR << 3) + CELL_CTL);
  Wire1.write(0x10);
  Wire1.endTransmission();
  delay(10);
  adc = analogRead(A7);

  VCOUT = adc * (3.3 / 1023.0);
  VC1 = (VCOUT / 0.6);
  Serial.print("VC1: ");
  Serial.println(VC1);

  Wire1.beginTransmission((I2C_GROUP_ADDR << 3) + CELL_CTL);
  Wire1.write(0x11);
  Wire1.endTransmission();
  delay(10);

  adc = analogRead(A7);
  VCOUT = adc * (3.3 / 1023.0);
  VC2 = (VCOUT / 0.6);
  Serial.print("VC2: ");
  Serial.println(VC2);
  // do the same thing for VC3, VC4, VC5, VC6
  Wire1.beginTransmission((I2C_GROUP_ADDR << 3) + CELL_CTL);
  Wire1.write(0x12);
  Wire1.endTransmission();
  delay(10);

  adc = analogRead(A7);
  VCOUT = adc * (3.3 / 1023.0);
  VC3 = (VCOUT / 0.6);
  Serial.print("VC3: ");
  Serial.println(VC3);

  Wire1.beginTransmission((I2C_GROUP_ADDR << 3) + CELL_CTL);
  Wire1.write(0x13);
  Wire1.endTransmission();
  delay(10);

  adc = analogRead(A7);
  VCOUT = adc * (3.3 / 1023.0);
  VC4 = (VCOUT / 0.6);
  Serial.print("VC4: ");
  Serial.println(VC4);

  Serial.print("Vctotal: ");
  VCtotal = VC1 + VC2 + VC3 + VC4;
  Serial.println(VCtotal);
  */
  delay(500);
}