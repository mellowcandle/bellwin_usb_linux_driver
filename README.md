# bellwin_usb_linux_driver
Linux userspace driver for Bellwin USB Power Splitter.

This is the reversed engineered driver for Linux for Bellwin 5 port USB Power splitter (UP516EU).
This userspace driver is neither officialy supported nor tester throughly. Please use at your own risk.
I take no responsibility for any damage you might encounter.

Userpsace Linux driver based on Linux hidraw kernel driver.

## Installation

Build by running `make && sudo make install`

## After installation:
run: `udevadm control --reload-rules`
Disconnect and reconnect the USB device.

