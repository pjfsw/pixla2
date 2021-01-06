#include "directory_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

bool _directory_tree_strendswith(char *str, char *suffix) {
    int len = strlen(str);
    int slen = strlen(suffix);
    if (len >= slen && !strncasecmp(&str[len-slen], suffix, slen)) {
        return true;
    }

    return false;
}



void _directory_tree_read_dir(DirectoryTree *tree, char *path) {
    if (tree->path_count >= DIRECTORY_TREE_FILE_LIMIT-1) {
        return;
    }
    DIR *dir = opendir(path);
    if (dir == NULL) {
        return;
    }
    tree->paths[tree->path_count].path = calloc(strlen(path)+1, 1 );
    strcpy(tree->paths[tree->path_count].path, path);
    char *path_to_save = tree->paths[tree->path_count].path;
    tree->path_count++;

    struct dirent *de;
    while (tree->count < DIRECTORY_TREE_FILE_LIMIT && NULL != (de = readdir(dir))) {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
            continue;
        }
        if (de->d_type == DT_DIR && tree->recursive) {
            char *subpath = calloc(strlen(path)+strlen(de->d_name)+2,1);
            sprintf(subpath, "%s/%s", path, de->d_name);
            _directory_tree_read_dir(tree, subpath);
            free(subpath);
        } else if (_directory_tree_strendswith(de->d_name, tree->suffix)) {
            tree->entries[tree->count].file_name = calloc(strlen(de->d_name)+1, 1);
            tree->entries[tree->count].path = path_to_save;
            strcpy(tree->entries[tree->count].file_name, de->d_name);
            tree->count++;
        }
    }

    closedir(dir);
}

DirectoryTree *directory_tree_create(char *base_path, char *suffix, bool recursive) {
    DirectoryTree *tree = calloc(1, sizeof(DirectoryTree));
    tree->suffix = suffix;
    tree->recursive = recursive;
    _directory_tree_read_dir(tree, base_path);
    return tree;
}

void directory_tree_destroy(DirectoryTree *tree) {
    if (tree == NULL) {
        return;
    }
    for (int i = 0; i < tree->count; i++) {
        if (tree->entries[i].file_name != NULL) {
            free(tree->entries[i].file_name);
        }
    }
    for (int i = 0; i < tree->path_count; i++) {
        if (tree->paths[i].path != NULL) {
            free(tree->paths[i].path);
        }
    }
    free(tree);
}
