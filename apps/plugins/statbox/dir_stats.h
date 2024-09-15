#ifndef DIR_STATS_H
#define DIR_STATS_H

#include "statbox.h"

bool collect_dir_stats(struct dir_stats *stats, bool (*id3_cb)(const char*));
bool custom_collect_dir_stats(struct dir_stats_custom *custom_stats, bool (*id3_cb)(const char*));

#endif /* DIR_STATS_H */