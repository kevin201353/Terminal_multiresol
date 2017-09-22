#include <stdio.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "global.h"
#include <sys/time.h>
#include <assert.h>

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
					}
				}
			}
			close(fd);
			flag = 0;
		}
	}
	sleep(1);
}

//int main()
//{
//	return 0;
//}
