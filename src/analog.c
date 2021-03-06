#include <pebble.h>
#include "ui.h"

static Layer *s_window_layer;

static GBitmap *s_board_bitmap;
static GBitmap *s_bezel_bitmap;
static Layer *s_board_layer;
static BitmapLayer *s_bezel_bitmap_layer;

static GPoint s_center;

static void draw_board(Layer *layer, GContext *ctx, int time_angle) {
  // GRect ctx_bounds = layer_get_bounds(layer);
  // GPoint ctx_ic = grect_center_point(&ctx_bounds);

  GRect board_bounds = gbitmap_get_bounds(s_board_bitmap);
  GPoint board_ic = grect_center_point(&board_bounds);

  // rotate 180 angle when time is 00:00
  int angle = ((180 + time_angle) * TRIG_MAX_ANGLE) / 360;
  graphics_draw_rotated_bitmap(ctx, s_board_bitmap, board_ic, angle, s_center);
}

static void board_update_proc(Layer *layer, GContext *ctx) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "board_update_proc");
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
#if USE_DEMO_MODE
  set_time_for_screenshot(tick_time);
#endif
  // int time_angle = (360 * ((tick_time->tm_hour * 3600) + (tick_time->tm_min * 60))) / 86400;
  int time_angle = (360 * (tick_time->tm_hour * 60 + tick_time->tm_min)) / 1440;
  draw_board(layer, ctx, time_angle);
}

static void init_board() {
  GRect window_bounds = layer_get_bounds(s_window_layer);

  s_board_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BOARD);
  s_board_layer = layer_create(window_bounds);
  layer_set_update_proc(s_board_layer, board_update_proc);
  layer_add_child(s_window_layer, s_board_layer);
}

static void deinit_board() {
  layer_destroy(s_board_layer);
  gbitmap_destroy(s_board_bitmap);
}

static void init_bezel() {
  GSize window_size = layer_get_bounds(s_window_layer).size;

  s_bezel_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BEZEL);
  GSize bezel_size = gbitmap_get_bounds(s_bezel_bitmap).size;
  GRect bezel_frame = GRect(s_center.x - bezel_size.w / 2, window_size.h - bezel_size.h, bezel_size.w, bezel_size.h);
  s_bezel_bitmap_layer = bitmap_layer_create(bezel_frame);
  bitmap_layer_set_bitmap(s_bezel_bitmap_layer, s_bezel_bitmap);
  bitmap_layer_set_compositing_mode(s_bezel_bitmap_layer, GCompOpSet);
  layer_add_child(s_window_layer, bitmap_layer_get_layer(s_bezel_bitmap_layer));
}

static void deinit_bezel() {
  bitmap_layer_destroy(s_bezel_bitmap_layer);
  gbitmap_destroy(s_bezel_bitmap);
}

void analog_init_clock(Window *window) {
  s_window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(s_window_layer);
  s_center = grect_center_point(&window_bounds);

  init_board();
  init_bezel();
  analog_update_layout();
}

void analog_update_layout() {
  // GRect unobstructed_bounds = layer_get_unobstructed_bounds(s_window_layer);
  // s_center = grect_center_point(&unobstructed_bounds);
  // layer_set_bounds(s_board_layer, unobstructed_bounds);
  // layer_set_bounds(bitmap_layer_get_layer(s_bezel_bitmap_layer), unobstructed_bounds);
}

void analog_deinit_clock(Window *window) {
  deinit_board();
  deinit_bezel();
}

void analog_update_clock(struct tm *tick_time, TimeUnits units_changed) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "analog_update_clock: %d", units_changed);
  static int last_min_angle = 0;
  int min_angle = 360 * (tick_time->tm_hour * 60 + tick_time->tm_min) / 1440;
  if (min_angle != last_min_angle) {
    layer_mark_dirty(s_board_layer);
    last_min_angle = min_angle;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "board_min_angle: %d", min_angle);
  }
}
