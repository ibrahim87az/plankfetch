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

int getos(char *str, char *dest) {
	FILE *fp = fopen("/etc/os-release", "r");
	char buff[SIZE];

	if(fp == NULL) return 0;

	// search for NAME=
	while(fgets(buff, sizeof(buff), fp) != NULL) {
		if(strncmp(buff, str, strlen(str)) == 0) {
			char *os = NULL;
			if(buff[strlen(str)] == '"' || buff[strlen(str)] == '\'') {
				os = buff + strlen(str) + 1;
				char *end = strrchr(os, '"');
				if(!end)
					end = strrchr(os, '\'');
				*end = '\0';
			}
			else os = buff + strlen(str);

			buff[strcspn(buff, "\n")] = '\0';
			strncpy(dest, os, SIZE);
			break;
		}
	}
	return 1;
}

int main(void) {
	char logo[6][SIZE] = {
		"        __  ",
		" _ __  / _| ",
		"| '_ \\| |_ ",
		"| |_) |  _| ",
		"| .__/|_|   ",
		"|_|         "
	};

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
	if(getos("NAME=", os) == 0) { 
		fprintf(stderr, "error getting os\n");
		return -1;
	}

	float used_ram = (sinfo.totalram - sinfo.freeram - sinfo.bufferram) / (1024.0f * 1024.0f);
	float total_ram = sinfo.totalram / (1024.0f * 1024.0f);

	char hostname[SIZE] = {0};
	char login[SIZE] = {0};

	hostnamecpy(hostname);
	logincpy(login);
	printf(GRN "\n\n%s" RST "@%s\n", login, hostname);

	for(int i = 0; i < (int) strlen(login) + (int) strlen(hostname) + 1; i++)
		putchar('-');
	putchar('\n');

	// PRINT INFO, NOW

	printf(GRN "%s\t" BLU "OS: " RST "%s %s\n", logo[0], os, info.machine);
	printf(GRN "%s\t" BLU "KERNEL: " RST "%s\n", logo[1], info.release);
	printf(GRN "%s\t" BLU "UPTIME: " RST "%ld H %ld M\n", logo[2], sinfo.uptime / 3600, sinfo.uptime / 60);
	printf(GRN "%s\t" BLU "RAM: " RST "%g MiB / %g MiB\n", logo[3], used_ram , total_ram);
	printf(GRN "%s\n", logo[4]);
	printf(GRN "%s\n\n\n" RST, logo[5]);
	//printf(GRN "%s\n", logo[6]);
	//printf(GRN "%s\n\n\n", logo[7]);




	return 0;
}
