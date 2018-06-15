#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

static int read_int_from_config_line(char* config_line) {
	char prm_name[64];
	int val;
	sscanf(config_line, "%s %d\n", prm_name, &val);

	return val;
}
static void read_str_from_config_line(char* config_line, char* val) {
	char prm_name[64];
	sscanf(config_line, "%s %s\n", prm_name, val);
}


int get_config(char *filename, config_t *config) {

	char buf[BUF_SIZE];
	FILE *fp = NULL;

	if ((fp=fopen(filename, "r")) == NULL) {
		fprintf(stderr, "Failed to open config file %s", filename);
		return 1;
	}

	while(! feof(fp)) {
		fgets(buf, BUF_SIZE, fp);

		/* skip comment and empty lines */
		if (buf[0] == '#' || strlen(buf) ==0 ) {
			continue;
		}
		if (strstr(buf, "PORT ")) {
			config->port = read_int_from_config_line(buf);
		}
		if (strstr(buf, "SERVER_IP ")) {
			read_str_from_config_line(buf, config->server_ip);
		}
	}

	fclose(fp);

	return 0;
}