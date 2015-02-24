LIBRARIES = libb64.a

# Build flags (uncomment one)
#############################
# Release build flags
CFLAGS += -O3
#############################
# Debug build flags
#CFLAGS += -g
#############################

SOURCES = cdecode.c  cencode.c

TARGETS =$(OBS_DIR)/$(LIBRARIES)

TGT?=x86_64
OBS_DIR = _$(TGT)_obs

LINK.o = $(CC)

CFLAGS += -Werror -pedantic
CFLAGS += -I ../../include/3rdparty/

vpath %.h ../include/b64

.PHONY : clean

all: init | $(TARGETS)
init:
	mkdir -p $(OBS_DIR)

$(OBS_DIR)/libb64.a: $(OBS_DIR)/cencode.o $(OBS_DIR)/cdecode.o
	$(AR) -rcs $@ $^

strip:
	strip $(BINARIES) *.exe

clean:
	rm -rf $(OBS_DIR)

$(OBS_DIR)/cdecode.o: cdecode.c
	$(CC) -o $@ -c $^ $(CFLAGS)

$(OBS_DIR)/cencode.o: cencode.c
	$(CC) -o $@ -c $^ $(CFLAGS)
	

distclean: clean
	rm -f depend


