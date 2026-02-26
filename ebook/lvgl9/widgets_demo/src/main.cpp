/*
 * Widgets demo — Parte 3 del ebook "Crea Interfaces Gráficas con LVGL y ESP32".
 * Misma base que hello_world; muestra una pantalla con label, botón, switch y LED.
 * Requiere LVGL 9, ESP32, TFT ILI9341 por SPI (TFT_eSPI).
 */
#include <Arduino.h>
#include <lvgl.h>
#include <TFT_eSPI.h>

#define MY_DISP_HOR_RES 240
#define MY_DISP_VER_RES 320

lv_display_t * display = NULL;

#define BYTES_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565))
static uint8_t draw_buf[MY_DISP_HOR_RES * MY_DISP_VER_RES / 10 * BYTES_PER_PIXEL];

TFT_eSPI tft = TFT_eSPI(MY_DISP_HOR_RES, MY_DISP_VER_RES);

lv_obj_t *ui_Demo;
lv_obj_t *ui_Titulo;
lv_obj_t *ui_Btn;
lv_obj_t *ui_Switch;
lv_obj_t *ui_Led;

void ui_demo_screen_init(void);

static uint32_t my_get_millis(void) {
  return millis();
}

void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  uint16_t *buf16 = (uint16_t *)px_map;
  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors(buf16, w * h, true);
  tft.endWrite();
  lv_display_flush_ready(disp);
}

void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
  uint16_t touchX = 0, touchY = 0;
  bool touched = tft.getTouch(&touchX, &touchY, 600);
  if (!touched) {
    data->state = LV_INDEV_STATE_REL;
  } else {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = touchX;
    data->point.y = touchY;
  }
}

void setup() {
  Serial.begin(115200);

  tft.begin();
  tft.setRotation(0);
  uint16_t calData[5] = {363, 3382, 297, 3319, 2};
  tft.setTouch(calData);

  lv_init();
  lv_tick_set_cb(my_get_millis);
  display = lv_display_create(MY_DISP_HOR_RES, MY_DISP_VER_RES);
  lv_display_set_buffers(display, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_flush_cb(display, my_disp_flush);

  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, my_touchpad_read);

  ui_demo_screen_init();

  Serial.println("Widgets demo ready");
}

void loop() {
  lv_timer_handler();
  delay(5);
}

void ui_demo_screen_init(void) {
  ui_Demo = lv_obj_create(NULL);

  lv_obj_clear_flag(ui_Demo, LV_OBJ_FLAG_SCROLLABLE);

  ui_Titulo = lv_label_create(ui_Demo);
  lv_label_set_text(ui_Titulo, "Widgets basicos");
  lv_obj_set_width(ui_Titulo, LV_SIZE_CONTENT);
  lv_obj_set_height(ui_Titulo, LV_SIZE_CONTENT);
  lv_obj_align(ui_Titulo, LV_ALIGN_TOP_MID, 0, 20);

  ui_Btn = lv_button_create(ui_Demo);
  lv_obj_set_size(ui_Btn, 120, 40);
  lv_obj_align(ui_Btn, LV_ALIGN_TOP_MID, 0, 60);
  lv_obj_t *btn_label = lv_label_create(ui_Btn);
  lv_label_set_text(btn_label, "Pulsar");
  lv_obj_center(btn_label);

  ui_Switch = lv_switch_create(ui_Demo);
  lv_obj_align(ui_Switch, LV_ALIGN_LEFT_MID, 40, 20);
  lv_obj_set_style_bg_color(ui_Switch,  lv_color_black(), LV_PART_MAIN);

  ui_Led = lv_led_create(ui_Demo);
  lv_obj_set_size(ui_Led, 24, 24);
  lv_obj_align(ui_Led, LV_ALIGN_RIGHT_MID, -40, 20);
  lv_led_set_color(ui_Led, lv_color_hex(0x00ff00));
  lv_led_on(ui_Led);

  lv_disp_load_scr(ui_Demo);
}
