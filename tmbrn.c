#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <unistd.h>
#include <dirent.h>
#include <ctype.h>


const char CHARS_HEX[/*22 + 1*/]	= "0123456789ABCDEFabcdef";

int is_hex(const char str[])
{
	int i, j;
	for(i=0; str[i]!='\0'; i++) {
		if(!isxdigit(str[i])) {
			return 0;
		}
	}

	return 1;
}

int bulk_rename(const char dir[])
{
	DIR *pdir_entry;
	struct dirent *pdir_info;
	char filename_buf[256], oldname_buf[512], newname_buf[512];
	size_t filename_len, dirname_len;
	FILE *pfile;

	pdir_entry = opendir(dir);
	if(pdir_entry == NULL) {
		printf("*** error (%d, opendir()): %s\n", errno, strerror(errno));

		return 1;
	}

	while((pdir_info = readdir(pdir_entry)) != NULL) {
		filename_len = strlen(pdir_info->d_name);
		if(filename_len > 13 && strcmp((pdir_info->d_name + filename_len - 4), ".map") == 0) {
			strcpy(filename_buf, pdir_info->d_name);
			*(filename_buf + filename_len - 4) = '\0';

			if(is_hex((filename_buf + filename_len - 12))) {
				if(*(filename_buf + filename_len - 13) == '_') {
					*(filename_buf + filename_len - 13) = '\0';

					dirname_len = strlen(dir);

					/* old filename */
					strcpy(oldname_buf, dir);

					if(dir[dirname_len - 1] != '/') {
						strcat(oldname_buf, "/");
					}

					strcat(oldname_buf, pdir_info->d_name);

					/* new filename */
					strcpy(newname_buf, dir);

					if(dir[dirname_len - 1] != '/') {
						strcat(newname_buf, "/");
					}

					strcat(newname_buf, filename_buf);
					strcat(newname_buf, ".map");

					pfile = fopen(newname_buf, "rb");
					if(pfile == NULL) {
						if(rename(oldname_buf, newname_buf) == 0) {
							printf("tmbrn: %s -> %s\n", oldname_buf, newname_buf);
						}
						else {
							printf("*** error (%d, rename()): %s: \'%s\' -> \'%s\'\n", errno, strerror(errno), oldname_buf, newname_buf);
						}
					}
					else {
						printf("*** error (!NULL, fopen()): file exists: \'%s\'\n", newname_buf);
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

