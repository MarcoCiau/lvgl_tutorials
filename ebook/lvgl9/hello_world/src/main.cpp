/*
 * Hello World — Capítulo 2 del ebook "Crea Interfaces Gráficas con LVGL y ESP32".
 * Mmuestra una pantalla con el texto "Hello :D"
 * Requiere LVGL 9, ESP32, TFT ILI9341 por SPI (TFT_eSPI).
 */
#include <Arduino.h>

#include <lvgl.h>
#include <TFT_eSPI.h>

lv_obj_t *ui_Inicio;
lv_obj_t *ui_Label2;

void ui_init(void);
/*Change to your screen resolution*/

#define MY_DISP_HOR_RES 240
#define MY_DISP_VER_RES 320

lv_display_t * display = NULL; 

/* LVGL will render to this 1/10 screen sized buffer for 2 bytes/pixel (RGB565 color depth) */
#define BYTES_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565))
static uint8_t draw_buf[MY_DISP_HOR_RES * MY_DISP_VER_RES / 10 * BYTES_PER_PIXEL];

/* TFT instance */
TFT_eSPI tft = TFT_eSPI(MY_DISP_HOR_RES, MY_DISP_VER_RES); 

/* Return the elapsed milliseconds since startup.
 * It needs to be implemented by the user */
static uint32_t my_get_millis(void)
{
  return millis();
}

/* Display flushing */
void my_disp_flush(lv_display_t  *disp, const lv_area_t *area, uint8_t * px_map)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  uint16_t * buf16 = (uint16_t *)px_map; /* Let's say it's a 16 bit (RGB565) display */
  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors(buf16, w * h, true);
  tft.endWrite();

  lv_display_flush_ready(disp);
}

/*Read the touchpad*/
// void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
void my_touchpad_read(lv_indev_t * indev, lv_indev_data_t * data)
{
  uint16_t touchX = 0, touchY = 0;

  bool touched = tft.getTouch(&touchX, &touchY, 600);

  if (!touched)
  {
    data->state = LV_INDEV_STATE_REL;
  }
  else
  {
    data->state = LV_INDEV_STATE_PR;

    /*Set the coordinates*/
    data->point.x = touchX;
    data->point.y = touchY;

    Serial.print("Data x ");
    Serial.println(touchX);

    Serial.print("Data y ");
    Serial.println(touchY);
  }
}

void setup()
{
  Serial.begin(115200); /* prepare for possible serial debug */

  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  Serial.println(LVGL_Arduino);
  Serial.println("I am LVGL_Arduino");


  tft.begin();        /* TFT init */
  tft.setRotation(0); /* Landscape orientation, flipped */
  /* TFT Touch Calibration Values */
  uint16_t calData[5] = {363, 3382, 297, 3319, 2};
  /* Calibrating TFT Touch */
  tft.setTouch(calData);


  lv_init();
  lv_tick_set_cb(my_get_millis);
  lv_display_t * display = lv_display_create(MY_DISP_HOR_RES, MY_DISP_VER_RES);
  lv_display_set_buffers(display, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

  /* This callback will display the rendered image */
  lv_display_set_flush_cb(display, my_disp_flush);

  lv_indev_t * indev = lv_indev_create();        /* Create input device */
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);   /* Set the device type */
  lv_indev_set_read_cb(indev, my_touchpad_read);    /* Set the read callback */

  ui_init();

  Serial.println("Setup done");
}

void loop()
{
  lv_timer_handler(); /* let the GUI do its work */
  delay(5);
}

void ui_init()
{
  ui_Inicio = lv_obj_create(NULL);
  lv_obj_clear_flag(ui_Inicio, LV_OBJ_FLAG_SCROLLABLE); /// Flags

  ui_Label2 = lv_label_create(ui_Inicio);
  lv_obj_set_width(ui_Label2, LV_SIZE_CONTENT);  
  lv_obj_set_height(ui_Label2, LV_SIZE_CONTENT);
  lv_obj_set_x(ui_Label2, 0);
  lv_obj_set_y(ui_Label2, 0);
  lv_obj_set_align(ui_Label2, LV_ALIGN_CENTER);
  lv_label_set_text(ui_Label2, "Hello :D");

  lv_disp_load_scr(ui_Inicio);
}


// /*
//  * Widgets demo — Parte 3 del ebook "Crea Interfaces Gráficas con LVGL y ESP32".
//  * Misma base que hello_world; muestra una pantalla con label, botón, switch y LED.
//  * Requiere LVGL 9, ESP32, TFT ILI9341 por SPI (TFT_eSPI).
//  */
// #include <Arduino.h>
// #include <lvgl.h>
// #include <TFT_eSPI.h>

// #define MY_DISP_HOR_RES 240
// #define MY_DISP_VER_RES 320

// lv_display_t * display = NULL;

// #define BYTES_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565))
// static uint8_t draw_buf[MY_DISP_HOR_RES * MY_DISP_VER_RES / 10 * BYTES_PER_PIXEL];

// TFT_eSPI tft = TFT_eSPI(MY_DISP_HOR_RES, MY_DISP_VER_RES);

// lv_obj_t *ui_Demo;
// lv_obj_t *ui_Titulo;
// lv_obj_t *ui_Btn;
// lv_obj_t *ui_Switch;
// lv_obj_t *ui_Led;

// void ui_demo_screen_init(void);

// static uint32_t my_get_millis(void) {
//   return millis();
// }

// void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
//   uint32_t w = (area->x2 - area->x1 + 1);
//   uint32_t h = (area->y2 - area->y1 + 1);
//   uint16_t *buf16 = (uint16_t *)px_map;
//   tft.startWrite();
//   tft.setAddrWindow(area->x1, area->y1, w, h);
//   tft.pushColors(buf16, w * h, true);
//   tft.endWrite();
//   lv_display_flush_ready(disp);
// }

// void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
//   uint16_t touchX = 0, touchY = 0;
//   bool touched = tft.getTouch(&touchX, &touchY, 600);
//   if (!touched) {
//     data->state = LV_INDEV_STATE_REL;
//   } else {
//     data->state = LV_INDEV_STATE_PR;
//     data->point.x = touchX;
//     data->point.y = touchY;
//   }
// }

// void setup() {
//   Serial.begin(115200);

//   tft.begin();
//   tft.setRotation(0);
//   uint16_t calData[5] = {363, 3382, 297, 3319, 2};
//   tft.setTouch(calData);

//   lv_init();
//   lv_tick_set_cb(my_get_millis);
//   display = lv_display_create(MY_DISP_HOR_RES, MY_DISP_VER_RES);
//   lv_display_set_buffers(display, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
//   lv_display_set_flush_cb(display, my_disp_flush);

//   lv_indev_t *indev = lv_indev_create();
//   lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
//   lv_indev_set_read_cb(indev, my_touchpad_read);

//   ui_demo_screen_init();

//   Serial.println("Widgets demo ready");
// }

// void loop() {
//   lv_timer_handler();
//   delay(5);
// }

// void ui_demo_screen_init(void) {
//   ui_Demo = lv_obj_create(NULL);

//   lv_obj_clear_flag(ui_Demo, LV_OBJ_FLAG_SCROLLABLE);

//   ui_Titulo = lv_label_create(ui_Demo);
//   lv_label_set_text(ui_Titulo, "Widgets basicos");
//   lv_obj_set_width(ui_Titulo, LV_SIZE_CONTENT);
//   lv_obj_set_height(ui_Titulo, LV_SIZE_CONTENT);
//   lv_obj_align(ui_Titulo, LV_ALIGN_TOP_MID, 0, 20);

//   ui_Btn = lv_button_create(ui_Demo);
//   lv_obj_set_size(ui_Btn, 120, 40);
//   lv_obj_align(ui_Btn, LV_ALIGN_TOP_MID, 0, 60);
//   lv_obj_t *btn_label = lv_label_create(ui_Btn);
//   lv_label_set_text(btn_label, "Pulsar");
//   lv_obj_center(btn_label);

//   ui_Switch = lv_switch_create(ui_Demo);
//   lv_obj_align(ui_Switch, LV_ALIGN_LEFT_MID, 40, 20);
//   lv_obj_set_style_bg_color(ui_Switch,  lv_color_black(), LV_PART_MAIN);

//   ui_Led = lv_led_create(ui_Demo);
//   lv_obj_set_size(ui_Led, 24, 24);
//   lv_obj_align(ui_Led, LV_ALIGN_RIGHT_MID, -40, 20);
//   lv_led_set_color(ui_Led, lv_color_hex(0x00ff00));
//   lv_led_on(ui_Led);

//   lv_disp_load_scr(ui_Demo);
// }
