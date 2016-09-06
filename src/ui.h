#pragma once

#define USE_DEMO_MODE 0

void set_time_for_screenshot(struct tm *tick_time);

void analog_init_clock(Window *window);
void analog_deinit_clock(Window *window);
void analog_update_clock(struct tm *tick_time, TimeUnits units_changed);

void digital_init_clock(Window *window);
void digital_deinit_clock(Window *window);
void digital_update_clock(struct tm *tick_time, TimeUnits units_changed);
