#include <Arduino.h>

#include <lvgl.h>
#include <TFT_eSPI.h>

// lvgl ui widgets
lv_obj_t *home_screen;
lv_obj_t *time_label;
lv_obj_t *btn_start_pause;
lv_obj_t *btn_start_pause_label;
lv_obj_t *btn_stop;
lv_obj_t *btn_stop_label;

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
// TFT_eSPI tft = TFT_eSPI(screenHeight,screenWidth );

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

// App function prototypes and variables
bool running = false;
bool paused = false;
unsigned long startTime = 0;
unsigned long currentTime = 0;
void timer_start();
void timer_stop();
void timer_pause();
void timer_loop();
// UI Event Handlers
static void start_pause_event_handler(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED)
  {
    Serial.println("start-pause event");

    if (!running)
    {
      timer_start();
      lv_label_set_text(btn_start_pause_label, "Pause");
    }
    else if (!paused)
    {
      timer_pause();
      lv_label_set_text(btn_start_pause_label, "Start");
    }
  }
}

static void stop_event_handler(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED)
  {
    Serial.println("stop event");
    timer_stop();
    lv_label_set_text(time_label, "00:00:00");
    lv_label_set_text(btn_start_pause_label, "Start");
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

  tft.begin();                      /* TFT init */
  tft.setRotation(SCREEN_ROTATION); /* Landscape orientation, flipped */
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
  timer_loop();
  delay(10);
}

void ui_init()
{
  // configure LVGL UI Theme
  lv_disp_t *dispp = lv_disp_get_default();
  lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
  lv_disp_set_theme(dispp, theme);
  home_screen = lv_obj_create(NULL);
  lv_obj_clear_flag(home_screen, LV_OBJ_FLAG_SCROLLABLE); /// Flags
  // Create Time Label
  time_label = lv_label_create(home_screen);
  lv_obj_set_width(time_label, LV_SIZE_CONTENT);
  lv_obj_set_height(time_label, LV_SIZE_CONTENT);
  lv_obj_set_x(time_label, 0);
  lv_obj_set_y(time_label, 0);
  lv_obj_set_align(time_label, LV_ALIGN_CENTER);
  lv_label_set_text(time_label, "00:00:00");
  lv_obj_set_style_text_font(time_label, &lv_font_montserrat_36, LV_PART_MAIN | LV_STATE_DEFAULT);
  // Create Start/Pause button
  btn_start_pause = lv_btn_create(home_screen);
  lv_obj_set_size(btn_start_pause, 80, 50);
  lv_obj_align(btn_start_pause, LV_ALIGN_CENTER, -60, 100);
  btn_start_pause_label = lv_label_create(btn_start_pause);
  lv_obj_align(btn_start_pause_label, LV_ALIGN_CENTER, 0, 0);
  lv_label_set_text(btn_start_pause_label, "Start");
  // Create Stop button
  btn_stop = lv_btn_create(home_screen);
  lv_obj_set_size(btn_stop, 80, 50);
  lv_obj_align(btn_stop, LV_ALIGN_CENTER, 60, 100);
  btn_stop_label = lv_label_create(btn_stop);
  lv_obj_align(btn_stop_label, LV_ALIGN_CENTER, 0, 0);
  lv_label_set_text(btn_stop_label, "Stop");
  // create-attach btns event handlers
  lv_obj_add_event_cb(btn_start_pause, start_pause_event_handler, LV_EVENT_ALL, NULL);
  lv_obj_add_event_cb(btn_stop, stop_event_handler, LV_EVENT_ALL, NULL);
  // load home screen
  lv_disp_load_scr(home_screen);
}

void timer_start()
{
  running = true;
  paused = false;
  startTime = millis() - currentTime;
}

void timer_stop()
{
  running = false;
  paused = false;
  startTime = 0;
  currentTime = 0;
}

void timer_pause()
{
  paused = true;
  running = false;
  currentTime = millis() - startTime;
}

void timer_loop()
{
  if (running)
  {
    static uint32_t update_timer = 0;
    currentTime = millis() - startTime;
    int hours = currentTime / 3600000;
    int minutes = (currentTime % 3600000) / 60000;
    int seconds = (currentTime % 60000) / 1000;

    char timeStr[9];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", hours, minutes, seconds);
    Serial.printf("%02d:%02d:%02d\n", hours, minutes, seconds);
    lv_label_set_text(time_label, timeStr);
  }
}