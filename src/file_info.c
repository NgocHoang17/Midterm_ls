#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "file_info.h"

static void human_readable(off_t size, char *buf) {
    const char *units[] = {"B","K","M","G","T"};
    int i = 0;
    double s = (double)size;
    while (s >= 1024 && i < 4) { s /= 1024; i++; }
    sprintf(buf, "%.1f%s", s, units[i]);
}

void print_permissions(mode_t mode) {
    char perm[11];
    perm[0] = S_ISDIR(mode) ? 'd' : S_ISLNK(mode) ? 'l' :
              S_ISCHR(mode) ? 'c' : S_ISBLK(mode) ? 'b' :
              S_ISFIFO(mode)? 'p' : S_ISSOCK(mode)? 's' : '-';
    const char chars[] = {'r','w','x'};
    for (int i = 0; i < 9; i++)
        perm[i+1] = (mode & (1 << (8 - i))) ? chars[i%3] : '-';
    perm[10] = '\0';
    printf("%s", perm);
}

void print_file_info(const char *path, const char *filename, struct stat *st, Options opt) {
    if (opt.long_format) {
        char timebuf[32];
        time_t t = st->st_mtime;
        if (opt.use_ctime) t = st->st_ctime;
        if (opt.use_atime) t = st->st_atime;
        strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&t));

        struct passwd *pw = getpwuid(st->st_uid);
        struct group *gr = getgrgid(st->st_gid);
        const char *user = pw ? pw->pw_name : "?";
        const char *group = gr ? gr->gr_name : "?";

        if (opt.inode) printf("%6ld ", st->st_ino);
        if (opt.block_size) printf("%4ld ", st->st_blocks / 2);

        print_permissions(st->st_mode);
        printf(" %3ld ", st->st_nlink);

        if (opt.numeric)
            printf("%d %d ", st->st_uid, st->st_gid);
        else
            printf("%-8s %-8s ", user, group);

        char sizebuf[16];
        if (opt.human) human_readable(st->st_size, sizebuf);
        else sprintf(sizebuf, "%ld", st->st_size);

        printf("%8s %s %s", sizebuf, timebuf, filename);

        if (opt.classify) {
            if (S_ISDIR(st->st_mode)) printf("/");
            else if (S_ISLNK(st->st_mode)) printf("@");
            else if (S_ISFIFO(st->st_mode)) printf("|");
            else if (S_ISSOCK(st->st_mode)) printf("=");
            else if (st->st_mode & S_IXUSR) printf("*");
        }
        printf("\n");
    } else {
        if (opt.inode) printf("%6ld ", st->st_ino);
        if (opt.block_size) printf("%4ld ", st->st_blocks / 2);
        printf("%s  ", filename);
    }
}
