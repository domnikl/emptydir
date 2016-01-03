#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

#include "getopt.h"

// is_directory return values
#define ERROR_COULD_NOT_STAT_FILE 1
#define IS_DIR 2
#define IS_FILE 0

// options to be set from CLI
#define OPTIONS_SHOW_HIDDEN 1
#define OPTIONS_ESCAPE_PATHS 2

void print_empty_directories_in(char *path, unsigned int options);
int is_directory(char *path);
int has_option(int options, int option);
void print_path(char *path, int options);
void escape_path(char *out, char *path);

void usage()
{
	printf("usage: emptydir [options] <path1> <path2> ...\n");
	printf("\t-a          \tsearch in hidden directories\n");
    printf("\t-b, --escape\tescape output paths to be used in conjunction with xargs\n");
	exit(1);
}

int main(int argc, char *argv[])
{
    unsigned int options = 0;
    const char *ch;

    while ((ch = GETOPT(argc, argv)) != NULL) {
        GETOPT_SWITCH(ch) {
        GETOPT_OPT("-a"):
            options = options | OPTIONS_SHOW_HIDDEN;
            break;
        GETOPT_OPT("-b"):
        GETOPT_OPT("--escape"):
            options = options | OPTIONS_ESCAPE_PATHS;
            break;
        GETOPT_MISSING_ARG:
            printf("missing argument to %s\n", ch);
            /* FALLTHROUGH */
        GETOPT_DEFAULT:
            usage();
        }
    }

    argc -= optind;
    argv += optind;

    if (argc == 0) {
        usage();
    }

	unsigned int i;

	for (i = 0; i < argc; i++) {
		print_empty_directories_in(argv[i], options);
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

void print_empty_directories_in(char *path, unsigned int options)
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
        } else if (strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0) {
            continue;
		} else if (strncmp(".", ent->d_name, 1) != 0 || has_option(options, OPTIONS_SHOW_HIDDEN)) {
			has_files = 1;
			// do the recursive thing
			char *new_path = (char*) malloc(sizeof(char) * (strlen(path) + 1 + strlen(ent->d_name) + 1));

			strcpy(new_path, path);
			strcat(new_path, "/");
			strcat(new_path, ent->d_name);

			if (is_directory(new_path) == IS_DIR) {
				print_empty_directories_in(new_path, options);
			}

			free(new_path);
			new_path = NULL;
		}
	}

	if (has_files == 0) {
        print_path(path, options);
	}

	closedir(dir);
}

void escape_path(char *out, char *path) 
{
    unsigned int i;
    size_t l;

    l = strlen(path);

    for (i = 0; i <= l; i++, path++, out++) {
        if (*path == '\0') {
            break;
        } else if (*path == ' ') {
            memcpy(out, "\\ ", sizeof(char) * 2);
            out++;
        } else {
            memcpy(out, path, sizeof(char));
        }
    }
}

void print_path(char *path, int options)
{
    if (has_option(options, OPTIONS_ESCAPE_PATHS)) {
        char *escaped = malloc(sizeof(char*) * strlen(path) * 2);
        escape_path(escaped, path);
        printf("%s\n", escaped);
        free(escaped);
    } else {
        printf("%s\n", path);    
    }
}

int has_option(int options, int option) 
{
    return (options & option) == option;
}
