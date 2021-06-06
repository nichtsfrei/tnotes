#include "teno_rules.h"
#include "teno_rules_filter.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define MAX_PATH_SIZE 4096

int read(void *f) { return fgetc((FILE *)f); }

struct TenoRule *read_rule(char *path, struct tm *t)
{
	FILE *f = fopen(path, "r");
	struct TenoRule *tr = NULL;
	if (f == NULL)
		goto exit;
	tr = teno_rules_parse_content(f, read, t, teno_rules_cmp);
exit:
	if (f != NULL)
		fclose(f);
	return tr;
}

int rread_dir(char *path, struct tm *t)
{
	char np[MAX_PATH_SIZE];
	struct dirent *de;
	struct stat path_stat;
	struct TenoRule *r;
	DIR *dr = opendir(path);
	int np_len, result = 0;
	if (dr == NULL) 
	{
		result = -1;
		goto exit;
	}
	while ((de = readdir(dr)) != NULL) {
		snprintf((char *)&np, MAX_PATH_SIZE, "%s/%s", path, de->d_name);
		// ignoring parent dirs
		if ((np_len = strlen((char *)&np)) > 1 &&
		    np[np_len - 1] != '.') {
			stat(np, &path_stat);
			if (S_ISDIR(path_stat.st_mode)) {
				if ((result = rread_dir((char *)&np, t)))
					fprintf(stderr, "Failure on %s\n", np);
			} else if (S_ISREG(path_stat.st_mode) &&
				   np[np_len - 3] == '.' &&
				   np[np_len - 2] == 'm' &&
				   np[np_len - 1] == 'd' &&

				   (r = read_rule(np, t)) != NULL) {
				printf("file://%s\n", np);
				free(r);
				r = NULL;
			}
		}
	}
exit:
	if (dr != NULL)
		closedir(dr);
	if (r != NULL)
		teno_rules_free(r);
	return result;
}

int main(int argc, char *argv[])
{
	time_t rawtime;
	time(&rawtime);
	struct tm *t = localtime(&rawtime);
	if (argc != 2) {
		fprintf(stderr, "Need path as parameter\n");
		return 1;
	}
	return rread_dir(argv[1], t);
}
