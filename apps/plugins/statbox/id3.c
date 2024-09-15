#include "id3.h"

static struct multiple_tracks_id3 mul_id3;

static const int32_t units[] =
{
    LANG_BYTE,
    LANG_KIBIBYTE,
    LANG_MEBIBYTE,
    LANG_GIBIBYTE
};

/* Calculate modified FNV hash of string
 * has good avalanche behaviour and uniform distribution
 * see http://home.comcast.net/~bretm/hash/ */
static unsigned int mfnv(char *str)
{
    const unsigned int p = 16777619;
    unsigned int hash = 0x811C9DC5; // 2166136261;

    if (!str)
        return 0;

    while(*str)
        hash = (hash ^ *str++) * p;
    hash += hash << 13;
    hash ^= hash >> 7;
    hash += hash << 3;
    hash ^= hash >> 17;
    hash += hash << 5;
    return hash;
}

static void init_mul_id3(void)
{
    mul_id3.artist_hash = 0;
    mul_id3.album_hash = 0;
    mul_id3.genre_hash = 0;
    mul_id3.composer_hash = 0;
    mul_id3.albumartist_hash = 0;
    mul_id3.grouping_hash = 0;
    mul_id3.comment_hash = 0;
    mul_id3.codectype = 0;
    mul_id3.vbr = false;
    mul_id3.bitrate = 0;
    mul_id3.frequency = 0;
    mul_id3.length = 0;
    mul_id3.filesize = 0;
    mul_id3.year = 0;
}

void collect_id3(struct mp3entry *id3, bool is_first_track)
{
    if (is_first_track)
    {
        init_mul_id3();
        mul_id3.artist_hash = mfnv(id3->artist);
        mul_id3.album_hash = mfnv(id3->album);
        mul_id3.genre_hash = mfnv(id3->genre_string);
        mul_id3.composer_hash = mfnv(id3->composer);
        mul_id3.albumartist_hash = mfnv(id3->albumartist);
        mul_id3.grouping_hash = mfnv(id3->grouping);
        mul_id3.comment_hash = mfnv(id3->comment);
        mul_id3.codectype = id3->codectype;
        mul_id3.vbr = id3->vbr;
        mul_id3.bitrate = id3->bitrate;
        mul_id3.frequency = id3->frequency;
        mul_id3.year = id3->year;
    }
    else
    {
        if (mul_id3.artist_hash && (mfnv(id3->artist) != mul_id3.artist_hash))
            mul_id3.artist_hash = 0;
        if (mul_id3.album_hash && (mfnv(id3->album) != mul_id3.album_hash))
            mul_id3.album_hash = 0;
        if (mul_id3.genre_hash && (mfnv(id3->genre_string) != mul_id3.genre_hash))
            mul_id3.genre_hash = 0;
        if (mul_id3.composer_hash && (mfnv(id3->composer) != mul_id3.composer_hash))
            mul_id3.composer_hash = 0;
        if (mul_id3.albumartist_hash && (mfnv(id3->albumartist) !=
                                          mul_id3.albumartist_hash))
            mul_id3.albumartist_hash = 0;
        if (mul_id3.grouping_hash && (mfnv(id3->grouping) != mul_id3.grouping_hash))
            mul_id3.grouping_hash = 0;
        if (mul_id3.comment_hash && (mfnv(id3->comment) != mul_id3.comment_hash))
            mul_id3.comment_hash = 0;

        if (mul_id3.codectype && (id3->codectype != mul_id3.codectype))
            mul_id3.codectype = AFMT_UNKNOWN;
        if (mul_id3.bitrate && (id3->bitrate != mul_id3.bitrate ||
                                 id3->vbr != mul_id3.vbr))
            mul_id3.bitrate = 0;
        if (mul_id3.frequency && (id3->frequency != mul_id3.frequency))
            mul_id3.frequency = 0;
        if (mul_id3.year && (id3->year != mul_id3.year))
            mul_id3.year = 0;
    }
    mul_id3.length += id3->length;
    mul_id3.filesize += id3->filesize;
}

/* (!) Note unit conversion below
 *
 *     Use result only as input for browse_id3,
 *     with the track_ct parameter set to  > 1.
 */
void finalize_id3(struct mp3entry *id3)
{
    id3->path[0] = '\0';
    id3->title = NULL;
    if (!mul_id3.artist_hash)
        id3->artist = NULL;
    if (!mul_id3.album_hash)
        id3->album = NULL;
    if (!mul_id3.genre_hash)
        id3->genre_string = NULL;
    if (!mul_id3.composer_hash)
        id3->composer = NULL;
    if (!mul_id3.albumartist_hash)
        id3->albumartist = NULL;
    if (!mul_id3.grouping_hash)
        id3->grouping = NULL;
    if (!mul_id3.comment_hash)
        id3->comment = NULL;
    id3->disc_string = NULL;
    id3->track_string = NULL;
    id3->year_string = NULL;
    id3->year = mul_id3.year;
    mul_id3.length /= 1000;  /* convert from ms to s */
    mul_id3.filesize >>= 10; /* convert from B to KiB */
    id3->length = mul_id3.length > ULONG_MAX ? 0 : mul_id3.length;
    id3->filesize = mul_id3.filesize > INT_MAX ? 0 : mul_id3.filesize;
    id3->frequency = mul_id3.frequency;
    id3->bitrate = mul_id3.bitrate;
    id3->codectype = mul_id3.codectype;
    id3->vbr = mul_id3.vbr;
    id3->discnum = 0;
    id3->tracknum = 0;
    id3->track_level = 0;
    id3->album_level = 0;
}

unsigned long human_size(unsigned long long byte_count, int32_t *unit_lang_id)
{
    const size_t n = sizeof(units)/sizeof(units[0]);
    unsigned int i;

    /* margin set at 10K boundary: 10239 B +1 => 10 KB */
    for(i = 0; i < n-1 && byte_count >= 10*1024; i++)
        byte_count >>= 10; /* div by 1024 */

    *unit_lang_id = units[i];
    return (unsigned long)byte_count;
}