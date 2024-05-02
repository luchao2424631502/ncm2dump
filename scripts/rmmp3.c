#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>

#define work_dir "/data/VipSongsDownload/"

int main()
{
	DIR *dir = opendir("/data/VipSongsDownload/");

	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		if (0 == strcmp(entry->d_name + strlen(entry->d_name) - 4, ".mp3")) {
			// char *dirpath = strdup(work_dir);
			// char *filepath = strcat(dirpath, entry->d_name);
			char *filepath = malloc(strlen(work_dir) + strlen(entry->d_name) + 1);
			memcpy(filepath, work_dir, strlen(work_dir));
			memcpy(filepath + strlen(work_dir), entry->d_name, strlen(entry->d_name));
			filepath[strlen(work_dir) + strlen(entry->d_name)] = 0;
			
			printf("%s\n", filepath);
			if (remove(filepath) == -1) {
				return -1;
			}
		}
	}
}
