# General

INSTALL_PLUGINS_DIR = /usr/local/lib/ladspa/
INCLUDES = -I.
LIBRARIES = -lm
# note: clang should use -Wno-missing-braces
CFLAGS = $(INCLUDES) -std=c++11 -Wall -Werror -O3 -fPIC
CC = g++
# TODO: cpp, cc?
FILENAME = test_effect
	
vocoder.so: $(FILENAME).cpp
	$(CC) $(CFLAGS) -o $(FILENAME).o -c $(FILENAME).cpp
	$(CC) -o $(FILENAME).so $(FILENAME).o -shared

# Targets

install: targets
	-mkdirhier	$(INSTALL_PLUGINS_DIR)
	cp *.so $(INSTALL_PLUGINS_DIR)

targets: vocoder.so

clean:
	-rm -f *.o *.so *~ core

