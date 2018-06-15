CC := gcc

TARGET:= reverse_echo_daemon

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) main.c -o $(TARGET)

clean:
	@rm -rf $(TARGET)