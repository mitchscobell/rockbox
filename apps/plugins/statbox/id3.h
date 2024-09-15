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