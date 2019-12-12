/*
 * DA6.c
 *
 * Created: 12/8/2019 10:37:43 PM
 * Author : Billy
 */ 

// 16MHz CPU clock
#define F_CPU 16000000UL
// 115200 baud rate setting
#define URRR_115200 8
// 9600 baud rate setting
#define UBRR_9600 103

#include <avr/io.h>
#include <util/atomic.h>
#include <util/delay.h>
#include <stdio.h>
#include "i2cmaster.h"
#include "MPU6050_def.h"

// MPU6050 read and write addresses
#define MPU6050_WRITE 0xD0
#define MPU6050_READ 0xD1

// terminal output function declarations
void uart_init(unsigned int ubrr);
void uart_send_string(char *data);

// MPU6050 function declarations
void init_MPU6050(void);
float get_reading(unsigned char data);

int main(void)
{
  // configure terminal
  uart_init(URRR_115200);
  
  // initialize i2c
  i2c_init();
  
  // initialize MPU6050
  init_MPU6050();
  
  // variables to store data from MPU6050
  float acl_x, acl_y, acl_z;
  float gyr_x, gyr_y, gyr_z;
  
  // output string for terminal display
  char out_string[200];
  
  // loop forever collecting and formatting the data, output to terminal
  while (1)
    {
      // output every 1 second
      _delay_ms(1000);
      
      // update all variables with readings from the peripheral
      acl_x = get_reading(ACCEL_XOUT_H);
      _delay_ms(200);
      acl_y = get_reading(ACCEL_YOUT_H);
      _delay_ms(200);
      acl_z = get_reading(ACCEL_ZOUT_H);
      _delay_ms(200);
      gyr_x = get_reading(GYRO_XOUT_H);
      _delay_ms(200);
      gyr_y = get_reading(GYRO_YOUT_H);
      _delay_ms(200);
      gyr_z = get_reading(GYRO_ZOUT_H);
      
      // generate output string for acl_x and send to terminal
      snprintf(out_string, sizeof(out_string), "Accel X:\t%.0f\r\nAccel Y:\t%.0f\r\nAccel Z:\t%.0f\r\nGyro X:\t\t%.0f\r\nGyro Y:\t\t%.0f\r\nGyro Z:\t\t%.0f\r\n\r\n", acl_x, acl_y, acl_z, gyr_x, gyr_y, gyr_z);
      
      // send output string to terminal
      ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
	  uart_send_string(out_string);
	}
    }
}

// configure terminal output
void uart_init(unsigned int ubrr)
{
  // set buad rate to 9600
  UBRR0H = (unsigned char)(ubrr>>8);
  UBRR0L = (unsigned char)ubrr;
  // set terminal to output
  UCSR0B = (1 << TXEN0);
  // set output mode
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_send_string(char *data)
{
  while((*data != '\0'))
    {
      while (!(UCSR0A & (1 <<UDRE0)));
      UDR0 = *data;
      data++;
    }
}

// get reading for MPU6050 register specified by pointer 'data'
float get_reading(unsigned char data)
{
  float value;
  
  i2c_start(MPU6050_WRITE);
  i2c_write(data);
  i2c_stop();

  i2c_start(MPU6050_READ);
  //value = (((int)i2c_readAck()<<8) | (int)i2c_readAck());
  int high = ((int)i2c_readAck()<<8);
  value = high | (int)i2c_readNak();
  i2c_stop();
  
  return value;
}

// initialize MPU6050 peripheral
void init_MPU6050(void)
{
  // power up delay
  _delay_ms(150);
	
  // reset all registers in the peripheral
  i2c_start(MPU6050_WRITE);
  i2c_write(PWR_MGMT_1);
  i2c_write(0x80);
  i2c_stop();
	
  // reset delay
  _delay_ms(900);
	
  // set sample rate divider to 8, sample rate = 8kHz
  i2c_start(MPU6050_WRITE);
  i2c_write(SMPLRT_DIV);
  i2c_write(0x07);
  i2c_stop();

  // set clock source to internal 8MHz and disable temp sensor
  i2c_start(MPU6050_WRITE);
  i2c_write(PWR_MGMT_1);
  i2c_write(0x08);
  i2c_stop();

  // DLPF configured for 8kHz
  i2c_start(MPU6050_WRITE);
  i2c_write(CONFIG);
  i2c_write(0x00);
  i2c_stop();
	
  // gyro configuration, set to 500 degrees per second
  i2c_start(MPU6050_WRITE);
  i2c_write(GYRO_CONFIG);
  i2c_write(0x08);
  i2c_stop();

  // accelerometer configuration, set to 8g
  i2c_start(MPU6050_WRITE);
  i2c_write(ACCEL_CONFIG);
  i2c_write(0x10);
  i2c_stop();

  // enable the data ready interrupt
  //i2c_start(MPU6050_WRITE);
  //i2c_write(INT_ENABLE);
  //i2c_write(0x01);
  //i2c_stop();
}
