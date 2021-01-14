OBJS := hidlib/hid.o bellwin_hid.o
CFLAGS := -Wall -Ihidlib
LDFLAGS := -ludev

all: $(OBJS)
		$(CC) -o bellwin $(OBJS) $(LDFLAGS)

clean:
		rm -rf *.o */*.o bellwin_hid

install:
	cp bellwin /usr/sbin
	cp udev/99-bellwin-hid.rules /etc/udev/rules.d/
