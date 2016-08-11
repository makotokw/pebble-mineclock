#include <pebble.h>
#include "ui.h"

static GFont s_time_font;
static TextLayer *s_time_text_layer;

static GFont s_day_font;
static TextLayer *s_dom_text_layer, *s_dow_text_layer;

static void update_clock(struct tm *time) {
  static char s_time_buffer[9];
  static char s_dow_buffer[4];
  static char s_dom_buffer[3];
  strftime(s_time_buffer, sizeof(s_time_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", time);
  text_layer_set_text(s_time_text_layer, s_time_buffer);

  strftime(s_dow_buffer, sizeof(s_dow_buffer), "%a", time);
  text_layer_set_text(s_dow_text_layer, s_dow_buffer);
  strftime(s_dom_buffer, sizeof(s_dom_buffer), "%d", time);
  text_layer_set_text(s_dom_text_layer, s_dom_buffer);
}

static void init_text_layer(TextLayer *text_layer, bool is_time, GTextAlignment text_alignment) {
  text_layer_set_background_color(text_layer, GColorClear);
  text_layer_set_text_color(text_layer, GColorBlack);
  text_layer_set_text(text_layer, "");
  text_layer_set_font(text_layer, is_time ? s_time_font : s_day_font);
  text_layer_set_text_alignment(text_layer, text_alignment);
}

void digital_init_clock(Window* window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MINECRAFTER_24));
  s_day_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WHITERABBIT_10));

  // time
  int time_text_y = PBL_IF_ROUND_ELSE(120, 114);
  int day_text_y = time_text_y + 28;
  s_time_text_layer = text_layer_create(GRect(0, time_text_y, bounds.size.w, 50));
  init_text_layer(s_time_text_layer, true, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_time_text_layer));

  // day of week
  s_dow_text_layer = text_layer_create(GRect(bounds.size.w/2 - 50 - 2, day_text_y, 50, 25));
  init_text_layer(s_dow_text_layer, false, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(s_dow_text_layer));

  // day of month
  int margin_w = PBL_IF_ROUND_ELSE(12, 6);
  s_dom_text_layer = text_layer_create(GRect(bounds.size.w/2 + 2, day_text_y, 50, 25));
  init_text_layer(s_dom_text_layer, false, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(s_dom_text_layer));

  // update clock
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  digital_update_clock(tick_time, 0);
}

void digital_deinit_clock(Window* window) {
  text_layer_destroy(s_time_text_layer);
  text_layer_destroy(s_dom_text_layer);
  text_layer_destroy(s_dow_text_layer);
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_day_font);
}

void digital_update_clock(struct tm *tick_time, TimeUnits units_changed) {
  static int last_hour = 0;
  static int last_min = 0;
  // set_time_for_screenshot(tick_time);
  if (last_hour != tick_time->tm_hour || last_min != tick_time->tm_min) {
    update_clock(tick_time);
    last_hour = tick_time->tm_hour;
    last_min = tick_time->tm_min;
  }
}
