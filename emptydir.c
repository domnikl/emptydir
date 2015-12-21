#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

// stat returns
#define ERROR_COULD_NOT_STAT_FILE 1
#define IS_DIR 2
#define IS_FILE 0

void print_empty_directories_in(char *path);
int is_dir(char *path);

int main(int argc, char *argv[])
{
	unsigned int i;

	for (i = 1; i < argc; i++) {
		print_empty_directories_in(argv[i]);
	}

	return 0;
}

int is_dir(char *path)
{
	int is_dir;
	struct stat s;

	if (stat(path, &s) == 0) {
	    if (s.st_mode & S_IFDIR) {
	        is_dir = IS_DIR;
	    } else if (s.st_mode & S_IFREG) {
	        is_dir = IS_FILE;
	    } else {
	        is_dir = IS_FILE;
	    }
	} else {
	    is_dir = ERROR_COULD_NOT_STAT_FILE;
	}

	return is_dir;
}

void print_empty_directories_in(char *path)
{
	DIR *dir;
	struct dirent *ent;

	dir = opendir(path);

	if (dir == NULL) {
		fprintf(stderr, "Could not open directory '%s'\n", path);
		return;
	}

	unsigned int has_files = 0;
	while ((ent = readdir(dir)) != NULL) {
		// TODO: ignore .gitignore
		if (strcmp(".gitignore", ent->d_name) == 0) {
			continue;
		}

		if (is_dir(ent->d_name) == IS_FILE) {
			has_files = 1;
		} else if (strcmp(".", ent->d_name) != 0 && strcmp("..", ent->d_name) != 0) {
			has_files = 1;
			// do the recursive thing
			char *new_path = (char*) malloc(sizeof(char) * (strlen(path) + 1 + strlen(ent->d_name) + 1));

			strcpy(new_path, path);
        	strcat(new_path, "/");
        	strcat(new_path, ent->d_name);

			if (is_dir(new_path) == IS_DIR) {
				print_empty_directories_in(new_path);
			}

			free(new_path);
		}
	}

	if (has_files == 0) {
		printf("%s\n", path);
	}

	closedir(dir);
}
