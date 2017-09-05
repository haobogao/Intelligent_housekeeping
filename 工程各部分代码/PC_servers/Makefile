#common makefile header

DIR_INC = ./include
DIR_BIN = ./bin
DIR_LIB = ./libs


TARGET	=  server
BIN_TARGET = $(DIR_BIN)/$(TARGET)

CROSS_COMPILE = 
CFLAGS = -g -I$(DIR_INC) -I/usr/local/include/nopoll

ifdef LINUX64
LDFLAGS := -L$(DIR_LIB)/x64 -L/usr/local/lib 
else
LDFLAGS := -L$(DIR_LIB)/x86 -L/usr/local/lib 
endif
LDFLAGS += -lmsc -lrt -ldl -lpthread -lnopoll

OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))

$(BIN_TARGET) : $(OBJECTS)
	$(CROSS_COMPILE)gcc ./src/*.c $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o : %.c
	$(CROSS_COMPILE)gcc -c $(CFLAGS) $< -o $@
clean:
	@rm -f *.o $(BIN_TARGET)

.PHONY:clean

#common makefile foot
