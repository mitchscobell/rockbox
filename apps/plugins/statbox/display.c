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
    rb->lcd_set_foreground(LCD_RGBPACK(0, 0, 0)); /* Set text color to black */
    rb->lcd_putsxy(stat_x, header_y + (header_height - line_height) / 2, "STAT");

    /* Draw "box" part */
    rb->lcd_set_foreground(LCD_RGBPACK(180, 196, 213)); /* Set text color to light blue */
    rb->lcd_putsxy(box_x, header_y + (header_height - line_height) / 2, "box");

    /* Draw a line below the header box */
    rb->lcd_set_foreground(LCD_RGBPACK(255, 255, 255)); /* Set line color to white */
    rb->lcd_drawline(header_x, header_y + header_height + 5, header_x + header_width, header_y + header_height + 5);

    /* Display stats with black background and white text */
    int vertical_offset = 60; // Increased spacing
    int spacing = 25;

    rb->lcd_set_background(LCD_RGBPACK(0, 0, 0)); /* Set background color to black */
    rb->lcd_set_foreground(LCD_RGBPACK(255, 255, 255)); /* Set text color to white */

    rb->lcd_putsxyf(10, vertical_offset, "Files: %d", custom_stats->stats.file_count);
    rb->lcd_putsxyf(10, vertical_offset + spacing, "Audio: %d (%.2f MB)", custom_stats->stats.audio_file_count, custom_stats->stats.audio_space_used / (1024.0 * 1024.0));
    rb->lcd_putsxyf(10, vertical_offset + 2 * spacing, "Images: %d (%.2f MB)", custom_stats->stats.img_file_count, custom_stats->stats.img_space_used / (1024.0 * 1024.0));
    rb->lcd_putsxyf(10, vertical_offset + 3 * spacing, "Videos: %d (%.2f MB)", custom_stats->stats.vid_file_count, custom_stats->stats.vid_space_used / (1024.0 * 1024.0));
    rb->lcd_putsxyf(10, vertical_offset + 4 * spacing, "Playlists: %d (%.2f MB)", custom_stats->stats.m3u_file_count, custom_stats->stats.m3u_space_used / (1024.0 * 1024.0));
    rb->lcd_putsxyf(10, vertical_offset + 5 * spacing, "Directories: %d", custom_stats->stats.dir_count);
    rb->lcd_putsxyf(10, vertical_offset + 6 * spacing, "Total Space Used: %.2f MB", custom_stats->stats.total_space_used / (1024.0 * 1024.0)); // Display total space used in MB

    /* Update display */
    rb->lcd_update();
}