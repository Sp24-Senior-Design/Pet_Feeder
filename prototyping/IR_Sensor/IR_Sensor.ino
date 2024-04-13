#include <Arduino.h>

#define TF_ADDR 0x10 // Default TF-LUNA address
// - - - -   Register Names and Numbers   - - - -
#define TFL_DIST_LO          0x00  //R Unit: cm
#define TFL_DIST_HI          0x01  //R
#define TFL_FLUX_LO          0x02  //R
#define TFL_FLUX_HI          0x03  //R
#define TFL_TEMP_LO          0x04  //R Unit: 0.01 Celsius
#define TFL_TEMP_HI          0x05  //R
#define TFL_TICK_LO          0x06  //R Timestamp
#define TFL_TICK_HI          0x07  //R
#define TFL_ERR_LO           0x08  //R
#define TFL_ERR_HI           0x09  //R
#define TFL_VER_REV          0x0A  //R
#define TFL_VER_MIN          0x0B  //R
#define TFL_VER_MAJ          0x0C  //R

#define TFL_SAVE_SETTINGS    0x20  //W -- Write 0x01 to save
#define TFL_SOFT_RESET       0x21  //W -- Write 0x02 to reboot.
                       // Lidar not accessible during few seconds,
                       // then register value resets automatically
#define TFL_SET_I2C_ADDR     0x22  //W/R -- Range 0x08,0x77.
                       // Must reboot to take effect.
#define TFL_SET_TRIG_MODE    0x23  //W/R -- 0-continuous, 1-trigger
#define TFL_TRIGGER          0x24  //W  --  1-trigger once
#define TFL_DISABLE          0x25  //W/R -- 0-disable, 1-enable
#define TFL_FPS_LO           0x26  //W/R -- lo byte
#define TFL_FPS_HI           0x27  //W/R -- hi byte
#define TFL_SET_LO_PWR       0x28  //W/R -- 0-normal, 1-low power
#define TFL_HARD_RESET       0x29  //W  --  1-restore factory settings
// - - - -                                - - - -

// 1:1000 10:100 (bps)
#define TIME_WAIT 100 // Frame Rate

#define SDA_PIN 13 // IO14
#define SCL_PIN 14 // IO12

#define SDA_IN pinMode(SDA_PIN, INPUT)
#define SDA_OUT pinMode(SDA_PIN, OUTPUT)
#define SDA_HIGH digitalWrite(SDA_PIN, HIGH)
#define SDA_LOW digitalWrite(SDA_PIN, LOW)
#define SCL_HIGH digitalWrite(SCL_PIN, HIGH)
#define SCL_LOW digitalWrite(SCL_PIN, LOW)

#define WAIT_DLY delay(TIME_WAIT);

void init_pins();
void i2c_start();
void i2c_write_addr();
void i2c_write_bit(int value);
void i2c_write_byte(uint8_t value);
uint8_t i2c_read_byte(int ack);
uint8_t i2c_read_bit(); 
int i2c_check();
void i2c_stop();
void init_i2c(int reg);
uint16_t get_dist();

void init_pins() {
  pinMode(SDA_PIN, OUTPUT);
  pinMode(SCL_PIN, OUTPUT);
}

void i2c_start() {
  SDA_HIGH;
  WAIT_DLY;
  SCL_HIGH;
  WAIT_DLY;
  SDA_LOW;
  WAIT_DLY;
  SCL_LOW;
  WAIT_DLY;
}

void i2c_write_addr() {
  // iterate through each bit to write a 1 or a 0
  for (uint8_t idx = 64; idx > 0; idx /= 2)
  {
    i2c_write_bit(TF_ADDR & idx);
  }
}

void i2c_write_bit(int value)
{
  (value != 0) ? SDA_HIGH : SDA_LOW;
  WAIT_DLY;
  SCL_HIGH;
  WAIT_DLY;
  SCL_LOW;
  WAIT_DLY;
}

void i2c_write_byte(uint8_t value) {
  // iterate through each bit to write a 1 or a 0
  for (uint8_t idx = 128; idx > 0; idx /= 2)
  {
    i2c_write_bit(value & idx);
  }
}

uint8_t i2c_read_byte() {
  uint8_t value = 0;

  // iterate through each bit to write a 1 or a 0
  for (uint8_t idx = 0; idx < 8; idx++)
  {
    uint8_t digit = i2c_read_bit();
    
    value |= digit;
    value = value << 1;
    Serial.print("Current IDX: ");
    Serial.print((idx));
    Serial.print("BIT: ");
    Serial.print((digit));
    Serial.print("Value: ");
    Serial.println((value));
  }
  Serial.println("BYTE");
  i2c_write_bit(0); // ACK
  return value;
}

uint8_t i2c_read_bit() {

  uint8_t bit_val = 0;
  SDA_IN;

  SCL_HIGH;
  WAIT_DLY;
  bit_val |= digitalRead(SDA_PIN);
  WAIT_DLY;
  SCL_LOW;
  WAIT_DLY;

  SDA_OUT;

  return bit_val;
}

void i2c_stop() {
  SDA_LOW;
  WAIT_DLY;
  SCL_HIGH;
  WAIT_DLY; 
  SDA_HIGH;
  WAIT_DLY;
}

uint16_t get_dist()
{
  init_i2c(TFL_DIST_LO);
  i2c_start();
  i2c_write_addr();
  i2c_write_bit(1); // Read bit
  uint8_t low_bits = i2c_read_byte();

  i2c_stop();
  
  init_i2c(TFL_DIST_HI);
  i2c_start();
  i2c_write_addr();
  i2c_write_bit(1); // Read bit
  uint8_t upper_bits = i2c_read_byte();
  i2c_stop();

  uint16_t dist_val = upper_bits << 8;
  dist_val |= low_bits;
  Serial.print("Lower: ");
  Serial.println(low_bits);
  Serial.print("Upper: ");
  Serial.println(upper_bits);
  Serial.print("Final: ");
  Serial.println(dist_val);
  return dist_val;
}

void init_i2c(int reg)
{
  Serial.println("I2C Bit-Banging");
  init_pins();
  // Intialize Communication
  i2c_start();
  i2c_write_addr();
  i2c_write_bit(0);

  bool ack = i2c_read_bit();
  Serial.print("ACK Check: ");
  Serial.print(ack);
  Serial.print("\n");

  i2c_write_byte(reg);
  ack = i2c_read_bit();
  Serial.print("ACK Check: ");
  Serial.print(ack);
  Serial.print("\n");
  i2c_stop();
}
  
void setup()
{
  Serial.begin(115200);
}

void loop()
{
  uint16_t dist = get_dist();
  Serial.print("(cm) DIST: ");
  Serial.println(dist);
}
