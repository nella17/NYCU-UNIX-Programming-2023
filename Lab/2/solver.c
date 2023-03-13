#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>

char* magic;

int check(const char* path) {
    int filefd = open(path, O_RDONLY);
    int size = lseek(filefd, 0, SEEK_END);
    lseek(filefd, 0, SEEK_SET);
    char* data = malloc(size);
    read(filefd, data, size);
    close(filefd);
    if (strstr(data, magic) != NULL)
        printf("%s\n", path);
    free(data);
    return 0;
}

int find(const char* path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        fprintf(stderr, "%s not found\n", path);
        return -1;
    }

    struct dirent *direct;
    while ((direct = readdir(dir)) != NULL) {
        if (!strcmp(direct->d_name, ".") || !strcmp(direct->d_name, ".."))
            continue;
        char* npath = malloc(strlen(path) + strlen(direct->d_name) + 3);
        sprintf(npath, "%s/%s", path, direct->d_name);
        // printf("%s\n", npath);
        switch (direct->d_type) {
        case DT_DIR:
            find(npath);
            break;
        case DT_REG:
            check(npath);
            break;
        default:
            fprintf(stderr, "unknown type %s %d\n", npath, direct->d_type);
        }
        free(npath);
    }

    closedir(dir);
    return 0;
}

signed main(int argc, const char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <path> <magic>", argv[0]);
        return -1;
    }

    const char* path = argv[1];
    magic = strdup(argv[2]);

    fprintf(stderr, "path: %s\nmagic: %s\n", path, magic);

    find(path);

    return 0;
}
