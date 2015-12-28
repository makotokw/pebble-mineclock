#include <pebble.h>
#include "ui.h"

static GFont s_time_font;
// static GFont s_info_font;
static TextLayer *s_time_text_layer;
// static TextLayer *s_day_text_layer;
// static TextLayer *s_dow_text_layer;

static void update_clock(struct tm *time) {
  static char s_time_buffer[9];
  // static char s_day_buffer[3];
  // static char s_dow_buffer[4];

  strftime(s_time_buffer, sizeof(s_time_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", time);
  text_layer_set_text(s_time_text_layer, s_time_buffer);

  // strftime(s_day_buffer, sizeof(s_day_buffer), "%d", time);
  // text_layer_set_text(s_day_text_layer, s_day_buffer);
  // strftime(s_dow_buffer, sizeof(s_dow_buffer), "%a", time);
  // text_layer_set_text(s_dow_text_layer, s_dow_buffer);
}

static void init_text_layer(TextLayer *text_layer, bool is_time, GTextAlignment text_alignment) {
  text_layer_set_background_color(text_layer, GColorClear);
  text_layer_set_text_color(text_layer, GColorBlack);
  text_layer_set_text(text_layer, "");
  // text_layer_set_font(text_layer, is_time ? s_time_font : s_info_font);
  text_layer_set_font(text_layer, s_time_font);
  text_layer_set_text_alignment(text_layer, text_alignment);
}

void digital_init_clock(Window* window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MINECRAFTER_24));
  // s_info_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WHITERABBIT_10));

  // time
  s_time_text_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(120, 114), bounds.size.w, 50));
  init_text_layer(s_time_text_layer, true, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_time_text_layer));

  // // day
  // int margin_w = PBL_IF_ROUND_ELSE(12, 6);
  // s_day_text_layer = text_layer_create(GRect(margin_w, bounds.size.h/2 + 18, 50, 25));
  // init_text_layer(s_day_text_layer, false, GTextAlignmentLeft);
  // layer_add_child(window_layer, text_layer_get_layer(s_day_text_layer));
  //
  // // dow
  // s_dow_text_layer = text_layer_create(GRect(bounds.size.w - 50 - margin_w, bounds.size.h/2 + 18, 50, 25));
  // init_text_layer(s_dow_text_layer, false, GTextAlignmentRight);
  // layer_add_child(window_layer, text_layer_get_layer(s_dow_text_layer));

  // update clock
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  update_clock(tick_time);
}

void digital_deinit_clock(Window* window) {
  text_layer_destroy(s_time_text_layer);
  // text_layer_destroy(s_day_text_layer);
  // text_layer_destroy(s_dow_text_layer);
  fonts_unload_custom_font(s_time_font);
  // fonts_unload_custom_font(s_info_font);
}

void digital_update_clock(struct tm *tick_time, TimeUnits units_changed) {
  update_clock(tick_time);
}
