#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

extern int errno;

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>


int is_hex(const char *str)
{
	while(*str != '\0') {
		if(isxdigit(*str) == 0) {
			return *str;
		}

		str++;
	}

	return 0;
}

int bulk_rename(const char dir[])
{
	DIR *pdir_entry;
	struct dirent *pdir_info;
	char filename_buf[256], oldname_buf[512], newname_buf[512];
	size_t filename_len, dirname_len;
	FILE *pfile;
	int cnt = 0;

	pdir_entry = opendir(dir);
	if(pdir_entry == NULL) {
		fprintf(stderr, "*** error (%d, opendir()): %s\n", errno, strerror(errno));

		return -1;
	}

	while((pdir_info = readdir(pdir_entry)) != NULL) {
		filename_len = strlen(pdir_info->d_name);
		if(pdir_info->d_type & 8 && filename_len > 13) {
			if(strcmp((pdir_info->d_name + filename_len - 4), ".map") == 0) {
				strcpy(filename_buf, pdir_info->d_name);
				*(filename_buf + filename_len - 4) = '\0';

				if(is_hex((filename_buf + filename_len - 12)) == 0) {
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
								fprintf(stderr, "*** error (%d, rename()): %s: \'%s\' -> \'%s\'\n", errno, strerror(errno), oldname_buf, newname_buf);
								cnt++;
							}
						}
						else {
							fprintf(stderr, "*** error (!NULL, fopen()): file exists: \'%s\'\n", newname_buf);
							fclose(pfile);
						}
					}
				}
			}
		}
	}

	closedir(pdir_entry);
	return cnt;
}

int main(int argc, char* argv[])
{
	char dir_buf[256];
	int cnt;

	if(argc > 1) {
		cnt = bulk_rename(argv[1]);
		if(cnt < 0) {
			exit(EXIT_FAILURE);
		}

		printf("\nrenamed %d files\n", cnt);
	}
	else {
		if(getcwd(dir_buf, 256) == NULL) {
			fprintf(stderr, "*** error (%d, getcwd()): %s\n", errno, strerror(errno));

			exit(EXIT_FAILURE);
		}

		cnt = bulk_rename(dir_buf);
		if(cnt < 0) {
			exit(EXIT_FAILURE);
		}

		printf("\nrenamed %d files\n", cnt);
	}

	exit(EXIT_SUCCESS);
}

