#ifndef DIRECTORY_TREE_H_
#define DIRECTORY_TREE_H_

#include <stdbool.h>

#define DIRECTORY_TREE_FILE_LIMIT 2048

typedef struct {
    char *path;
    char *file_name;
} FileEntry;

typedef struct {
    char *path;
} DirectoryTreePath;

typedef struct {
    char *suffix;
    bool recursive;
    FileEntry entries[DIRECTORY_TREE_FILE_LIMIT];
    DirectoryTreePath paths[DIRECTORY_TREE_FILE_LIMIT];
    int count;
    int path_count;
} DirectoryTree;

DirectoryTree *directory_tree_create(char *base_path, char *suffix, bool recursive);

void directory_tree_destroy(DirectoryTree *tree);


#endif /* DIRECTORY_TREE_H_ */
