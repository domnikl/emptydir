#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

// is_directory return values
#define ERROR_COULD_NOT_STAT_FILE 1
#define IS_DIR 2
#define IS_FILE 0

void print_empty_directories_in(char *path);
int is_directory(char *path);

void usage()
{
	printf("usage: emptydir <path1> <path2> ...\n");
	exit(1);
}

int main(int argc, char *argv[])
{
	if (argc == 2 && (strcmp("-h", argv[1]) == 0 || strcmp("--help", argv[1]) == 0)) {
		usage();
	}

	unsigned int i;

	for (i = 1; i < argc; i++) {
		print_empty_directories_in(argv[i]);
	}

	return 0;
}

int is_directory(char *path)
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

	if ((dir = opendir(path)) == NULL) {
		fprintf(stderr, "Could not open directory '%s'\n", path);
		return;
	}

	struct dirent *ent;
	unsigned int has_files = 0;
	while ((ent = readdir(dir)) != NULL) {
		if (strcmp(".gitignore", ent->d_name) == 0 || strcmp(".hgignore", ent->d_name) == 0) {
			continue;
		} else if (is_directory(ent->d_name) == IS_FILE) {
			has_files = 1;
		} else if (strcmp(".", ent->d_name) != 0 && strcmp("..", ent->d_name) != 0) {
			has_files = 1;
			// do the recursive thing
			char *new_path = (char*) malloc(sizeof(char) * (strlen(path) + 1 + strlen(ent->d_name) + 1));

			strcpy(new_path, path);
			strcat(new_path, "/");
			strcat(new_path, ent->d_name);

			if (is_directory(new_path) == IS_DIR) {
				print_empty_directories_in(new_path);
			}

			free(new_path);
			new_path = NULL;
		}
	}

	if (has_files == 0) {
		printf("%s\n", path);
	}

	closedir(dir);
}
