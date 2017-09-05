#ifndef __CHECK_MAC__
#define __CHECK_MAC__

#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define ONE_CONNECT 1
#define ONE_DISCONNECT 0
#define LED1  0
#define LED2  1
#define LED3  2
#define LED4  3
#define LED_ON 	0
#define LED_OFF 1

/*  Given a pathname for a file ,then the show_jpg function will take it  to  fill  the  framebuffer */
int show_jpg(const char *pathname);

/* Detect MAC address  /usr/Bind_addr and  /sys/module/rt3070ap/parameters/Mac_addr  have consistent */
/* consistent return 1   else  return 0 */
int check_mac(void);

/* Set the buzzer alarm */
int buzzer_warning(void);

/* request the mac event device : /dev/event */
/* return the new file descriptor, or -1  if  an  error */
int request_mac_event(char *name);

/* get the mac state from file descriptor ev_fd */
/* the Come in state ,will  return   ONE_CONNECT  */
/* the out state ,will return   ONE_DISCONNECT  */
int get_mac_info(int ev_fd);

/* close a mac file descriptor */
void release_mac_event(int ev_fd);


/*  Given a pathname for a file ,then the play_music function will to play  the  music */
int play_music(const char *pathname);

/*  Stop the music playing */
void  stop_music(void);


/* contrl led state */
/* The argument "no" must be  LED1 or  LED2  or LED3  or LED4 */
/* The argument "state" must be  LED_ON or  LED_OFF */
int set_led(int no,int state);

#endif

