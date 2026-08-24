// main/demo_leader.c —— 像素警察领导小游戏。
//
// 这是一个完全虚构的卡通角色：上键触发“攻击”反应，下键触发“巴掌”
// 反应，确定键短按复位。动作只作用于屏幕上的像素角色，不对应现实人物。
#include "demo.h"
#include "bsp_audio.h"
#include "bsp_display.h"
#include "ui_pixel.h"
#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define FRAME_WIDTH  96
#define FRAME_HEIGHT 180
#define FRAME_STRIDE (FRAME_WIDTH * 2)
#define FRAME_BYTES  (FRAME_STRIDE * FRAME_HEIGHT)
#define SAMPLE_RATE 16000
#define AUDIO_CHUNK 512

extern const uint8_t s_leader_frame_0[] asm("_binary_leader_frame_0_rgb565_start");
extern const uint8_t s_leader_frame_1[] asm("_binary_leader_frame_1_rgb565_start");
extern const uint8_t s_leader_frame_2[] asm("_binary_leader_frame_2_rgb565_start");
extern const uint8_t s_leader_frame_3[] asm("_binary_leader_frame_3_rgb565_start");
extern const uint8_t s_leader_apology_start[] asm("_binary_leader_apology_pcm_start");
extern const uint8_t s_leader_apology_end[] asm("_binary_leader_apology_pcm_end");

static const lv_image_dsc_t s_frames[] = {
    { .header.magic = LV_IMAGE_HEADER_MAGIC, .header.cf = LV_COLOR_FORMAT_RGB565,
      .header.flags = 0, .header.w = FRAME_WIDTH, .header.h = FRAME_HEIGHT,
      .header.stride = FRAME_STRIDE, .data_size = FRAME_BYTES, .data = s_leader_frame_0 },
    { .header.magic = LV_IMAGE_HEADER_MAGIC, .header.cf = LV_COLOR_FORMAT_RGB565,
      .header.flags = 0, .header.w = FRAME_WIDTH, .header.h = FRAME_HEIGHT,
      .header.stride = FRAME_STRIDE, .data_size = FRAME_BYTES, .data = s_leader_frame_1 },
    { .header.magic = LV_IMAGE_HEADER_MAGIC, .header.cf = LV_COLOR_FORMAT_RGB565,
      .header.flags = 0, .header.w = FRAME_WIDTH, .header.h = FRAME_HEIGHT,
      .header.stride = FRAME_STRIDE, .data_size = FRAME_BYTES, .data = s_leader_frame_2 },
    { .header.magic = LV_IMAGE_HEADER_MAGIC, .header.cf = LV_COLOR_FORMAT_RGB565,
      .header.flags = 0, .header.w = FRAME_WIDTH, .header.h = FRAME_HEIGHT,
      .header.stride = FRAME_STRIDE, .data_size = FRAME_BYTES, .data = s_leader_frame_3 },
};

static lv_obj_t *s_scr;
static lv_obj_t *s_image;
static lv_obj_t *s_status;
static lv_obj_t *s_hint;
static lv_timer_t *s_anim_timer;
static TaskHandle_t s_audio_task;
static volatile bool s_audio_requested;
static uint8_t s_anim_phase;

static void set_frame(size_t frame) {
    lv_image_set_src(s_image, &s_frames[frame]);
}

static void set_status(const char *status, const char *hint) {
    if (s_status) lv_label_set_text(s_status, status);
    if (s_hint) lv_label_set_text(s_hint, hint);
}

static void image_x(void *obj, int32_t value) {
    lv_obj_set_x((lv_obj_t *)obj, value);
}

static void shake_image(void) {
    lv_anim_delete(s_image, image_x);
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, s_image);
    lv_anim_set_exec_cb(&anim, image_x);
    lv_anim_set_values(&anim, 72, 77);
    lv_anim_set_duration(&anim, 55);
    lv_anim_set_playback_duration(&anim, 95);
    lv_anim_set_playback_delay(&anim, 35);
    lv_anim_set_repeat_count(&anim, 1);
    lv_anim_set_path_cb(&anim, lv_anim_path_step);
    lv_anim_start(&anim);
}

static void animation_tick(lv_timer_t *timer) {
    (void)timer;
    if (s_anim_phase < 4) {
        s_anim_phase++;
        if (s_anim_phase == 3) set_frame(3);
    }
}

static void play_apology(void) {
    if (bsp_audio_set_format(SAMPLE_RATE, 16, 1) != ESP_OK) return;
    bsp_audio_set_volume(82);
    const uint8_t *cursor = s_leader_apology_start;
    while (cursor < s_leader_apology_end) {
        size_t count = (size_t)(s_leader_apology_end - cursor);
        if (count > AUDIO_CHUNK * sizeof(int16_t)) count = AUDIO_CHUNK * sizeof(int16_t);
        bsp_audio_write(cursor, count);
        cursor += count;
    }
    bsp_audio_stop();
}

static void audio_task(void *arg) {
    (void)arg;
    for (;;) {
        if (s_audio_requested) {
            s_audio_requested = false;
            play_apology();
        } else {
            vTaskDelay(pdMS_TO_TICKS(25));
        }
    }
}

static void choose_action(bool slap) {
    s_anim_phase = 0;
    set_frame(slap ? 2 : 1);
    set_status(slap ? "SLAP!" : "ATTACK!", "VOICE: WO CUO LE");
    shake_image();
    s_audio_requested = true;
}

void demo_leader_enter(void) {
    s_scr = ui_pixel_screen_create("LEADER");

    s_status = ui_pixel_label(s_scr, "READY", &lv_font_montserrat_20, UI_INK);
    lv_obj_set_width(s_status, 224);
    lv_obj_set_pos(s_status, 8, 48);
    lv_obj_set_style_text_align(s_status, LV_TEXT_ALIGN_CENTER, 0);

    s_image = lv_image_create(s_scr);
    lv_obj_set_pos(s_image, 72, 78);
    lv_image_set_src(s_image, &s_frames[0]);

    lv_obj_t *panel = ui_pixel_panel_create(s_scr, 18, 261, 204, 25, UI_PAPER);
    s_hint = lv_label_create(panel);
    lv_obj_set_width(s_hint, 188);
    lv_obj_set_style_text_font(s_hint, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(s_hint, lv_color_hex(UI_INK), 0);
    lv_obj_set_style_text_align(s_hint, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_center(s_hint);
    lv_label_set_text(s_hint, "UP ATTACK   DOWN SLAP");

    lv_obj_t *footer = ui_pixel_label(s_scr, "OK RESET   LONG OK MENU",
                                      &lv_font_montserrat_14, UI_INK);
    lv_obj_set_pos(footer, 38, 291);

    s_anim_phase = 4;
    s_audio_requested = false;
    if (!s_audio_task) xTaskCreate(audio_task, "leader_audio", 3072, NULL, 4, &s_audio_task);
    if (!s_anim_timer) s_anim_timer = lv_timer_create(animation_tick, 90, NULL);
    lv_screen_load(s_scr);
}

void demo_leader_exit(void) {
    s_audio_requested = false;
    if (s_audio_task) { vTaskDelete(s_audio_task); s_audio_task = NULL; }
    bsp_audio_stop();
    if (s_anim_timer) { lv_timer_delete(s_anim_timer); s_anim_timer = NULL; }
    if (s_scr) { lv_obj_delete(s_scr); s_scr = NULL; }
    s_image = s_status = s_hint = NULL;
}

void demo_leader_key(bsp_btn_t btn, bsp_btn_ev_t ev) {
    if (ev != BSP_BTN_CLICK) return;
    if (btn == BSP_BTN_UP) choose_action(false);
    else if (btn == BSP_BTN_DOWN) choose_action(true);
    else if (btn == BSP_BTN_OK) {
        s_anim_phase = 4;
        set_frame(3);
        set_status("READY", "UP ATTACK   DOWN SLAP");
    }
}
