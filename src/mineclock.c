#include <pebble.h>
#include "ui.h"

static Window *s_window;

#if USE_DEMO_MODE
void set_time_for_screenshot(struct tm *tick_time) {
  tick_time->tm_mon = 0;
  tick_time->tm_mday = 1;
  tick_time->tm_hour = (((tick_time->tm_min * 60 + tick_time->tm_sec) / 5) % 4) * 6;
  tick_time->tm_min = 0;
}
#endif

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  // APP_LOG(APP_LOG_LEVEL_DEBUG, "tick_handler %d", units_changed);
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

static void unobstructed_are_did_change(void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "unobstructed_are_did_change");
  analog_update_layout();
  digital_update_layout();
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

#if PBL_PLATFORM_APLITE
#else
  unobstructed_area_service_subscribe((UnobstructedAreaHandlers){
    .did_change = unobstructed_are_did_change,
  }, NULL);
#endif

#if USE_DEMO_MODE
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
#else
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
#endif
}

static void deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
