#ifndef FILE_INFO_H
#define FILE_INFO_H
#include <sys/stat.h>
#include "options.h"

void print_permissions(mode_t mode);
void print_file_info(const char *path, const char *filename, struct stat *st, Options opt);

#endif
