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

void display_stats(struct dir_stats_custom *custom_stats)
{
    int font_id = FONT_SYSFIXED; // Use a larger built-in font
    rb->lcd_setfont(font_id);

    int line_height = rb->font_get(font_id)->height; // Use the loaded font's height

    /* Clear display and show the collected directory stats */
    rb->lcd_clear_display();

    /* Draw a visually appealing "STATbox" header */
    int header_x = 10;
    int header_y = 5;
    int header_width = LCD_WIDTH - 20;
    int header_height = 30;

    rb->lcd_set_foreground(LCD_RGBPACK(255, 221, 79)); /* Set background color to a light yellow */

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
    rb->lcd_set_foreground(LCD_RGBPACK(180, 196, 213)); /* Set text color to light blue */
    rb->lcd_putsxy(box_x, header_y + (header_height - line_height) / 2, "box");

    /* Draw a line below the header box */
    rb->lcd_set_foreground(LCD_RGBPACK(255, 255, 255)); /* Set line color to white */
    rb->lcd_drawline(header_x, header_y + header_height + 5, header_x + header_width, header_y + header_height + 5);

    /* Draw the main rectangle for the stats */
    int rect_x = 10;
    int rect_y = 60;
    int rect_width = LCD_WIDTH - 20;
    int rect_height = 7 * (line_height + 10) + 6 * 5; // 7 lines of text with 6 separators

    rb->lcd_set_foreground(LCD_RGBPACK(28, 28, 30)); /* Set background color to rgb(28, 28, 30) */
    rb->lcd_fillrect(rect_x, rect_y, rect_width, rect_height);

    /* Display stats with black background and white text */
    rb->lcd_set_background(LCD_RGBPACK(28, 28, 30));    /* Set background color to rgb(28, 28, 30) */
    rb->lcd_set_foreground(LCD_RGBPACK(255, 255, 255)); /* Set text color to white */

    int icon_spacing = 20;          // Space reserved for the icon
    int text_x = 15 + icon_spacing; // Adjust text position to leave space for the icon
    int text_y = rect_y + 5;
    int value_x = rect_x + rect_width - 10; // Right-align values with a 10-pixel padding

    rb->lcd_putsxy(text_x, text_y, "Audio");
    rb->lcd_putsxyf(value_x - lcd_getstringsizef("%.2f MB (%d)", custom_stats->stats.audio_space_used / (1024.0 * 1024.0), custom_stats->stats.audio_file_count), text_y, "%.2f MB (%d)", custom_stats->stats.audio_space_used / (1024.0 * 1024.0), custom_stats->stats.audio_file_count);
    text_y += line_height + 10;

    rb->lcd_putsxy(text_x, text_y, "Images");
    rb->lcd_putsxyf(value_x - lcd_getstringsizef("%.2f MB (%d)", custom_stats->stats.img_space_used / (1024.0 * 1024.0), custom_stats->stats.img_file_count), text_y, "%.2f MB (%d)", custom_stats->stats.img_space_used / (1024.0 * 1024.0), custom_stats->stats.img_file_count);
    text_y += line_height + 10;

    rb->lcd_putsxy(text_x, text_y, "Videos");
    rb->lcd_putsxyf(value_x - lcd_getstringsizef("%.2f MB (%d)", custom_stats->stats.vid_space_used / (1024.0 * 1024.0), custom_stats->stats.vid_file_count), text_y, "%.2f MB (%d)", custom_stats->stats.vid_space_used / (1024.0 * 1024.0), custom_stats->stats.vid_file_count);
    text_y += line_height + 10;

    rb->lcd_putsxy(text_x, text_y, "Playlists");
    rb->lcd_putsxyf(value_x - lcd_getstringsizef("%.2f MB (%d)", custom_stats->stats.m3u_space_used / (1024.0 * 1024.0), custom_stats->stats.m3u_file_count), text_y, "%.2f MB (%d)", custom_stats->stats.m3u_space_used / (1024.0 * 1024.0), custom_stats->stats.m3u_file_count);
    text_y += line_height + 10;

    rb->lcd_putsxy(text_x, text_y, "Directories");
    rb->lcd_putsxyf(value_x - lcd_getstringsizef("(%d)", custom_stats->stats.dir_count), text_y, "(%d)", custom_stats->stats.dir_count);
    text_y += line_height + 10;

    rb->lcd_putsxy(text_x, text_y, "Files");
    rb->lcd_putsxyf(value_x - lcd_getstringsizef("(%d)", custom_stats->stats.file_count), text_y, "(%d)", custom_stats->stats.file_count);
    text_y += line_height + 10;

    rb->lcd_putsxy(text_x, text_y, "Total Space Used");
    rb->lcd_putsxyf(value_x - lcd_getstringsizef("%.2f MB", custom_stats->stats.total_space_used / (1024.0 * 1024.0)), text_y, "%.2f MB", custom_stats->stats.total_space_used / (1024.0 * 1024.0));

    /* Draw separator lines between each stat */
    int padding_left = 15; // Padding from the left side
    for (int i = 1; i < 7; i++)
    {
        int line_y = rect_y + i * (line_height + 10);
        rb->lcd_set_foreground(LCD_RGBPACK(141, 140, 142)); /* Set line color to rgb(141, 140, 142) */
        rb->lcd_drawline(rect_x + padding_left, line_y, rect_x + rect_width, line_y);
    }

    /* Update display */
    rb->lcd_update();
}