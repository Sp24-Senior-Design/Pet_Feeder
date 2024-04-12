#include "IR.h"

void init_ir() {
  gpio_reset_pin(IR_SDA);                         
  gpio_set_direction(IR_SDA, GPIO_MODE_OUTPUT);
  gpio_reset_pin(IR_SCL);                         
  gpio_set_direction(IR_SCL, GPIO_MODE_OUTPUT);
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
  }
  i2c_write_bit(0); // ACK
  return value;
}

uint8_t i2c_read_bit() {

  uint8_t bit_val = 0;
  SDA_IN;

  SCL_HIGH;
  WAIT_DLY;
  bit_val |= gpio_get_level(IR_SDA);
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

void get_dist()
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

  // IR Sensor value (type: uint16_t)
  printf("(cm) DIST: %d", dist_val);
  // return dist_val;
}

void init_i2c(int reg)
{
  init_ir();
  // Intialize Communication
  i2c_start();
  i2c_write_addr();
  i2c_write_bit(0);

  bool ack = i2c_read_bit();
  if (ack != 0)
  {
    printf("Error with communication");
  }
  i2c_write_byte(reg);
  ack = i2c_read_bit();
  i2c_stop();
}