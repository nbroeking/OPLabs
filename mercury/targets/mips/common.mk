CFLAGS=-Wall -I include
CXXFLAGS=-Wall -I include
PREFIX=mips-buildroot-linux-uclibc-
CC=$(PREFIX)gcc
CXX=$(PREFIX)g++
AR=$(PREFIX)ar
LDFLAGS=
QEMU=qemu-mips
