CC := gcc

ifeq ($(INSTALL_DIR),)
INSTALL_DIR := "/"
endif

TARGET:= reverse_echo_daemon
CLIENT_TARGET:=reverse_echo_client

LDFLAGS:= -pthread

.PHONY: all install
all: $(TARGET) $(CLIENT_TARGET)

$(TARGET):
	$(CC) $(CFLAGS) $(LDFLAGS) main.c config.c -o $(TARGET)

$(CLIENT_TARGET):
	$(CC) $(CFLAGS) client.c -o $(CLIENT_TARGET)

install: $(TARGET) $(CLIENT_TARGET)
	mkdir -p $(INSTALL_DIR)/etc
	cp *.conf $(INSTALL_DIR)/etc/
	mkdir -p $(INSTALL_DIR)/usr/bin
	cp $(TARGET) $(INSTALL_DIR)/usr/bin/
	cp $(CLIENT_TARGET) $(INSTALL_DIR)/usr/bin/

clean:
	@rm -rf $(TARGET) $(CLIENT_TARGET)
