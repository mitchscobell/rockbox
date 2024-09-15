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

#ifndef DIR_STATS_H
#define DIR_STATS_H

#include "statbox.h"

struct dir_stats {
    char dirname[MAX_PATH];
    unsigned int dir_count;
    unsigned int file_count;
    unsigned int audio_file_count;
    unsigned int img_file_count;
    unsigned int vid_file_count;
    unsigned int m3u_file_count;
    unsigned long byte_count;
    unsigned long total_space_used; // New field for total space used
    bool count_all;
};

struct dir_stats_custom {
    struct dir_stats stats;
    // Add any custom fields here if needed
};

bool collect_dir_stats(struct dir_stats *stats, bool (*id3_cb)(const char*));
bool custom_collect_dir_stats(struct dir_stats_custom *custom_stats, bool (*id3_cb)(const char*));

#endif /* DIR_STATS_H */