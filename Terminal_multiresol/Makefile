export PKG_CONFIG_PATH=/usr/lib/lib/pkgconfig
CFLAGS=`pkg-config --cflags gtk+-3.0 mxml libcurl`
LIBS=`pkg-config --libs gtk+-3.0 mxml libcurl`

TIME=`date +"%Y-%m-%d %T"`
.INIT:
	@echo $(TIME)
	@echo "char buildtime[32]=\"$(TIME)\";" > buildtime.h

ALLFILE=thrdMsgWindow.c checkaddress.c SYMsgDialog.c resol.c tcpclient.c resolts_main.c loginwindow.c vmlistwindow.c \
           networkSet.c setting.c curlc.c \
					Ovirtdispatch.c  Loginfo.c vms.c configxml.c logincit.c aboutwindow.c testnet.c \
					SYMsgDialog2.c  SYMsgDialogVm.c  netInfoWin.c  scale.c config2.c stu_teach.c MsgServer.c stu_server.c SYPasDialog.c md5.c msg_queue.c \
					wait3.c modifypass.c SYPasModifyDialog.c cjson.c keypress.c
ALLOBJECT=shencloud-ui thrdMsgWindow.o checkaddress.o SYMsgDialog.o resol.o \
									resolts_main.o networkSet.o setting.o loginwindow.o \
									curlc.o Ovirtdispatch.o \
									Loginfo.o vms.o configxml.o  tcpclient.o vmlistwindow.o  logincit.o aboutwindow.o \
									testnet.o SYMsgDialog2.o  SYMsgDialogVm.o  netInfoWin.o  scale.o config2.o  stu_teach.o  MsgServer.o stu_server.o SYPasDialog.o md5.o msg_queue.o wait3.o \
									modifypass.o SYPasModifyDialog.o cjson.o keypress.o

all: .INIT clean shencloudui

shencloudui:
	gcc -std=gnu99 -c  $(ALLFILE) $(CFLAGS)
	gcc -std=gnu99 -o  $(ALLOBJECT) $(ALLOBJECT1) -Wall $(LIBS)   -lpcre -lpthread -lcrypto -lm

clean:
	rm -rf *.o shencloud-ui
