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

#include "plugin.h"
#include "display.h"
#include "utils.h"

void show_splash_screen(void)
{
    rb->lcd_clear_display();
    rb->lcd_puts(0, 0, "Collecting stats...");
    rb->lcd_update();
}

void draw_header(void)
{
    int font_id = FONT_SYSFIXED;
    rb->lcd_setfont(font_id);

    int line_height = rb->font_get(font_id)->height; // Use the loaded font's height

    /* Draw "STATbox" header */
    int header_x = 10;
    int header_y = 5;
    int header_width = LCD_WIDTH - 20;
    int header_height = 30;

    rb->lcd_set_foreground(LCD_RGBPACK(255, 221, 79)); /* Set background color to a ROCKbox yellow */

    /* Draw the rectangle for the header */
    rb->lcd_fillrect(header_x, header_y, header_width, header_height);

    /* Calculate positions for "STAT" and "box" parts */
    int stat_width = rb->lcd_getstringsize("STAT", NULL, NULL);
    int box_width = rb->lcd_getstringsize("box", NULL, NULL);
    int total_width = stat_width + box_width;
    int stat_x = LCD_WIDTH / 2 - total_width / 2;
    int box_x = stat_x + stat_width;

    /* Draw "STAT" part */
    rb->lcd_set_background(LCD_RGBPACK(255, 221, 79)); /* Set text background color */
    rb->lcd_set_foreground(LCD_RGBPACK(0, 0, 0));      /* Set text color to black */
    rb->lcd_putsxy(stat_x, header_y + (header_height - line_height) / 2, "STAT");

    /* Draw "box" part */
    rb->lcd_set_foreground(LCD_RGBPACK(180, 196, 213));
    rb->lcd_putsxy(box_x, header_y + (header_height - line_height) / 2, "box");

    /* Draw a line below the header box */
    rb->lcd_set_foreground(LCD_RGBPACK(0, 0, 0));
    rb->lcd_drawline(header_x, header_y + header_height + 5, header_x + header_width, header_y + header_height + 5);
}

void draw_stat_line(const char *label, const char *value, int *text_y, int line_height, int text_x, int value_x)
{
    int centered_y = *text_y + (line_height / 2) - (rb->font_get(FONT_SYSFIXED)->height / 2);
    rb->lcd_putsxy(text_x, centered_y, label);
    rb->lcd_putsxy(value_x - rb->lcd_getstringsize(value, NULL, NULL), centered_y, value);
    *text_y += line_height + 10;
}

void draw_separator_lines(int rect_x, int rect_y, int rect_width, int line_height, int num_lines)
{
    int padding_left = 15; // Padding from the left side
    for (int i = 1; i < num_lines; i++)
    {
        int line_y = rect_y + i * (line_height + 10) - 5;
        rb->lcd_set_foreground(LCD_RGBPACK(141, 140, 142));
        rb->lcd_drawline(rect_x + padding_left, line_y, rect_x + rect_width, line_y);
    }
}

void display_stats(struct dir_stats_custom *custom_stats)
{
    int font_id = FONT_SYSFIXED;
    rb->lcd_setfont(font_id);

    int line_height = rb->font_get(font_id)->height; // Use the loaded font's height

    /* Clear display and show the collected directory stats */
    rb->lcd_clear_display();

    draw_header();

    /* Draw the main rectangle for the stats */
    int rect_x = 10;
    int rect_y = 40;
    int rect_width = LCD_WIDTH - 20;
    int rect_height = 10 * (line_height + 10) + 8 * 5; // 10 lines of text with 8 separators

    rb->lcd_set_foreground(LCD_RGBPACK(28, 28, 30));
    rb->lcd_fillrect(rect_x, rect_y, rect_width, rect_height);

    /* Display stats with black background and white text */
    rb->lcd_set_background(LCD_RGBPACK(28, 28, 30));    /* Set background color to rgb(28, 28, 30) */
    rb->lcd_set_foreground(LCD_RGBPACK(255, 255, 255)); /* Set text color to white */

    int icon_spacing = 20;                  // TODO: Space reserved for an icon
    int text_x = 15 + icon_spacing;         // Adjust text position to leave space for the icon
    int text_y = rect_y + line_height + 10; // Adjusted to push all text down by one line width
    int value_x = rect_x + rect_width - 10; // Right-align values with a 10-pixel padding

    char buffer[32];

    snprintf(buffer, sizeof(buffer), "%.2f MB (%d)", custom_stats->stats.audio_space_used / (1024.0 * 1024.0), custom_stats->stats.audio_file_count);
    draw_stat_line("Audio", buffer, &text_y, line_height, text_x, value_x);

    snprintf(buffer, sizeof(buffer), "%.2f MB (%d)", custom_stats->stats.img_space_used / (1024.0 * 1024.0), custom_stats->stats.img_file_count);
    draw_stat_line("Images", buffer, &text_y, line_height, text_x, value_x);

    snprintf(buffer, sizeof(buffer), "%.2f MB (%d)", custom_stats->stats.vid_space_used / (1024.0 * 1024.0), custom_stats->stats.vid_file_count);
    draw_stat_line("Videos", buffer, &text_y, line_height, text_x, value_x);

    snprintf(buffer, sizeof(buffer), "%.2f MB (%d)", custom_stats->stats.m3u_space_used / (1024.0 * 1024.0), custom_stats->stats.m3u_file_count);
    draw_stat_line("Playlists", buffer, &text_y, line_height, text_x, value_x);

    // Get the battery level
    int battery_level = rb->battery_level();

    // Get the estimated battery time remaining in minutes
    int battery_time = rb->battery_time();

    // Convert battery time to hours and minutes
    int battery_hours = battery_time / 60;
    int battery_minutes = battery_time % 60;

    // Display the battery information on one line
    snprintf(buffer, sizeof(buffer), "%d hr %d min (%d%%)", battery_hours, battery_minutes, battery_level);
    draw_stat_line("Battery", buffer, &text_y, line_height, text_x, value_x);

    snprintf(buffer, sizeof(buffer), "(%d)", custom_stats->stats.dir_count);
    draw_stat_line("Directories", buffer, &text_y, line_height, text_x, value_x);

    snprintf(buffer, sizeof(buffer), "(%d)", custom_stats->stats.file_count);
    draw_stat_line("Files", buffer, &text_y, line_height, text_x, value_x);

    snprintf(buffer, sizeof(buffer), "%.2f MB", custom_stats->stats.total_space_used / (1024.0 * 1024.0));
    draw_stat_line("Total Space Used", buffer, &text_y, line_height, text_x, value_x);

    draw_separator_lines(rect_x, rect_y + line_height + 10, rect_width, line_height, 8);

    /* Update display */
    rb->lcd_update();
}