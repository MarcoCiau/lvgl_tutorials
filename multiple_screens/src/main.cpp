#include <Arduino.h>

#include <lvgl.h>
#include <TFT_eSPI.h>

// Define UI Screens-views
lv_obj_t *home_screen;
lv_obj_t *settings_screen;

//Define UI Widgets
lv_obj_t * setpoint_label;
// Variables for the thermostat
int current_temp = 22;
int target_temp = 24;

// Local functions to create screens-views
void ui_create_home_screen();
void ui_create_settings_screen();

// Event Handlers
void settigns_btn_event_handler(lv_event_t * e);
void home_btn_event_handler(lv_event_t * e);
void slider_event_handler(lv_event_t * e);

void ui_init(void);
/*Change to your screen resolution*/
#define SCREEN_ROTATION 0 // set the screen rotation

/*Change to your screen resolution*/
#if (SCREEN_ROTATION == 1) || (SCREEN_ROTATION == 3)
static const uint16_t screenWidth = 320; // rotation 1 or 3
static const uint16_t screenHeight = 240;
#else
static const uint16_t screenWidth = 240; // rotation 0 or 2
static const uint16_t screenHeight = 320;
#endif

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char *buf)
{
  Serial.printf(buf);
  Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)&color_p->full, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
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
  Serial.println("I am LVGL_Arduino - MCiauTech");

  lv_init();

#if LV_USE_LOG != 0
  lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

  tft.begin();        /* TFT init */
  tft.setRotation(0); /* Landscape orientation, flipped */
  /* TFT Touch Calibration Values */
  uint16_t calData[5] = {367, 3320, 583, 3264, 6};

  /* Calibrating TFT Touch */
  tft.setTouch(calData);

  lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * screenHeight / 10);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);
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
  //setting theme
  lv_disp_t *dispp = lv_disp_get_default();
  lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
  lv_disp_set_theme(dispp, theme);
  //create home screen
  ui_create_home_screen();
  //create settings screen
  ui_create_settings_screen();
  // load (show) home screen
  lv_disp_load_scr(home_screen);
}

void ui_create_home_screen()
{
  home_screen = lv_obj_create(NULL);

  lv_obj_t *temp_label = lv_label_create(home_screen);
  lv_label_set_text_fmt(temp_label, "Current Temp: %d C", current_temp);
  lv_obj_align(temp_label, LV_ALIGN_CENTER, 0, -50);

  lv_obj_t *settings_btn = lv_btn_create(home_screen);
  lv_obj_align(settings_btn, LV_ALIGN_CENTER, 0, 50);
  lv_obj_t *settings_btn_label = lv_label_create(settings_btn);
  lv_label_set_text(settings_btn_label, "Settings");
  lv_obj_add_event_cb(settings_btn, settigns_btn_event_handler, LV_EVENT_CLICKED, NULL);
}

void ui_create_settings_screen()
{
  settings_screen = lv_obj_create(NULL);

  setpoint_label = lv_label_create(settings_screen);
  lv_label_set_text_fmt(setpoint_label, "Set point: %d C", target_temp);
  lv_obj_align(setpoint_label, LV_ALIGN_CENTER, 0, -50);
  
  lv_obj_t * slider = lv_slider_create(settings_screen);
  lv_slider_set_range(slider, 16, 30);
  lv_slider_set_value(slider, target_temp, LV_ANIM_OFF);
  lv_obj_align(slider, LV_ALIGN_CENTER, 0, 50);
  lv_obj_set_width(slider, 180);
  lv_obj_add_event_cb(slider, slider_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

  lv_obj_t *home_btn = lv_btn_create(settings_screen);
  lv_obj_align(home_btn, LV_ALIGN_CENTER, 0, 120);
  lv_obj_t *home_btn_label = lv_label_create(home_btn);
  lv_label_set_text(home_btn_label, "Home");
  lv_obj_add_event_cb(home_btn, home_btn_event_handler, LV_EVENT_CLICKED, NULL);
}

// Button event to switch settings screen
void settigns_btn_event_handler(lv_event_t * e) {
  lv_scr_load(settings_screen);
}

// Button event to switch home screen
void home_btn_event_handler(lv_event_t * e) {
  lv_scr_load(home_screen);
}

// Slider event to update target temperature
void slider_event_handler(lv_event_t * e) {
  lv_obj_t * slider = lv_event_get_target(e);
  target_temp = lv_slider_get_value(slider);
  lv_label_set_text_fmt(setpoint_label, "Set point: %d C", target_temp);
}
