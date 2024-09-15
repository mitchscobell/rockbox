#include "statbox.h"
#include "dir_stats.h"
#include "display.h"

enum plugin_status plugin_start(const void* parameter)
{
    (void)parameter;
    int button, success;
    static struct dir_stats_custom custom_stats;
    custom_stats.stats.dirname[0] = '/';
    custom_stats.stats.count_all = true;

#ifdef HAVE_ADJUSTABLE_CPU_FREQ
    rb->cpu_boost(true);
#endif

    /* Collect stats without displaying intermediate results */
    success = custom_collect_dir_stats(&custom_stats, NULL);

#ifdef HAVE_ADJUSTABLE_CPU_FREQ
    rb->cpu_boost(false);
#endif

    if (!success)
        return PLUGIN_OK;

    /* Clear display before showing stats */
    rb->lcd_clear_display();
    display_stats(&custom_stats);

    /* Wait for user interaction */
    while (1)
    {
        switch (button = rb->get_action(CONTEXT_STD, TIMEOUT_BLOCK))
        {
            case ACTION_STD_CANCEL:
                return PLUGIN_OK;
            default:
                if (rb->default_event_handler(button) == SYS_USB_CONNECTED)
                    return PLUGIN_USB_CONNECTED;
        }
    }

    return PLUGIN_OK;
}