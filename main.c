#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>

// colors
#define GRN "\e[0;32m"
#define BLU "\e[0;34m"
#define RST "\e[0m"

#define SIZE 256

// copy the hostname in dest
void hostnamecpy(char *dest) {
	if(gethostname(dest, SIZE) == -1) perror("gethostname");
}

// copy the login in dest
void logincpy(char *dest) {
	strncpy(dest, getenv("USER"), SIZE);
}

int getos(char *dest) {
	FILE *fp = fopen("/etc/os-release", "r");
	char buff[SIZE];

	if(fp == NULL) return 0;

	// search for NAME=
	while(fgets(buff, sizeof(buff), fp) != NULL) {
		if(strncmp(buff, "NAME=", 5) == 0) {
			char *os = NULL;
			if(buff[5] == '"' || buff[5] == '\'') {
				os = buff + 6;
				char *end = strrchr(os, '"');
				if(!end)
					end = strrchr(os, '\'');
				*end = '\0';
			}
			else os = buff + 5;

			buff[strcspn(buff, "\n")] = '\0';
			strncpy(dest, os, SIZE);
			break;
		}
	}
	return 1;
}

int main(void) {
	struct utsname info;
	if(uname(&info) == -1) {
		perror("uname");
		return -1;
	}

	struct sysinfo sinfo;
	if(sysinfo(&sinfo) == 1) {
		perror("sysinfo");
		return -1;
	}
	char os[SIZE];
	if(getos(os) == 0) { 
		fprintf(stderr, "error getting os\n");
		return -1;
	}

	float used_ram = (sinfo.totalram - sinfo.freeram - sinfo.bufferram) / (1024.0f * 1024.0f);
	float total_ram = sinfo.totalram / (1024.0f * 1024.0f);

	char hostname[SIZE] = {0};
	char login[SIZE] = {0};

	hostnamecpy(hostname);
	logincpy(login);
	printf(GRN "%s" RST "@%s\n", login, hostname);

	for(int i = 0; i < (int) strlen(login) + (int) strlen(hostname) + 1; i++)
		putchar('-');
	putchar('\n');

	printf(BLU "OS: " RST "%s %s\n", os, info.machine);
	printf(BLU "KERNEL: " RST "%s\n", info.release);
	printf(BLU "UPTIME: " RST "%ld H %ld M\n", sinfo.uptime / 3600, sinfo.uptime / 60);
	printf(BLU "RAM: " RST "%g MiB / %g MiB\n", used_ram , total_ram);

	return 0;
}
