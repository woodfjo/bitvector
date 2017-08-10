
#tools
CC=gcc

#dirs
SRC_DIR = .

INCLUDES = \
	-I.

SRC = \
	$(SRC_DIR)/bv.c \
	$(SRC_DIR)/bv_test.c

all: $(SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -o bv_test $(SRC)
