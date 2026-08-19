#include <Wire.h>

// ==========================
// PICO PINS
// ==========================

#define SDA_PIN 4
#define SCL_PIN 5

#define GREEN_LED 14   // PUFF
#define RED_LED   13   // SIP

// BMP280 I2C address
#define BMP280_ADDR 0x76

// ==========================
// BMP280 REGISTERS
// ==========================

#define REG_CHIP_ID     0xD0
#define REG_RESET       0xE0
#define REG_CTRL_MEAS   0xF4
#define REG_CONFIG      0xF5
#define REG_PRESS_MSB   0xF7


// WRITE TO BMP280


void bmp280Write(uint8_t reg, uint8_t value)
{
 Wire.beginTransmission(BMP280_ADDR);
 Wire.write(reg);
 Wire.write(value);
 Wire.endTransmission();
}


// READ FROM BMP280


void bmp280Read(uint8_t reg, uint8_t *buffer, uint8_t length)
{
 Wire.beginTransmission(BMP280_ADDR);
 Wire.write(reg);
 Wire.endTransmission(false);

 Wire.requestFrom(BMP280_ADDR, length);

 for (uint8_t i = 0; i < length; i++)
 {
   buffer[i] = Wire.read();
 }
}

// READ CHIP ID

uint8_t bmp280ReadID()
{
 uint8_t id;

 bmp280Read(REG_CHIP_ID, &id, 1);

 return id;
}

// READ RAW PRESSURE

int32_t bmp280ReadRawPressure()
{
 uint8_t data[3];

 bmp280Read(REG_PRESS_MSB, data, 3);

 int32_t pressure =
     ((int32_t)data[0] << 12) |
     ((int32_t)data[1] << 4) |
     (data[2] >> 4);

 return pressure;
}

// SETUP


void setup()
{
 Serial.begin(115200);

 delay(1000);

 Serial.println();
 Serial.println("================================");
 Serial.println("PNEUMATIC MOUSE");
 Serial.println("================================");



 pinMode(GREEN_LED, OUTPUT);
 pinMode(RED_LED, OUTPUT);

 digitalWrite(GREEN_LED, LOW);
 digitalWrite(RED_LED, LOW);


 Wire.setSDA(SDA_PIN);
 Wire.setSCL(SCL_PIN);

 Wire.begin();



 uint8_t chipID = bmp280ReadID();

 Serial.print("BMP280 Chip ID: 0x");
 Serial.println(chipID, HEX);

 if (chipID == 0x58)
 {
   Serial.println("BMP280 detected!");
 }
 else
 {
   Serial.println("ERROR: BMP280 not detected!");
 }



 bmp280Write(REG_CTRL_MEAS, 0x27);
 bmp280Write(REG_CONFIG, 0x00);

 delay(100);

 Serial.println("BMP280 initialized.");
 Serial.println();
}


// MAIN LOOP

void loop()
{
 int32_t pressure = bmp280ReadRawPressure();

 Serial.print("Raw pressure: ");
 Serial.println(pressure);

  // TEMPORARY TEST THRESHOLDS


 static int32_t baseline = 0;

 if (baseline == 0)
 {
   baseline = pressure;

   Serial.print("Baseline: ");
   Serial.println(baseline);
 }

 int32_t difference = pressure - baseline;

 Serial.print("Difference: ");
 Serial.println(difference);


 // PUFF


 if (difference > 1000)
 {
   Serial.println(">>> PUFF DETECTED <<<");

   digitalWrite(GREEN_LED, HIGH);
   digitalWrite(RED_LED, LOW);
 }

  // SIP


 else if (difference < -1000)
 {
   Serial.println(">>> SIP DETECTED <<<");

   digitalWrite(RED_LED, HIGH);
   digitalWrite(GREEN_LED, LOW);
 }

 // --------------------------
 // NORMAL
 // --------------------------

 else
 {
   digitalWrite(GREEN_LED, LOW);
   digitalWrite(RED_LED, LOW);
 }

 delay(100);
}
