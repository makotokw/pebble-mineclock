#include <pebble.h>
#include "ui.h"

static GFont s_time_font;
static TextLayer *s_time_text_layer;

static void update_clock(struct tm *time) {
  static char s_time_buffer[9];
  strftime(s_time_buffer, sizeof(s_time_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", time);
  text_layer_set_text(s_time_text_layer, s_time_buffer);
}

static void init_text_layer(TextLayer *text_layer, bool is_time, GTextAlignment text_alignment) {
  text_layer_set_background_color(text_layer, GColorClear);
  text_layer_set_text_color(text_layer, GColorBlack);
  text_layer_set_text(text_layer, "");
  text_layer_set_font(text_layer, s_time_font);
  text_layer_set_text_alignment(text_layer, text_alignment);
}

void digital_init_clock(Window* window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MINECRAFTER_24));

  // time
  s_time_text_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(120, 114), bounds.size.w, 50));
  init_text_layer(s_time_text_layer, true, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_time_text_layer));

  // update clock
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  digital_update_clock(tick_time, 0);
}

void digital_deinit_clock(Window* window) {
  text_layer_destroy(s_time_text_layer);
  fonts_unload_custom_font(s_time_font);
}

void digital_update_clock(struct tm *tick_time, TimeUnits units_changed) {
  static int last_hour = 0;
  static int last_min = 0;
  if (last_hour != tick_time->tm_hour || last_min != tick_time->tm_min) {
    update_clock(tick_time);
    last_hour = tick_time->tm_hour;
    last_min = tick_time->tm_min;
  }
}
