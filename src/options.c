#include <stdio.h>
#include <string.h>
#include "options.h"

Options parse_arguments(int argc, char *argv[]) {
    Options opt = {0};
    opt.path = ".";  // Mặc định liệt kê thư mục hiện tại

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            for (int j = 1; argv[i][j]; j++) {
                switch (argv[i][j]) {
                    case 'a': opt.show_all = true; break;
                    case 'A': opt.almost_all = true; break;
                    case 'l': opt.long_format = true; break;
                    case 'n': opt.numeric = true; break;
                    case 'i': opt.inode = true; break;
                    case 's': opt.block_size = true; break;
                    case 'h': opt.human = true; break;
                    case 'r': opt.reverse = true; break;
                    case 'R': opt.recursive = true; break;
                    case 'd': opt.dir_as_file = true; break;
                    case 'f': opt.no_sort = true; break;
                    case 'F': opt.classify = true; break;
                    case 'S': opt.sort_size = true; break;
                    case 't': opt.sort_time = true; break;
                    case 'c': opt.use_ctime = true; break;
                    case 'u': opt.use_atime = true; break;
                    default:
                        printf("Warning: Unsupported option -%c\n", argv[i][j]);
                }
            }
        } else {
            opt.path = argv[i];
        }
    }
    return opt;
}
