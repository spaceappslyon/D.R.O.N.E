/*
** test.c for test in /home/cnu432bvwy/exo/test
** 
** Made by Foussat Flavien and V1C0D3R
** Login   <foussa_f@epitech.net>, <V1C0D3R@gmail.com>
** 
** Started on  Sun Apr 12 09:12:01 2015 Foussat Flavien
** Last update Sun Apr 12 11:57:18 2015 V1C0D3R
*/

#include <wiringPi.h>
#include <softPwm.h>
#include "piFace.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define PIFACE 200
#define INIT_PIN 6

void	check_arguments(char *p, char *a, char *b)
{
  if (p[0] < '0' || p[0] > '9')
    {
      printf("error, it's not a number\n");
      printf("usage : arg[1] = serv, arg[2] = angle\nexit\n");
      free(a);
      free(b);
      free(p);
      exit(1);
    }
}

void	check_number_arguments(int i, char *p, char *a, char *b)
{
  if (p[i] == '\0')
    {
      printf("error, too few arguments\n");
      printf("usage : arg[1] = serv, arg[2] = angle\nexit\n");
      free(a);
      free(b);
      free(p);
      exit(1);
    }
}

int	main()
{
  char  *p;
  char  *a;
  char	*b;
  int	serv;
  int	ang;
  int	o;
  int	i;

  int lastAng = 0;

  a = calloc(99, sizeof(p));
  b = calloc(99, sizeof(p));
  p = calloc(99, sizeof(p));
  i = 0;
  o = 0;

  piFaceSetup(PIFACE);
 
  wiringPiSetup();
  softPwmCreate (PIFACE+INIT_PIN, 0, 1023) ;

  printf("Hello world !\nUsage : arg[1] = serv, arg[2] = angle\n");
  for(;;)
    {
      read(0, p, 99);
      check_arguments(p, a, b);
      while(p[i] != ' ')
	{
	  check_number_arguments(i, p, a, b);
	  a[i] = p[i++];
	}
      i++;
      while(p[i] != '\0')
	b[o++] = p[i++];
      serv = atoi(a);
      ang = atoi(b);

      ang = (ang > 1023)?1023:ang;
      ang = (ang < 0)?0:ang;
      
      if(lastAng < ang)
	{
	      for (i = lastAng ; i < ang ; i=i+1)
	      {
	      	softPwmWrite (PIFACE+INIT_PIN, i) ;
	      	delay (20) ;
	      }
	      lastAng = ang;
	}
	else if(lastAng > ang)
	{
	      for (i = lastAng ; i > ang ; i=i-1)
	      {
	      	softPwmWrite (PIFACE+INIT_PIN, i) ;
	      	delay (20) ;
	      }
		delay(500);
	      lastAng = ang;
	}

      printf("serv = %d, ang = %d\n", serv, ang);

      p = calloc(99, sizeof(p));
      a = calloc(99, sizeof(p));
      b = calloc(99, sizeof(p));
      i = 0;
      o = 0;
    }
  return (0);
}
