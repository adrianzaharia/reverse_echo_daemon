CC := gcc

TARGET:= reverse_echo_daemon
CLIENT_TARGET:=reverse_echo_client

all: $(TARGET) $(CLIENT_TARGET)

$(TARGET):
	$(CC) $(CFLAGS) main.c config.c -o $(TARGET)

$(CLIENT_TARGET):
	$(CC) $(CFLAGS) client.c -o $(CLIENT_TARGET)
clean:
	@rm -rf $(TARGET) $(CLIENT_TARGET)