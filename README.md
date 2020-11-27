# bellwin_usb_linux_driver
Linux userspace driver for Bellwin USB Power splitter

Linux Driver for Bellwin USB power splitter which officially has only Windows driver/API.

Userpsace Linux driver based on Linux hidraw kernel driver.

Installation:

Copy udev/99-hid.rules to /etc/rules.d
run: udevadm control --reload-rules
Disconnect and reconnect the USB device.

