/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2024 Mitch Scobell
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/

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