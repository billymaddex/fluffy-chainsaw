/*
 * Final.c
 *
 * Created: 12/12/2019 6:47:07 PM
 * Author : Billy
 */ 

// 16MHz CPU clock
#define F_CPU 16000000UL
// 115200 baud rate setting
#define URRR_115200 8

#include <avr/io.h>
#include <util/atomic.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include "i2cmaster.h"

// APDS read and write addresses
#define APDS9960_WRITE 0x72
#define APDS9960_READ 0x73

// 15s timer variable, interval is 14 because we count from 0
const int INTERVAL = 14;
int transmit_time;

// variables to hold the sensor data
int16_t raw_R = 0;
int16_t raw_G = 0;
int16_t raw_B = 0;
int16_t raw_C = 0;

// variable to control data field for looping upload
// since only 1 value can be uploaded on each 15s interval, loop through all 4 of them
int field = 1;

// terminal output function declarations
void uart_init(unsigned int ubrr);
void uart_send_string(char *data);

// timer setup declaration
void timer_init(void);

// ESP01 setup declaration
void esp01_init(void);

// function to upload values
void upload_value(uint16_t value);

// APDS9960 sensor initialization
void init_APDS9960(void);

// get readings from the APDS9960 sensor
void getreading(void);

// TIMER1 CTC interrupt handler
ISR(TIMER1_COMPA_vect)
{
  // when transmit time is up, do something
  if (transmit_time-- == 0)
    {
      // send a value based on current field rotation
      if (field == 1)
	{
	  // red => field 1
	  upload_value(raw_R);
	  field++;
	}
      else if (field == 2)
	{
	  // blue => field 2
	  upload_value(raw_B);
	  field++;
	}
      else if (field == 3)
	{
	  // green => field 3
	  upload_value(raw_G);
	  field++;
	}
      else if (field == 4)
	{
	  // ambient light => field 4
	  upload_value(raw_C);
	  field = 1;
	}
      
      // reset transmit_time
      transmit_time = INTERVAL;
    }
}

int main(void)
{
  // configure terminal, baud rate 115200
  uart_init(URRR_115200);
  
  // configure the ESP01 module
  esp01_init();
  
  // initialize i2c
  i2c_init();
  
  // initialize sensor
  init_APDS9960();
  
  // start the timer
  timer_init();
  
  // enable global interrupt
  sei();
  
  // loop forever
  while (1)
    {
      getreading();
      
      _delay_ms(1000);
    }
}

void upload_value(uint16_t value)
{
  // output strings
  char data_size[20];
  char report[80];
  int report_len = 0;
  
  // generate output string from 'value' and set to upload to 'field' variable # field
  snprintf(report, sizeof(report), "GET /update?api_key=RGW36KTS9JXT0XM9&field%d=%d\r\n\r\n", field, value);
  
  // get number of bytes to send
  while (report[report_len] != '\0')
    {
      report_len++;
    }
  
  // generate SEND AT string based on byte size of report
  snprintf(data_size, sizeof(data_size), "AT+CIPSEND=%d\r\n", report_len);
  
  // output to terminal
  ATOMIC_BLOCK(ATOMIC_FORCEON)
    {
      uart_send_string("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n");
      uart_send_string(data_size);
      uart_send_string(report);
    }
}

// get readings from the APDS9960 sensor
void getreading(void){

  i2c_start(APDS9960_WRITE);
  // set pointer to CDATAL
  i2c_write(0x94);
  i2c_stop();
	
  // read CDATAL
  i2c_start(APDS9960_READ);
  raw_C =  ((uint8_t)i2c_readNak());
  i2c_stop();
	
  i2c_start(APDS9960_WRITE);
  // set pointer to CDATAH
  i2c_write(0x95);
  i2c_stop();
	
  // read CDATAH
  i2c_start(APDS9960_READ);
  raw_C |= ((uint8_t)i2c_readNak())<<8;
  i2c_stop();

  i2c_start(APDS9960_WRITE);
  // set pointer to RDATAL
  i2c_write(0x96);
  i2c_stop();
	
  // read RDATAL
  i2c_start(APDS9960_READ);
  raw_R = ((uint8_t)i2c_readNak());
  i2c_stop();
	
  i2c_start(APDS9960_WRITE);
  // set pointer to RDATAH
  i2c_write(0x97);
  i2c_stop();
	
  // read RDATAH
  i2c_start(APDS9960_READ);
  raw_R |= ((uint8_t)i2c_readNak())<<8;
  i2c_stop();

  i2c_start(APDS9960_WRITE);
  // set pointer to GDATAL
  i2c_write(0x98);
  i2c_stop();
	
  // read GDATAL
  i2c_start(APDS9960_READ);
  raw_G = ((uint8_t)i2c_readNak());
  i2c_stop();
	
  i2c_start(APDS9960_WRITE);
  // set pointer to GDATAH
  i2c_write(0x99);
  i2c_stop();
	
  // read GDATAH
  i2c_start(APDS9960_READ);
  raw_G |= ((uint8_t)i2c_readNak())<<8;
  i2c_stop();
	
  i2c_start(APDS9960_WRITE);
  // set pointer to BDATAL
  i2c_write(0x9A);
  i2c_stop();
	
  // read BDATAL
  i2c_start(APDS9960_READ);
  raw_B = ((uint8_t)i2c_readNak());
  i2c_stop();
	
  i2c_start(APDS9960_WRITE);
  // set pointer to BDATAH
  i2c_write(0x9B);
  i2c_stop();
	
  // read BDATAH
  i2c_start(APDS9960_READ);
  raw_B |= ((uint8_t)i2c_readNak())<<8;
  i2c_stop();
}

// configure the APDS9960 sensor
void init_APDS9960(void)
{
  // power up delay 150ms
  _delay_ms(150);
	
  // Enable Register, power on the device and the ALS
  i2c_start(APDS9960_WRITE);
  i2c_write(0x80);
  i2c_write(0x03);
  i2c_stop();

  // ADC Integration Time Register, 200ms, 72 cycles
  i2c_start(APDS9960_WRITE);
  i2c_write(0x81);
  i2c_write(0xB6);
  i2c_stop();

  // Control Register One, ALS and color gain = 1x
  i2c_start(APDS9960_WRITE);
  i2c_write(0x8F);
  i2c_write(0x00);
  i2c_stop();
}

// configure and activate the timer
void timer_init(void)
{
  // initialize transmit_time
  transmit_time = INTERVAL;
  
  // configure interrupt handling
  // enable TIMER1 COMPA interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  // set TIMER1 to CTC operations
  TCCR1B |= (1 << WGM12);
  // TIMER1 COMPA = X
  OCR1A = 15620;
  
  // start TIMER1 with prescalar = 1024
  TCCR1B |= (1 << CS12) | (1 << CS10);
}

// configure terminal output
void uart_init(unsigned int ubrr)
{
  // set buad rate based on ubrr
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
  
  // required delay between ESP01 UART commands
  _delay_ms(1800);
}

// configure ESP01 and connect to WiFi
void esp01_init(void)
{
  uart_send_string("AT\r\n");
  uart_send_string("AT+CWMODE=1\r\n");
  uart_send_string("AT+CIPSTA=\"10.7.7.22\"\r\n");
  uart_send_string("AT+CWJAP=\"Untempered Schism\",\"rycbar11\"\r\n");
  uart_send_string("AT+CIPMUX=0\r\n");
}
