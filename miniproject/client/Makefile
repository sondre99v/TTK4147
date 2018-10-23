# selected compiler (gcc/clang/etc.)
COMPILER ?= avr32-linux-gcc
# program executable name 
TARGET = main 
# compiler flags 
CFLAGS = -g -Wall
# linker flags 
LDFLAGS = -g -lpthread
# list of sources 
SOURCES = $(shell find -name "*.c")
# default rule, to compile everything 
all: $(TARGET) 
# define object files 
OBJECTS = $(SOURCES:.c=.o) 
# link programs 
$(TARGET): $(OBJECTS) 
	$(COMPILER) $(LDFLAGS) -o $@ $^ 
# compile 
%.o : %.c 
	$(COMPILER) $(CFLAGS) -c -o $@ $< 
# cleaning 
clean: 
	rm -rf $(TARGET) $(OBJECTS) 
# cleaning and building
rebuild:
	make clean
	make all
local:
	make COMPILER=gcc LDFLAGS=-pthread CFLAGS="-Wall -DRUN_ON_LOCALHOST"
