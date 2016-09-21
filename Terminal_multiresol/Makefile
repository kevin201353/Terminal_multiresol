
CFLAGS=`pkg-config --cflags gtk+-3.0 mxml libcurl`
LIBS=`pkg-config --libs gtk+-3.0 mxml libcurl`

TIME=`date +"%Y-%m-%d %T"`
.INIT:
	@echo $(TIME)
	@echo "char buildtime[32]=\"$(TIME)\";" > buildtime.h

ALLFILE=thrdMsgWindow.c checkaddress.c SYMsgDialog.c resol.c loginwindow.c vmlistwindow.c\
          resolts_main.c networkSet.c setting.c curlc.c \
					Ovirtdispatch.c  Loginfo.c vms.c configxml.c logincit.c aboutwindow.c testnet.c
ALLOBJECT=shencloud-ui thrdMsgWindow.o checkaddress.o SYMsgDialog.o resol.o \
									resolts_main.o networkSet.o setting.o loginwindow.o \
									curlc.o Ovirtdispatch.o \
									Loginfo.o vms.o configxml.o vmlistwindow.o  logincit.o aboutwindow.o testnet.o
all: .INIT clean shencloudui

shencloudui:
	gcc -std=gnu99 -c  $(ALLFILE) $(CFLAGS)
	gcc -std=gnu99 -o  $(ALLOBJECT) -Wall $(LIBS)  -linterface -lpcre

clean:
	rm -rf *.o shencloud-ui
