/*
 * Midterm.c
 *
 * Created: 10/27/2019 12:50:46 PM
 * Author : Billy
 */ 

// 16MHz CPU clock
#define F_CPU 16000000UL
// 115200 baud rate setting
#define URRR_115200 8

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>

// 15s timer variable, interval is 14 because we count from 0
const int INTERVAL = 14;
int transmit_time;

// terminal setup and output function declarations
void uart_init(unsigned int ubrr);
void uart_send_string(char *data);

// ADC function declarations
void adc_init(void);

// ESP01 setup declaration
void esp01_init(void);

// function to transmit temperature value
void xmit_temp(float temp);

// TIMER1 CTC interrupt handler
ISR(TIMER1_COMPA_vect)
{
  // when transmit time is up, do something
  if (transmit_time-- == 0)
    {
      // start ADC conversion
      ADCSRA |= (1 << ADSC);
		
      // reset transmit_time
      transmit_time = INTERVAL;
    }
}

ISR(ADC_vect)
{
  // collect and compute ADC value
  // value stored in ADC
  float cel = ((5.0 * ADC) / 1000.0) + 4;
  float fahr = (1.8 * cel) + 32;
  
  // transmit the temp data
  xmit_temp(fahr);
}

int main(void)
{
  // configure terminal
  uart_init(URRR_115200);

  // configure and start ADC
  adc_init();
	
  // configure ESP01 and connect to WiFi
  esp01_init();
	
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

  // enable global interrupt
  sei();
	
  // loop forever
  while (1);
}

// configure terminal output
void uart_init(unsigned int ubrr)
{
  // set baud rate
  UBRR0H = (unsigned char)(ubrr>>8);
  UBRR0L = (unsigned char)ubrr;
  // set terminal to input and output
  UCSR0B = (1 << TXEN0) | (1 << RXEN0);
  // set output mode 8-bit data
  //UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
  
  UCSR0C =
    // async operation
    (0 << UMSEL01) |
    (0 << UMSEL00) |
    // Parity disabled
    (0 << UPM01) |
    (0 << UPM00) |
    // stop Bits = 1bit 
    (0 << USBS0) |
    // 8 Data bits
    (1 << UCSZ01) |
    (1 << UCSZ00) |
    // for Synch Mode only clock polarity
    (0 << UCPOL0);
}

void uart_send_string(char *data)
{
  // send until the string is done
  while((*data != '\0'))
    {
      while (!(UCSR0A & (1 << UDRE0)));
      UDR0 = *data;
      data++;
    }
	
  // required delay between UART commands
  _delay_ms(1800);
}

// configure ADC input
void adc_init (void)
{
  // set PORTC to input for ADC
  DDRC = 0x00;
	
  ADMUX =
    // Reference Selection Bits
    // AVcc with external capacitor at AREF
    (0 << REFS1) |
    (1 << REFS0) |
    // ADC Left Adjust Result
    (1 << ADLAR) |
    // Analog Channel Selection Bits
    // ADC5 (PC5 PIN28)
    (0 << MUX3) |
    (1 << MUX2) |
    (0 << MUX1) |
    (1 << MUX0);
  ADCSRA =
    // ADC ENable
    (1 << ADEN) |
    // ADC Start Conversion
    (0 << ADSC) |
    // ADC Auto Trigger Enable
    (0 << ADATE) |
    // ADC Interrupt Flag
    (0 << ADIF) |
    // ADC Interrupt Enable
    (1 << ADIE) |
    // ADC Prescaler Select Bits
    // prescalar = 128
    (1 << ADPS2) |
    (1 << ADPS1) |
    (1 << ADPS0);
}

void xmit_temp(float temp)
{
  // output strings
  char send[20];
  char report[80];
  int report_len = 0;
	
  // generate output string
  snprintf(report, sizeof(report), "GET /update?api_key=TNTAKRCSHOJW0ANL&field1=%.2f\r\n\r\n", temp);
	
  // get number of bytes to send
  while (report[report_len] != '\0')
    {
      report_len++;
    }
	
  // generate SEND AT string based on byte size of report
  snprintf(send, sizeof(send), "AT+CIPSEND=%d\r\n", report_len);

  // output to terminal
  ATOMIC_BLOCK(ATOMIC_FORCEON)
    {
      uart_send_string("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n");
      uart_send_string(send);
      uart_send_string(report);
    }
}

// configure ESP01 and connect to WiFi
void esp01_init(void)
{
  uart_send_string("AT\r\n");
  uart_send_string("AT+CWMODE=1\r\n");
  uart_send_string("AT+CIPSTA=\"10.7.7.22\"\r\n");
  uart_send_string("AT+CWJAP=\"xxxxxxxx\",\"xxxxxxxx\"\r\n");
  uart_send_string("AT+CIPMUX=0\r\n");
}
