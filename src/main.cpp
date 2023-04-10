#include <Arduino.h>
#include <Wire.h>

// PIN SDA1 = PA18 SCL1 = PA17
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
#define VC_CAL_EXT_1 0x17
#define VC_CAL_EXT_2 0x18
#define VREF_CAL_EXT 0x1B

const double Voffset = 0.0275;
const double B_MAX = 16.8;
const double B_MIN = 12.8;
const double Gvcout = 0.6;

char ADDRESS;
float VIOUTsensen;

double GCvc1, GCvc2, GCvc3, GCvc4;
double OCvc1, OCvc2, OCvc3, OCvc4;
double GCref;

int VC1_GC_4, VC2_GC_4, VC3_GC_4, VC4_GC_4;
int VC1_OC_4, VC2_OC_4, VC3_OC_4, VC4_OC_4;
int VREF_GC_4, VREF_OC_4, VREF_OC_5;
int VC1_GAIN_CORR, VC2_GAIN_CORR, VC3_GAIN_CORR, VC4_GAIN_CORR;
int VREF_GAIN_CORR;
int VC1_OFFSET_CORR, VC2_OFFSET_CORR, VC3_OFFSET_CORR, VC4_OFFSET_CORR;
int VREF_OFFSET_CORR;

int test;

void setup()
{
  // ADC setup pin A0 Temp
  pinMode(A0, INPUT);
  // ADC setup pin A1 Courant
  pinMode(A1, INPUT);
  // ADC setup pin A2 Volt
  pinMode(A2, INPUT);
  analogReadResolution(12); // adc resolution 12 bits

  // adc external reference voltage 3.3V
  // put your setup code here, to run once:
  // set up Wire1
  Wire1.begin();
  // set up serial
  Serial.begin(115200);

  Wire1.beginTransmission((I2C_GROUP_ADDR << 3) + POWER_CTL);
  Wire1.write(0x5C);
  Wire1.endTransmission();
  delay(10);

  Wire1.beginTransmission((I2C_GROUP_ADDR << 3) + CONFIG_1);
  Wire1.write(0x00);
  Wire1.endTransmission();

  delay(10);

  Wire1.beginTransmission((I2C_GROUP_ADDR << 3) + CONFIG_2);
  Wire1.write(0x01);
  Wire1.endTransmission();
  delay(10);

  Wire1.requestFrom((I2C_GROUP_ADDR << 3) + CONFIG_1, 1); // request 6 bytes from slave device #8
  while (Wire1.available())
  {                      // slave may send less than requested
    test = Wire1.read(); // receive a byte as character
  }

  delay(500);
  while (test != 0x00)
  {
    Wire1.requestFrom((I2C_GROUP_ADDR << 3) + CONFIG_1, 1); // request 6 bytes from slave device #8
    while (Wire1.available())
    {                      // slave may send less than requested
      test = Wire1.read(); // receive a byte as character
    }
    delay(500);
    Serial.print("test: ");
    Serial.print(test);
    Serial.println("  rip");
  }

  Wire1.beginTransmission((I2C_GROUP_ADDR << 3) + CONFIG_1);
  Wire1.write(0x05);
  Wire1.endTransmission();
  delay(10);

  // Wire1.requestFrom((I2C_GROUP_ADDR << 3) + VREF_CAL, 1);
  // while (Wire1.available())
  // {
  //   int temp = Wire1.read();
  //   VREF_OFFSET_CORR = temp >> 4;
  //   VREF_GAIN_CORR = temp & 0x0F;
  // }
  // delay(10);
  // Wire1.requestFrom((I2C_GROUP_ADDR << 3) + VC1_CAL, 1);
  // while (Wire1.available())
  // {
  //   int temp = Wire1.read();
  //   VC1_OFFSET_CORR = temp >> 4;
  //   VC1_GAIN_CORR = temp & 0x0F;
  // }
  // delay(10);
  // Wire1.requestFrom((I2C_GROUP_ADDR << 3) + VC2_CAL, 1);
  // while (Wire1.available())
  // {
  //   int temp = Wire1.read();
  //   VC2_OFFSET_CORR = temp >> 4;
  //   VC2_GAIN_CORR = temp & 0x0F;
  // }
  // delay(10);
  // Wire1.requestFrom((I2C_GROUP_ADDR << 3) + VC3_CAL, 1);
  // while (Wire1.available())
  // {
  //   int temp = Wire1.read();
  //   VC3_OFFSET_CORR = temp >> 4;
  //   VC3_GAIN_CORR = temp & 0x0F;
  // }
  // delay(10);
  // Wire1.requestFrom((I2C_GROUP_ADDR << 3) + VC4_CAL, 1);
  // while (Wire1.available())
  // {
  //   int temp = Wire1.read();
  //   VC4_OFFSET_CORR = temp >> 4;
  //   VC4_GAIN_CORR = temp & 0x0F;
  // }
  // delay(10);
  // Wire1.requestFrom((I2C_GROUP_ADDR << 3) + VC_CAL_EXT_1, 1);
  // while (Wire1.available())
  // {
  //   int temp = Wire1.read();
  //   VC1_OC_4 = temp & B10000000;
  //   VC1_GC_4 = temp & B01000000;
  //   VC2_OC_4 = temp & B00100000;
  //   VC2_GC_4 = temp & B00010000;
  // }
  // delay(10);
  // Wire1.requestFrom((I2C_GROUP_ADDR << 3) + VC_CAL_EXT_2, 1);
  // while (Wire1.available())
  // {
  //   int temp = Wire1.read();
  //   VC3_OC_4 = temp & B10000000;
  //   VC3_GC_4 = temp & B01000000;
  //   VC4_OC_4 = temp & B00100000;
  //   VC4_GC_4 = temp & B00010000;
  // }
  // delay(10);
  // Wire1.requestFrom((I2C_GROUP_ADDR << 3) + VREF_CAL_EXT, 1);
  // while (Wire1.available())
  // {
  //   int temp = Wire1.read();
  //   VREF_OC_5 = temp & B00000100;
  //   VREF_OC_4 = temp & B00000010;
  //   VREF_GC_4 = temp & B00000001;
  // }
  // delay(10);
  // GCref = (1 + ((VREF_GC_4 << 4) + VREF_GAIN_CORR) * 0.001) + (((VREF_OC_5 << 5) + (VREF_OC_4 << 4) + VREF_OFFSET_CORR) * 0.001) / 3;
  // GCvc1 = ((VC1_GC_4 << 4) + VC1_GAIN_CORR) * 0.001;
  // OCvc1 = ((VC1_OC_4 << 4) + VC1_OFFSET_CORR) * 0.001;
  // GCvc2 = ((VC2_GC_4 << 4) + VC2_GAIN_CORR) * 0.001;
  // OCvc2 = ((VC2_OC_4 << 4) + VC2_OFFSET_CORR) * 0.001;
  // GCvc3 = ((VC3_GC_4 << 4) + VC3_GAIN_CORR) * 0.001;
  // OCvc3 = ((VC3_OC_4 << 4) + VC3_OFFSET_CORR) * 0.001;
  // GCvc4 = ((VC4_GC_4 << 4) + VC4_GAIN_CORR) * 0.001;
  // OCvc4 = ((VC4_OC_4 << 4) + VC4_OFFSET_CORR) * 0.001;
}

void loop()
{
  double VCOUT;
  double Isense;
  double Rsense = 0.001;
  double VC1, VC2, VC3, VC4;
  double Vref;

  double VCtotal;
  uint32_t adc;
  double pourcentage_batterie;
  int gain;

  Wire1.beginTransmission((I2C_GROUP_ADDR << 3) + CELL_CTL);
  Wire1.write(0x10);
  Wire1.endTransmission();
  delay(10);
  adc = analogRead(A2);

  VCOUT = adc * (3.3 / 4096.0) - Voffset;
  VC1 = VCOUT / Gvcout;
  // VC1 = ((VCOUT * GCref + OCvc1)/Gvcout)* (1 + GCvc1);

  Wire1.beginTransmission((I2C_GROUP_ADDR << 3) + CELL_CTL);
  Wire1.write(0x11);
  Wire1.endTransmission();
  delay(10);

  adc = analogRead(A2);
  VCOUT = adc * (3.3 / 4096.0) - Voffset;
  VC2 = VCOUT / Gvcout;
  // VC2 = ((VCOUT * GCref + OCvc2) / Gvcout) * (1 + GCvc2);

  Wire1.beginTransmission((I2C_GROUP_ADDR << 3) + CELL_CTL);
  Wire1.write(0x12);
  Wire1.endTransmission();
  delay(10);

  adc = analogRead(A2);
  VCOUT = adc * (3.3 / 4096.0) - Voffset;
  VC3 = VCOUT / Gvcout;
  // VC3 = ((VCOUT * GCref + OCvc3) / Gvcout) * (1 + GCvc3);

  Wire1.beginTransmission((I2C_GROUP_ADDR << 3) + CELL_CTL);
  Wire1.write(0x13);
  Wire1.endTransmission();
  delay(10);

  adc = analogRead(A2);
  VCOUT = adc * (3.3 / 4096.0) - Voffset;
  VC4 = VCOUT / Gvcout;
  // VC4 = ((VCOUT * GCref + OCvc4) / Gvcout) * (1 + GCvc4);
  delay(10);
  VCtotal = VC1 + VC2 + VC3 + VC4;
  pourcentage_batterie = (VCtotal - B_MIN) / (B_MAX - B_MIN) * 100;

  Serial.print("VC1: ");
  Serial.print(VC1);
  Serial.print(" VC2: ");
  Serial.print(VC2);
  Serial.print(" VC3: ");
  Serial.print(VC3);
  Serial.print(" VC4: ");
  Serial.print(VC4);
  Serial.print(" Vctotal: ");
  Serial.print(VCtotal);
  Serial.print(" pourcentage batterie: ");
  Serial.println(pourcentage_batterie);
}