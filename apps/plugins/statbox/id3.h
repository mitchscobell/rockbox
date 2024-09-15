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

#ifndef ID3_H
#define ID3_H

#include "plugin.h"

struct multiple_tracks_id3 {
    unsigned long long filesize;
    unsigned long long length;
    unsigned long frequency;
    unsigned int artist_hash;
    unsigned int composer_hash;
    unsigned int albumartist_hash;
    unsigned int grouping_hash;
    unsigned int comment_hash;
    unsigned int album_hash;
    unsigned int genre_hash;
    unsigned int codectype;
    unsigned int bitrate;
    int year;
    bool vbr;
};

void collect_id3(struct mp3entry *id3, bool is_first_track);
void finalize_id3(struct mp3entry *id3);
unsigned long human_size(unsigned long long byte_count, int32_t *unit_lang_id);

#endif /* ID3_H */