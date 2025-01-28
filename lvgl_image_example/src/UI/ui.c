#include "ui.h"
#include "ui_helpers.h"
int goto_widget_flag=0;
int zero_clean=0;
int bar_flag=0;
///////////////////// VARIABLES ////////////////////

lv_obj_t * ui_BEGIN;
lv_obj_t * home_img_demo;
lv_obj_t * hello_label;
///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////
static void anim_x_cb(void * var, int32_t v)
{
    lv_obj_set_x(var, v);
}
static void anim_y_cb(void * var, int32_t v)
{
    lv_obj_set_y(var, v);
}

///////////////////// FUNCTIONS ////////////////////
void show_label_cb(lv_timer_t * timer) {
    // Create and show the label after the delay
    hello_label = lv_label_create(ui_BEGIN);
    lv_obj_set_width(hello_label, LV_SIZE_CONTENT);
    lv_obj_set_height(hello_label, LV_SIZE_CONTENT); 
    lv_obj_set_x(hello_label, 0);
    lv_obj_set_y(hello_label, 100);
    lv_obj_set_align(hello_label, LV_ALIGN_CENTER);
    lv_label_set_text(hello_label, "LIKE AND SUSCRIBE :D");
    lv_obj_set_style_text_color(hello_label, lv_color_hex(0x09BEFB), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(hello_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(hello_label, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
}

///////////////////// SCREENS ////////////////////
void ui_BEGIN_screen_init(void)
{
    ui_BEGIN = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_BEGIN, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_BEGIN, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BEGIN, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    home_img_demo = lv_img_create(ui_BEGIN);
    lv_img_set_src(home_img_demo, &mciautech_logo);
    lv_obj_set_width(home_img_demo, LV_SIZE_CONTENT);   
    lv_obj_set_height(home_img_demo, LV_SIZE_CONTENT);    
    lv_obj_set_size(home_img_demo, 158, 112);
    lv_obj_set_pos(home_img_demo, 320, 0);
    lv_obj_add_flag(home_img_demo, LV_OBJ_FLAG_ADV_HITTEST);    
    lv_obj_clear_flag(home_img_demo, LV_OBJ_FLAG_SCROLLABLE);     

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, home_img_demo);
    lv_anim_set_values(&a, 0, 190);
    lv_anim_set_time(&a, 1000);
    lv_anim_set_exec_cb(&a, anim_y_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_start(&a);

    // Set a timer to show the label after a delay of 2000 milliseconds (2 seconds)
    lv_timer_t * timer = lv_timer_create(show_label_cb, 2000, NULL);
}



void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_BEGIN_screen_init();
    lv_disp_load_scr(ui_BEGIN);
}
