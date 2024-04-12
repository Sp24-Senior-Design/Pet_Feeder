#ifndef IR_h
#define IR_h

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define IR_SDA GPIO_NUM_14 // SDA
#define IR_SCL GPIO_NUM_4 // SCL
#define SDA_IN gpio_reset_pin(IR_SDA); gpio_set_direction(IR_SDA, GPIO_MODE_INPUT)
#define SDA_OUT gpio_reset_pin(IR_SDA); gpio_set_direction(IR_SDA, GPIO_MODE_OUTPUT)
#define SDA_HIGH gpio_set_level(IR_SDA, 1)
#define SDA_LOW gpio_set_level(IR_SDA, 0)
#define SCL_HIGH gpio_set_level(IR_SCL, 1)
#define SCL_LOW gpio_set_level(IR_SCL, 0)
#define TIME_WAIT 1 //
#define WAIT_DLY vTaskDelay(pdMS_TO_TICKS(TIME_WAIT));
#define TF_ADDR 0x10 // Default TF-LUNA address
#define TFL_DIST_LO 0x00  //R Unit: cm
#define TFL_DIST_HI 0x01  //R

void init_ir();
void i2c_start();
void i2c_write_addr();
void i2c_write_bit(int value);
void i2c_write_byte(uint8_t value);
uint8_t i2c_read_byte();
uint8_t i2c_read_bit(); 
int i2c_check();
void i2c_stop();
void init_i2c(int reg);

#endif