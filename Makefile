OBJS := hidlib/hid.o bellwin_hid.o
CFLAGS := -Wall -Ihidlib
LDFLAGS := -ludev

all: $(OBJS)
		$(CC) -o bellwin_hid $(OBJS) $(LDFLAGS)

clean:
		rm -rf *.o */*.o bellwin_hid

install:
	cp bellwin_hid /usr/sbin
	cp udev/99-hid.rules /etc/rules.d/
