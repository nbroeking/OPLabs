CFLAGS=-Wall -I include
CXXFLAGS=-Wall -I include
PREFIX=mipsel-buildroot-linux-uclibc-
CC=$(PREFIX)gcc
CXX=$(PREFIX)g++
AR=$(PREFIX)ar
QEMU=qemu-mipsel
