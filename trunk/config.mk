CC = gcc
CFLAGS = -g -O0

KERNEL_HEADERS = /usr/src/linux-2.6.0/include

# do not change stuff below.

PACKAGE_NAME = jammod
PACKAGE_VERSION = 1.0pre1

CFLAGS += -DPACKAGE_NAME=\"$(PACKAGE_NAME)\" -DPACKAGE_VERSION=\"$(PACKAGE_VERSION)\"

