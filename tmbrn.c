#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <unistd.h>
#include <dirent.h>


const char CHARS_HEX[/*22 + 1*/]	= "0123456789ABCDEFabcdef";

int is_hex(const char str[])
{
	int i, j;
	for(i=0; str[i]!='\0'; i++) {
		for(j=0; str[j]!='\0'; j++) {
			if(str[i] == CHARS_HEX[j]) {
				return str[i];
			}
		}
	}

	return 0;
}

int bulk_rename(const char dir[])
{
	DIR *pdir_entry;
	struct dirent *pdir_info;
	char filename_buf[256], fullname_buf[512], rn_buf[512];
	size_t filename_len, dirname_len;
	FILE *pfile;

	pdir_entry = opendir(dir);
	if(pdir_entry == NULL) {
		printf("*** error (%d, opendir()): %s\n", errno, strerror(errno));

		return 1;
	}

	while ((pdir_info = readdir(pdir_entry)) != NULL) {
		filename_len = strlen(pdir_info->d_name);
		if(strcmp((pdir_info->d_name + filename_len - 4), ".map") == 0) {
			strcpy(filename_buf, pdir_info->d_name);
			*(filename_buf + filename_len - 4) = '\0';

			if(is_hex((filename_buf + filename_len - 4 - 8)) != 0) {
				if(*(filename_buf + filename_len - 4 - 8 - 1) == '_') {
					*(filename_buf + filename_len - 4 - 8 - 1) = '\0';

					dirname_len = strlen(dir);

					/* original filename */
					strcpy(fullname_buf, dir);

					if(dir[dirname_len - 1] != '/') {
						strcat(fullname_buf, "/");
					}

					strcat(fullname_buf, pdir_info->d_name);

					/* new filename */
					strcpy(rn_buf, dir);

					if(dir[dirname_len - 1] != '/') {
						strcat(rn_buf, "/");
					}

					strcat(rn_buf, filename_buf);
					strcat(rn_buf, ".map");

					pfile = fopen(rn_buf, "r");
					if(pfile == NULL) {
						if(rename(fullname_buf, rn_buf) == 0) {
							printf("%s -> %s\n", fullname_buf, rn_buf);
						}
						else {
							printf("*** error (%d, rename()): %s: \'%s\' -> \'%s\'\n", errno, strerror(errno), fullname_buf, rn_buf);
						}
					}
					else {
						printf("*** error (!NULL, fopen()): file exists: \'%s\'\n", rn_buf);
						fclose(pfile);
					}
				}
			}
		}
	}

	closedir(pdir_entry);
	return 0;
}

int main(int argc, char* argv[])
{
	char dir_buf[256];
	if(argc > 1) {
		bulk_rename(argv[1]);
	}
	else {
		if(getcwd(dir_buf, 256) == NULL) {
			printf("*** error (%d, getcwd()): %s\n", errno, strerror(errno));

			return 1;
		}

		bulk_rename(dir_buf);
	}

	exit(EXIT_SUCCESS);
}

