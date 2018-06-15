#ifndef __CONFIG_H__
#define __CONFIG_H__

#define DEFAULT_PORT 1024
#define DEFAULT_SERVER_IP "127.0.0.1"
#define DEFAULT_CONFIG_FILENAME "/etc/reverse_echo.conf"
#define BUF_SIZE 1024


typedef struct
{
	int port;
	char server_ip[64];
} config_t ;

int get_config(char *filename, config_t *config);

#endif
