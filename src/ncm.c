#include <stdio.h>
#include <pthread.h>
#include "dump.h"
#include "thpool.h"
	
extern char *optarg; 

typedef void (*wrap_func)(void *);

int main(size_t argc, char *argv[])
{
	/* 解析命令行 */
	// opterr = 0;
	int option, is_dir = 0, thread_nums = sysconf(_SC_NPROCESSORS_ONLN);
	char *work_dir = NULL;
	while ((option = getopt(argc, argv, "o:hd:j:")) != -1) {
		switch(option) {
		case 'h':
			printf("Usage %s [OPTIONS] FILES \n"
				"Convert CloudMusic ncm files to mp3/flac files\n\n"
				"Examples:\n"
				"\tncm2dump test.ncm\n"
				"\tncm2dump test1.ncm test2.ncm test3.ncm\n"
				"\tncm2dump -d ./download/\n"
				"\tncm2dump -j 6 -d ./download/\n\n"
				"Options:\n"
				"\t-h                   display HELP and EXIT\n"
				"\t-j [N]               start N threads to convert\n"
				"\t-d <Directory>       batch convert ncm in a specified <directory>\n"
				"\t-o <file>            place out file in <file>\n"
				"", argv[0]);
			goto END;
			break;

		case 'o':
			// printf("arg is %s\n", optarg);
			break;
		case 'd':
			is_dir = 1;
			work_dir = optarg;
			// printf("arg is %s\n", optarg);
			break;
		case 'j':
			thread_nums = atoi(optarg);
			// printf("arg is %d\n", thread_nums);
			break;
		default: // 选项和参数不规范直接exit
			goto END;
		}
	}
	
	// 没有输入文件
	if (optind == argc && !is_dir) {
		printf("%s: fatal error: no input files. USE %s -h\n", 
				argv[0], argv[0]);
		goto END; 
	}

	char **file_queue = &argv[optind];
	size_t file_nums = argc - optind;

	// for (size_t index = optind; index < argc; index++) {
	// 	printf("%s\n", argv[index]);
	// }

	// 创建线程池
	threadpool pool = thpool_init(thread_nums);

	char **file_name = NULL;
	int count = 0;
	if (!is_dir) {
		for (size_t i = 0; i < file_nums; i++) {
			// work_convert(file_queue[i]);
			// 过滤不合法文件名
			if (0 == strcmp(file_queue[i] + strlen(file_queue[i]) -  4,
					       	".ncm")) {
				thpool_add_work(pool, (wrap_func)work_convert,
						file_queue[i]);
				printf("[SUCCESS] %s\n", file_queue[i]);
			}
			else
				printf("[INVALID FILE] %s\n", file_queue[i]);
		}
	} else {

		// 0. 切换到子目录
		if (chdir(work_dir) == -1) {
			printf("chdir ERROR\n");
			goto END;
		}

		// 1. 遍历目录, 得到目录下面的所有文件
		DIR *dir = opendir("./");
		if (!dir) {
			printf("opendir ERROR\n");
			goto END;
		}

		struct dirent *entry;
		while ((entry = readdir(dir)) != NULL) {
			if (strcmp(entry->d_name + strlen(entry->d_name) - 4,
					       	".ncm")) 
				continue;
			count++;
		}
		
		file_name = malloc(sizeof(char *) * count);
		size_t index = 0;

		rewinddir(dir);
		while ((entry = readdir(dir)) != NULL) {
			if (strcmp(entry->d_name + strlen(entry->d_name) - 4,
						".ncm")) 
				continue;
			file_name[index] = strdup(entry->d_name);
			thpool_add_work(pool, (wrap_func)work_convert,
					file_name[index]);
			index++;
		}

	}

	thpool_wait(pool);
	thpool_destroy(pool);
	if (is_dir)
		for (size_t i = 0; i < count; i++)
			free(file_name[i]);
END:
	return 0;
}
