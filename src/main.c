#include "list_dir.h"
#include "options.h"

int main(int argc, char *argv[]) {
    Options opt = parse_arguments(argc, argv);
    list_directory(opt.path, opt);
    return 0;
}
