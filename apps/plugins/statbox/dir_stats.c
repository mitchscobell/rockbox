#include "dir_stats.h"
#include "id3.h"

static const char *image_exts[] = {"bmp","jpg","jpe","jpeg","png","ppm"};
static const char *video_exts[] = {"mpg","mpeg","mpv","m2v"};

bool collect_dir_stats(struct dir_stats *stats, bool (*id3_cb)(const char*))
{
    bool result = true;
    int files_in_dir = 0; // Changed from unsigned int to int
    static unsigned int id3_count;
    static unsigned long last_get_action;
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

            int attr = rb->filetype_get_attr(entry->d_name);
            if (attr == FILE_ATTR_AUDIO)
                stats->audio_file_count++;
            else if (attr == FILE_ATTR_M3U)
                stats->m3u_file_count++;
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
                        break;
                    }
                }
                if (i >= ARRAYLEN(image_exts)) {
                    for(i = 0; i < ARRAYLEN(video_exts); i++) {
                        if(!rb->strcasecmp(ptr, video_exts[i])) {
                            stats->vid_file_count++;
                            break;
                        }
                    }
                }
            }
        }
        else if (rb->filetype_get_attr(entry->d_name) == FILE_ATTR_AUDIO)
        {
            rb->splash_progress(id3_count++, stats->audio_file_count,
                                "%s (%s)",
                                rb->str(LANG_WAIT), rb->str(LANG_OFF_ABORT));
            rb->snprintf(stats->dirname + dirlen, sizeof(stats->dirname) - dirlen,
                         "/%s", entry->d_name); /* append name to current directory */
            id3_cb(stats->dirname); /* allow metadata to be collected */
        }

        if (TIME_AFTER(*(rb->current_tick), last_get_action + HZ/8))
        {
            if(ACTION_STD_CANCEL == rb->get_action(CONTEXT_STD,TIMEOUT_NOBLOCK))
            {
                stats->canceled = true;
                result = false;
            }
            last_get_action = *(rb->current_tick);
        }
        rb->yield();
    }
    rb->closedir(dir);
    if (stats->max_files_in_dir < files_in_dir)
        stats->max_files_in_dir = files_in_dir;
    return result;
}

bool custom_collect_dir_stats(struct dir_stats_custom *custom_stats, bool (*id3_cb)(const char*))
{
    custom_stats->suppress_display = true; // Disable intermediate display updates
    return collect_dir_stats(&custom_stats->stats, id3_cb);
}