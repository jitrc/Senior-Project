/* main.c: the main entry point for my arduino control code.

   Target Board: Arduino Mega 2560
   Target Processor: Atmel ATMega2560

   Author: Austin Hendrix
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
//#include <util/delay.h>
#include <stdio.h>

#include "ros.h"

extern "C" {
#include "pwm.h"
#include "motor.h"
#include "serial.h"
#include "power.h"
#include "servo.h"
#include "comm.h"
#include "main.h"
#include "wheelmon.h"
#include "speedman.h"
#include "compass.h"
#include "bump.h"
}

#include "interrupt.h"
#include "protocol.h"
#include "gps.h"

#define CLK 16000

extern volatile int8_t steer;
extern volatile uint32_t ticks;

volatile uint16_t shutdown_count;

volatile int32_t idle_cnt = 0;

Packet<16> c_pack('C');
Packet<16> battery('b');

inline void writes16(int16_t s, uint8_t * buf) {
   buf[0] = s & 0xFF;
   buf[1] = (s >> 8) & 0xFF;
}

/*
void shutdown(void) {
   uint8_t i;
   volatile uint16_t sz = 0;
   struct heading h;
   while( shutdown_count == 0 ) {
      // Query command, for debugging
      // read speeds and output them
      cli();
      compass();
      sei();

      odom.reset();
      odom.append((uint16_t)rcount);
      odom.append((uint16_t)lcount);
      odom.append((uint16_t)qcount);
      odom.append((int16_t)rspeed);
      odom.append((int16_t)lspeed);
      odom.append((int16_t)qspeed);
      odom.append(steer);
      odom.finish();
      while( sz != 0 ) yeild();
      sz = odom.outsz();
      brain_tx_buffer((uint8_t *)odom.outbuf(), (uint16_t*)&sz);

      i=0;
      do {
         yeild();
         cli();
         h = compass_poll();
         sei();
         i++;
      } while( h.x == 0 && h.y == 0 && i < 10);

      c_pack.reset();
      c_pack.append((int16_t)(h.x - 13));
      c_pack.append((int16_t)(h.y - 48));
      c_pack.finish();
      while( sz != 0 ) yeild();
      sz = c_pack.outsz();
      brain_tx_buffer((uint8_t *)c_pack.outbuf(), (uint16_t*)&sz);

      battery.reset();
      battery.append(main_battery());
      battery.append(motor_battery());
      battery.append(idle_cnt);
      battery.finish();
      while( sz != 0 ) yeild();
      sz = battery.outsz();
      brain_tx_buffer((uint8_t *)battery.outbuf(), (uint16_t*)&sz);
   }
   while( shutdown_count > 0 ) {
      shutdown_count--;
      yeild();
      yeild();
   }
   // LED ON
   while(1) {
      PORTB |= (1 << 7);
      pwr_off();
      yeild();
   }
}
*/

void idle(void) {
   while(1) {
      idle_cnt++;
   }
}

int main() {
   DDRB |= 1 << 7;
   motor_init();

   servo_init();
   DDRC |= (1 << 1);
   servo_map(0, &PORTC, 1);

   servo_set(0, 127);
   battery_init();

   compass_init();

   bump_init();

   interrupt_init();

   // this combination of settings makes this the idle process
   sei(); // enable interrupts

   // serial port 3: bluetooth
   serial_init(BT);
   // set baud rate: 115.2k baud
   serial_baud(BT,115200);

   // serial port 0: brain
   serial_init(BRAIN);
   serial_baud(BRAIN, 115200);
   // do some serial init manually
   //  double-speed for more accurate baud rate
   UCSR0A |= (1 << U2X0);
   UBRR0 = 16;

   // GPS initialization
   gps_init(GPS);

   // initialize the shutdown process
   shutdown_count = 0;

   // power up!
   pwr_on();

   while(1) {
      // func, schedule, priority
      //system(shutdown, 50, 3); // odometry updates and the like; 20Hz

      //system(gps_thread, 3, 20); // gps thread. relatively low priority, 20Hz

      //system(brain_rx_thread, 3, 20); // start brain thread
      //system(bt_rx_thread, 3, 20);    // start bluetooth thread
   }
   
   // if we're here, we're done. power down.
   pwr_off();
   // loop forever, in case the arduino is on external power
   while(1);
}
