#include <pebble.h>
#include "ui.h"

static Window *s_window;

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
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

static void init(void) {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(s_window, animated);

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
