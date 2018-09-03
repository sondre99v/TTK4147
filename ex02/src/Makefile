# selected compiler (gcc/clang/etc.)
COMPILER = clang
# program executable name 
TARGET = main 
# compiler flags 
CFLAGS = -g -Wall -fsanitize=thread
# linker flags 
LDFLAGS = -g -lpthread -fsanitize=thread
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
	rm -f $(TARGET) $(OBJECTS) 

rebuild:
	make clean
	make all