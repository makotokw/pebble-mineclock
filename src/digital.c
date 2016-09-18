#include <pebble.h>
#include "ui.h"

static Layer *s_window_layer;

static GFont s_time_font;
static GFont s_time_small_font;
static TextLayer *s_time_text_layer;

static GFont s_day_font;
static TextLayer *s_dom_text_layer, *s_dow_text_layer;

static int s_last_hour = -1;
static int s_last_min = -1;

static void reset_text() {
  text_layer_set_text(s_time_text_layer, "");
  text_layer_set_text(s_dow_text_layer, "");
  text_layer_set_text(s_dom_text_layer, "");
}

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

static void update_clock_at_current_time() {
  // update clock
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  digital_update_clock(tick_time, 0);
}

static void init_text_layer(TextLayer *text_layer, GTextAlignment text_alignment) {
  text_layer_set_background_color(text_layer, GColorClear);
  text_layer_set_text_color(text_layer, GColorBlack);
  text_layer_set_text(text_layer, "");
  text_layer_set_text_alignment(text_layer, text_alignment);
}

void digital_init_clock(Window* window) {
  s_window_layer = window_get_root_layer(window);
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MINECRAFTER_25));
  s_time_small_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MINECRAFTER_18));
  s_day_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WHITERABBIT_12));

  s_time_text_layer = text_layer_create(GRect(0, 0, 0, 0));
  s_dom_text_layer = text_layer_create(GRect(0, 0, 0, 0));
  s_dow_text_layer = text_layer_create(GRect(0, 0, 0, 0));
  layer_add_child(s_window_layer, text_layer_get_layer(s_time_text_layer));
  layer_add_child(s_window_layer, text_layer_get_layer(s_dom_text_layer));
  layer_add_child(s_window_layer, text_layer_get_layer(s_dow_text_layer));

  digital_update_layout();
}

static void do_layout_fullscreen(GRect bounds)
{
  int time_text_y = PBL_IF_ROUND_ELSE(117, 112);
  int day_text_y = time_text_y + 29;

  // time
  layer_set_frame(text_layer_get_layer(s_time_text_layer), GRect(0, time_text_y, bounds.size.w, 50));
  init_text_layer(s_time_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_time_text_layer, s_time_font);

  // day of week
  layer_set_frame(text_layer_get_layer(s_dow_text_layer), GRect(bounds.size.w/2 - 50, day_text_y, 50, 25));
  init_text_layer(s_dow_text_layer, GTextAlignmentRight);
  text_layer_set_font(s_dow_text_layer, s_day_font);

  // day of month
  int margin_w = PBL_IF_ROUND_ELSE(12, 6);
  layer_set_frame(text_layer_get_layer(s_dom_text_layer), GRect(bounds.size.w/2 + 4, day_text_y, 50, 25));
  init_text_layer(s_dom_text_layer, GTextAlignmentLeft);
  text_layer_set_font(s_dom_text_layer, s_day_font);
}

static void do_layout_unobstructed(GRect bounds, GRect unobstructed_bounds)
{
  int time_text_y = bounds.size.h / 2 + 13;
  int day_text_y = bounds.size.h / 2 + 19;

  // time
  layer_set_frame(text_layer_get_layer(s_time_text_layer), GRect(0, time_text_y, bounds.size.w, 50));
  init_text_layer(s_time_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_time_text_layer, s_time_small_font);

  // day of week
  layer_set_frame(text_layer_get_layer(s_dow_text_layer), GRect(5, day_text_y, 50, 25));
  init_text_layer(s_dow_text_layer, GTextAlignmentLeft);
  text_layer_set_font(s_dow_text_layer, s_day_font);

  // day of month
  int margin_w = PBL_IF_ROUND_ELSE(12, 6);
  layer_set_frame(text_layer_get_layer(s_dom_text_layer), GRect(bounds.size.w - 55, day_text_y, 50, 25));
  init_text_layer(s_dom_text_layer, GTextAlignmentRight);
  text_layer_set_font(s_dom_text_layer, s_day_font);
}

void digital_update_layout() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "digital_update_layout");
  GRect bounds = layer_get_bounds(s_window_layer);
  GRect unobstructed_bounds = layer_get_unobstructed_bounds(s_window_layer);
  int16_t obstruction_height = bounds.size.h - unobstructed_bounds.size.h;

  s_last_hour = -1;
  s_last_min = -1;

#if PBL_ROUND
  do_layout_fullscreen(bounds);
#else
  if (obstruction_height == 0) {
    do_layout_fullscreen(bounds);
  } else { // unobstructed
    do_layout_unobstructed(bounds, unobstructed_bounds);
  }
#endif

  update_clock_at_current_time();
}

void digital_deinit_clock(Window* window) {
  text_layer_destroy(s_time_text_layer);
  text_layer_destroy(s_dom_text_layer);
  text_layer_destroy(s_dow_text_layer);
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_time_small_font);
  fonts_unload_custom_font(s_day_font);
}

void digital_update_clock(struct tm *tick_time, TimeUnits units_changed) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "digital_update_clock: %d", units_changed);
#if USE_DEMO_MODE
  set_time_for_screenshot(tick_time);
#endif
  if (s_last_hour != tick_time->tm_hour || s_last_min != tick_time->tm_min) {
    update_clock(tick_time);
    s_last_hour = tick_time->tm_hour;
    s_last_min = tick_time->tm_min;
  }
}
