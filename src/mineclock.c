#include <pebble.h>
#include "ui.h"

static Window *s_window;

/*
void set_time_for_screenshot(struct tm *tick_time) {
  tick_time->tm_hour = 6 * (int)(tick_time->tm_sec / 15);
  tick_time->tm_min = 0;
}
*/

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "tick_handler %d", units_changed);
  analog_update_clock(tick_time, units_changed);
  digital_update_clock(tick_time, units_changed);
}

static void window_load(Window *window) {
  analog_init_clock(window);
  digital_init_clock(window);
}

static void window_unload(Window *window) {
  analog_deinit_clock(window);
  digital_deinit_clock(window);
}

static void app_did_focus(bool in_focus) {
  if (in_focus) {
    Layer *window_layer = window_get_root_layer(s_window);
    layer_mark_dirty(window_layer);
  }
}

static void init(void) {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(s_window, animated);

  app_focus_service_subscribe_handlers((AppFocusHandlers){
    .did_focus = app_did_focus,
  });

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
