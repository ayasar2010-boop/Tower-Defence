/* daycycle.c — T100: Yaşayan zaman + Gün/Gece döngüsü */
#include "daycycle.h"
#include "types.h"
#include <math.h>
#include <stdio.h>

/* ============================================================
 * Anahtar renk çerçeveleri — timeOfDay'e göre ekran overlay
 * timeOfDay: [şafak=0.00, gündüz=0.15, alacakaranlık=0.45,
 *             gece başlangıcı=0.60, gece yarısı=0.82, döngü=1.00]
 * ============================================================ */
typedef struct { float t; Color c; } KeyFrame;

static const KeyFrame KEYS[] = {
    {0.00f, {255, 140,  50,  70}},  /* şafak — sıcak turuncu parıltı */
    {0.15f, {255, 220, 100,   0}},  /* gündüz  — şeffaf              */
    {0.45f, {180,  60,  20,  55}},  /* alacakaranlık — kırmızı-turuncu */
    {0.60f, { 20,  20,  80, 130}},  /* gece — koyu mavi              */
    {0.82f, { 10,  10,  55, 155}},  /* gece yarısı — en karanlık     */
    {1.00f, {255, 140,  50,  70}},  /* şafak (döngü başı)            */
};
#define KEY_COUNT 6

static Color LerpColor(Color a, Color b, float t) {
    return (Color){
        (unsigned char)((float)a.r + ((float)b.r - (float)a.r) * t),
        (unsigned char)((float)a.g + ((float)b.g - (float)a.g) * t),
        (unsigned char)((float)a.b + ((float)b.b - (float)a.b) * t),
        (unsigned char)((float)a.a + ((float)b.a - (float)a.a) * t),
    };
}

static Color GetOverlayColor(float tod) {
    for (int i = 0; i < KEY_COUNT - 1; i++) {
        if (tod >= KEYS[i].t && tod < KEYS[i + 1].t) {
            float span  = KEYS[i + 1].t - KEYS[i].t;
            float local = (span > 0.0f) ? (tod - KEYS[i].t) / span : 0.0f;
            return LerpColor(KEYS[i].c, KEYS[i + 1].c, local);
        }
    }
    return KEYS[0].c;
}

/* ============================================================
 * Init / Update
 * ============================================================ */
void InitDayCycle(DayCycle *dc, float dayDuration) {
    dc->timeOfDay   = 0.18f; /* gündüz ortasında başla */
    dc->dayDuration = (dayDuration > 0.0f) ? dayDuration : 120.0f;
    dc->dayCount    = 1;
}

void UpdateDayCycle(DayCycle *dc, float dt) {
    dc->timeOfDay += dt / dc->dayDuration;
    if (dc->timeOfDay >= 1.0f) {
        dc->timeOfDay -= 1.0f;
        dc->dayCount++;
    }
}

/* ============================================================
 * DrawDayNightOverlay
 * ============================================================ */
void DrawDayNightOverlay(const DayCycle *dc) {
    Color ov = GetOverlayColor(dc->timeOfDay);
    if (ov.a == 0) return;
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ov);
}

/* ============================================================
 * IsNight / GetTimeOfDayName
 * ============================================================ */
bool IsNight(const DayCycle *dc) {
    return dc->timeOfDay >= 0.55f && dc->timeOfDay < 0.97f;
}

const char *GetTimeOfDayName(const DayCycle *dc) {
    float t = dc->timeOfDay;
    if (t < 0.10f) return "Safak";
    if (t < 0.45f) return "Gunduz";
    if (t < 0.60f) return "Alacakaranlik";
    if (t < 0.82f) return "Gece";
    if (t < 0.97f) return "Gece Yarisi";
    return "Safak Oncesi";
}

/* ============================================================
 * DrawDayCycleHUD — üst-merkez küçük gösterge
 * ============================================================ */
void DrawDayCycleHUD(const DayCycle *dc) {
    const int BAR_W = 128, BAR_H = 7;
    int bx = SCREEN_WIDTH / 2 - BAR_W / 2;
    int by = 4;

    /* Arka panel */
    DrawRectangle(bx - 4, by - 2, BAR_W + 8, BAR_H + 16, (Color){0, 0, 0, 150});

    /* Zaman ilerleme çubuğu */
    DrawRectangle(bx, by, BAR_W, BAR_H, (Color){30, 30, 50, 220});
    int fillW = (int)(dc->timeOfDay * BAR_W);
    Color fillC = IsNight(dc) ? (Color){80, 90, 200, 255} : (Color){250, 200, 50, 255};
    if (fillW > 0)
        DrawRectangle(bx, by, fillW, BAR_H, fillC);
    DrawRectangleLines(bx, by, BAR_W, BAR_H, (Color){160, 160, 160, 140});

    /* "Gün N · Zaman" etiketi */
    char label[40];
    snprintf(label, sizeof(label), "Gun %d  %s", dc->dayCount, GetTimeOfDayName(dc));
    int tw = MeasureText(label, 9);
    DrawText(label, SCREEN_WIDTH / 2 - tw / 2, by + BAR_H + 2, 9, (Color){210, 210, 210, 200});
}
