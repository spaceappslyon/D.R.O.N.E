#include <wiringPi.h>
#include <softPwm.h>
#include "piFace.h"
 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
 
int main (void)
{
  int i, j ;
 
  printf ("Raspberry Pi PiFace + PWM test program\n") ;
  
  piFaceSetup(200);
 
  wiringPiSetup();
  softPwmCreate (206, 0, 100) ;
 
  for (;;)
  {
    for (i = 0 ; i <= 100 ; i=i+10)
    {
      softPwmWrite (206, i) ;
      delay (20) ;
    }
    delay(500);
    for (i = 100 ; i >= 0 ; i=i-10)
    {
      softPwmWrite (206, i) ;
      delay (20) ;
    }
    delay(500);
  }
 
  return 0 ;
}

//gcc -o pifPWM pifPWM.c piFace.c -lpthread -lwiringPi