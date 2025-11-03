#ifndef OPTIONS_H
#define OPTIONS_H
#include <stdbool.h>

typedef struct {
    bool show_all;       // -a
    bool almost_all;     // -A
    bool long_format;    // -l
    bool numeric;        // -n
    bool inode;          // -i
    bool block_size;     // -s
    bool human;          // -h
    bool reverse;        // -r
    bool recursive;      // -R
    bool dir_as_file;    // -d
    bool no_sort;        // -f
    bool classify;       // -F
    bool sort_size;      // -S
    bool sort_time;      // -t
    bool use_ctime;      // -c
    bool use_atime;      // -u
    const char *path;
} Options;

Options parse_arguments(int argc, char *argv[]);
#endif
