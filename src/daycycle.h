/* daycycle.h — T100: Yaşayan zaman + Gün/Gece döngüsü */
#ifndef DAYCYCLE_H
#define DAYCYCLE_H

#include "raylib.h"
#include <stdbool.h>

/* timeOfDay: 0.0=şafak · 0.25=öğle · 0.5=akşam · 0.75=gece yarısı */
typedef struct {
    float timeOfDay;   /* 0.0 – 1.0 */
    float dayDuration; /* gerçek saniye cinsinden tam döngü */
    int   dayCount;    /* tamamlanan gün sayısı */
} DayCycle;

void        InitDayCycle(DayCycle *dc, float dayDuration);
void        UpdateDayCycle(DayCycle *dc, float dt);

/* Ekran üzerine renkli yarı-saydam katman çizer (gece karartması, şafak tüllü turuncu) */
void        DrawDayNightOverlay(const DayCycle *dc);

/* Üst-merkez küçük HUD: gün sayısı + zaman dilimi + ilerleme çubuğu */
void        DrawDayCycleHUD(const DayCycle *dc);

bool        IsNight(const DayCycle *dc);
const char *GetTimeOfDayName(const DayCycle *dc);

#endif /* DAYCYCLE_H */
