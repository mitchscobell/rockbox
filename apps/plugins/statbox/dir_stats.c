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

#include "dir_stats.h"
#include "id3.h"

static const char *image_exts[] = {"bmp","jpg","jpe","jpeg","png","ppm"};
static const char *video_exts[] = {"mpg","mpeg","mpv","m2v"};

bool collect_dir_stats(struct dir_stats *stats, bool (*id3_cb)(const char*))
{
    bool result = true;
    int files_in_dir = 0; // Changed from unsigned int to int
    struct dirent* entry;
    int dirlen = rb->strlen(stats->dirname);
    DIR* dir =  rb->opendir(stats->dirname);
    if (!dir)
    {
        rb->splashf(HZ*2, "open error: %s", stats->dirname);
        return false;
    }
    else if (!stats->dirname[1]) /* root dir */
        stats->dirname[0] = dirlen = 0;

    /* walk through the directory content */
    while(result && (0 != (entry = rb->readdir(dir))))
    {
        struct dirinfo info = rb->dir_get_info(dir, entry);
        if (info.attribute & ATTR_DIRECTORY)
        {
            if (!rb->strcmp((char *)entry->d_name, ".") ||
                !rb->strcmp((char *)entry->d_name, ".."))
                continue; /* skip these */

            rb->snprintf(stats->dirname + dirlen, sizeof(stats->dirname) - dirlen,
                         "/%s", entry->d_name); /* append name to current directory */
            if (!id3_cb)
            {
                stats->dir_count++; /* new directory */
            }
            result = collect_dir_stats(stats, id3_cb); /* recursion */
        }
        else if (!id3_cb)
        {
            char *ptr;
            stats->file_count++; /* new file */
            files_in_dir++;
            stats->byte_count += info.size;
            stats->total_space_used += info.size; // Add file size to total space used

            int attr = rb->filetype_get_attr(entry->d_name);
            if (attr == FILE_ATTR_AUDIO)
            {
                stats->audio_file_count++;
                stats->audio_space_used += info.size; // Add file size to audio space used
            }
            else if (attr == FILE_ATTR_M3U)
            {
                stats->m3u_file_count++;
                stats->m3u_space_used += info.size; // Add file size to playlist space used
            }
            /* image or video file attributes have to be compared manually */
            else if (stats->count_all &&
                     (ptr = rb->strrchr(entry->d_name,'.')))
            {
                unsigned int i;
                ptr++;
                for(i = 0; i < ARRAYLEN(image_exts); i++)
                {
                    if(!rb->strcasecmp(ptr, image_exts[i]))
                    {
                        stats->img_file_count++;
                        stats->img_space_used += info.size; // Add file size to image space used
                        break;
                    }
                }
                if (i >= ARRAYLEN(image_exts)) {
                    for(i = 0; i < ARRAYLEN(video_exts); i++) {
                        if(!rb->strcasecmp(ptr, video_exts[i])) {
                            stats->vid_file_count++;
                            stats->vid_space_used += info.size; // Add file size to video space used
                            break;
                        }
                    }
                }
            }
        }
    }

    rb->closedir(dir);
    return result;
}

bool custom_collect_dir_stats(struct dir_stats_custom *custom_stats, bool (*id3_cb)(const char*))
{
    return collect_dir_stats(&custom_stats->stats, id3_cb);
}