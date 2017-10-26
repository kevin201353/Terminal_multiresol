#include <stdio.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "global.h"
#include <sys/time.h>
#include <assert.h>
#include <termios.h> 

static struct termios oldt;

//restore terminal settings
void restore_terminal_settings(void)
{
    //Apply saved settings
    tcsetattr(0, TCSANOW, &oldt); 
}

//make terminal read 1 char at a time
void disable_terminal_return(void)
{
    struct termios newt;
    
    //save terminal settings
    tcgetattr(0, &oldt); 
    //init new settings
    newt = oldt;  
    //change settings
    newt.c_lflag &= ~(ICANON | ECHO);
    //apply settings
    tcsetattr(0, TCSANOW, &newt);
    
    //make sure settings will be restored when program ends
    atexit(restore_terminal_settings);
}

gboolean show_vmlist_dialog(gpointer* message)
{
	SY_vmlistwindow_main();
	return FALSE;
}

void linux_key_press()
{
	int fd_kb;
    struct input_event event_kb;
    char szdev[100] = {0};
#ifdef ARM
	sprintf(szdev, "/dev/input/%s", "event0");
#else
	sprintf(szdev, "/dev/input/%s", "event3");
#endif
    fd_kb = open(szdev, O_RDONLY);
    if(fd_kb <= 0)
    {
		LogInfo("open device error\n");
		return 0;
    }
	while(1)
	{
		if(read(fd_kb, &event_kb, sizeof(event_kb)) == sizeof(event_kb))
		{
			if (event_kb.type == EV_KEY)
			{
				if (event_kb.value == 1)
				{
					if(event_kb.code == KEY_ESC)
					    break;
					if(event_kb.code == KEY_F2)
					{
						LogInfo("list_key_press xxxxx  F2\n");
#ifdef MEETING
						char szMsg[BUF_MSG_LEN]= {0};
						sprintf(szMsg, "\napagentui.ThinviewChange####{}\n");
						write(1, szMsg, strlen(szMsg));
#endif
					}
					if(event_kb.code == KEY_W)
						LogInfo("list_key_press xxxxx  W\n");
				}
			}
		}
		usleep(3000);
	}
	close(fd_kb);
}


#define KEYUI   (KEY_U & KEY_I)

#define test_bit(bit) (mask[(bit)/8] & (1 << ((bit)%8)))
void linux_key_press2()
{
	struct input_event event_kb;
	char		name[64];
	char		buf[64];
	int		fd = 0;
	int		flag;
	int		i = 0;
	unsigned char mask[EV_MAX/8 + 1];
    int		version;
	short    nshift = 0;
	short    key_u = 0;
	short    key_i = 0;
	while(1)
	{
		for (i = 0; i < 32; i++) 
		{
	        sprintf(name, "/dev/input/event%d", i);
	        if ((fd = open(name, O_RDONLY, 0)) >= 0) {
	            ioctl(fd, EVIOCGVERSION, &version);
	            ioctl(fd, EVIOCGNAME(sizeof(buf)), buf);
	            ioctl(fd, EVIOCGBIT(0, sizeof(mask)), mask);
	            //LogInfo("linux_key_press2, %s\n", buf);
	            if (strcmp(buf, "Generic USB Keyboard") == 0)
	            {
	            	LogInfo("linux_key_press2, find USB Keyboard.\n");
					flag = 1;
					break;
			   }
	            close(fd);
	        }
	    }
		if (flag == 1)
		{
		    if (fd >= 0)
				close(fd);
		    fd = open(name, O_RDONLY);
			while(read(fd, &event_kb, sizeof(event_kb)) == sizeof(event_kb))
			{
				if (event_kb.type == EV_KEY)
				{
					if (event_kb.value == 1)
					{
						if(event_kb.code == KEY_ESC)
						    break;
						if(event_kb.code == KEY_F2)
						{
							LogInfo("list_key_press xxxxx  F2\n");
#ifdef MEETING
							char szMsg[BUF_MSG_LEN]= {0};
							sprintf(szMsg, "\napagentui.ThinviewChange####{}\n");
							write(1, szMsg, strlen(szMsg));
#endif
						}
						if(event_kb.code == KEY_W)
							LogInfo("list_key_press xxxxx  W\n");
#if 0
#ifdef WUHUDX
						if (event_kb.code == KEY_LEFTSHIFT)
							nshift = 1;
						if (event_kb.code == KEY_U)
							key_u = 1;
						if (event_kb.code == KEY_I)
							key_i = 1;
						if (key_u == 1 && key_i == 1 && nshift == 1)
						{
							nshift = 0;
							key_u = 0;
							key_i = 0;
							LogInfo("list_key_press xxxxx  shift + u + i \n");
							g_idle_add(show_vmlist_dialog, NULL);
						}
#endif
#endif
					}
				}
			}
			close(fd);
			flag = 0;
		}
	}
}


void read_input()
{
   //do where
}
//int main()
//{
//	return 0;
//}
