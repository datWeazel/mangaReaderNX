#pragma once

extern int current_max_cursor_pos;

int touch_cursor(int x, int y);        // returns cursor position based on the touch location.
int touch_yes_no_option(int x, int y); // returns YES or NO.
