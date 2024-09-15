#ifndef STATBOX_H
#define STATBOX_H

#include "plugin.h"

#define MAX_PATH 260

struct dir_stats {
    char dirname[MAX_PATH];
    int file_count;
    int audio_file_count;
    int img_file_count;
    int vid_file_count;
    int m3u_file_count;
    int dir_count;
    bool count_all;
    unsigned long long byte_count;
    int max_files_in_dir;
    bool canceled;
};

struct dir_stats_custom {
    struct dir_stats stats;
    bool suppress_display;
};

enum plugin_status plugin_start(const void* parameter);

#endif /* STATBOX_H */