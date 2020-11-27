OBJS := hidlib/hid.o bellwin_hid.o
CFLAGS := -g -Wall -Ihidlib
LDFLAGS := -ludev

all: $(OBJS)
		$(CC) -o bellwin_hid $(OBJS) $(LDFLAGS)

clean:
		rm -rf *.o */*.o bellwin_hid

