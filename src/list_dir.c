#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "list_dir.h"
#include "file_info.h"

typedef struct {
    char name[256];
    struct stat st;
} Entry;

static int compare_size(const void *a, const void *b) {
    off_t s1 = ((Entry*)a)->st.st_size;
    off_t s2 = ((Entry*)b)->st.st_size;
    return (s2 > s1) - (s2 < s1);
}

static int compare_time(const void *a, const void *b) {
    time_t t1 = ((Entry*)a)->st.st_mtime;
    time_t t2 = ((Entry*)b)->st.st_mtime;
    return (t2 > t1) - (t2 < t1);
}

static int compare_name(const void *a, const void *b) {
    return strcmp(((Entry*)a)->name, ((Entry*)b)->name);
}

void list_directory(const char *path, Options opt) {
    if (!path || strlen(path) == 0) {
        fprintf(stderr, "ls: invalid path\n");
        return;
    }

    struct stat st;
    if (lstat(path, &st) < 0) {
        perror("ls");
        return;
    }

    if (!S_ISDIR(st.st_mode) || opt.dir_as_file) {
        print_file_info(path, path, &st, opt);
        if (!opt.long_format) printf("\n");
        return;
    }

    DIR *dir = opendir(path);
    if (!dir) {
        perror("ls");
        return;
    }

    Entry *entries = NULL;
    size_t count = 0;
    struct dirent *entry;

    while ((entry = readdir(dir))) {
        if (!opt.show_all && !opt.almost_all && entry->d_name[0] == '.')
            continue;
        if (opt.almost_all && (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")))
            continue;

        Entry *tmp = realloc(entries, (count + 1) * sizeof(Entry));
        if (!tmp) {
            perror("realloc");
            free(entries);
            closedir(dir);
            return;
        }
        entries = tmp;

        snprintf(entries[count].name, sizeof(entries[count].name), "%s", entry->d_name);
        char fullpath[512];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        if (lstat(fullpath, &entries[count].st) < 0) {
            perror("stat");
            memset(&entries[count].st, 0, sizeof(struct stat));
        }
        count++;
    }
    closedir(dir);

    if (count == 0) {
        printf("%s:\n(empty)\n", path);
        return;
    }

    if (!opt.no_sort) {
        if (opt.sort_size)
            qsort(entries, count, sizeof(Entry), compare_size);
        else if (opt.sort_time)
            qsort(entries, count, sizeof(Entry), compare_time);
        else
            qsort(entries, count, sizeof(Entry), compare_name);

        if (opt.reverse) {
            for (size_t i = 0; i < count / 2; i++) {
                Entry tmp = entries[i];
                entries[i] = entries[count - 1 - i];
                entries[count - 1 - i] = tmp;
            }
        }
    }

    printf("\n%s:\n", path);
    for (size_t i = 0; i < count; i++)
        print_file_info(path, entries[i].name, &entries[i].st, opt);
    if (!opt.long_format) printf("\n");

    if (opt.recursive) {
        for (size_t i = 0; i < count; i++) {
            if (S_ISDIR(entries[i].st.st_mode) &&
                strcmp(entries[i].name, ".") &&
                strcmp(entries[i].name, "..")) {

                char next[512];
                snprintf(next, sizeof(next), "%s/%s", path, entries[i].name);
                list_directory(next, opt);
            }
        }
    }

    free(entries);
}
