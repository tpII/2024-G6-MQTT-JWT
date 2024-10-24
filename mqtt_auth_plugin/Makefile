# Compiler and flags
CC = gcc
CFLAGS = -fPIC -Wall \
         -Iinclude \
         -I/snap/mosquitto/current/usr/include

# Linking flags
LDFLAGS = -shared \
          -L/usr/lib/x86_64-linux-gnu \
          -L/snap/mosquitto/current/usr/lib \
          -ljwt \
          -lmosquitto

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Target and source files
TARGET = $(BIN_DIR)/mqtt_auth_plugin.so
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Build rules
all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean