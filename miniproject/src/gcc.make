INCLUDE = -I../lib/network -I../lib/time
# selected compiler (gcc/clang/etc.)
COMPILER = gcc
# program executable name 
TARGET = main 
# compiler flags 
CFLAGS = -g -Wall -std=c99#-std=gnu11
# linker flags 
LDFLAGS = -g -pthread
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
	$(COMPILER) $(CFLAGS) $(INCLUDE) -c -o $@ $< 
# cleaning 
clean: 
	rm -f $(TARGET) $(OBJECTS) 
# cleaning and building
rebuild:
	make clean
	make all
