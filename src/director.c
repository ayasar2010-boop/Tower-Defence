/* director.c — T98: Director AI performans izleme + elit düşman kararları */
#include "director.h"
#include <math.h>
#include <stdio.h>

/* ============================================================
 * Sabitler
 * ============================================================ */
#define DIR_ELITE_THRESHOLD   0.72f  /* Bu puan üstünde elite başlar          */
#define DIR_ELITE_MAX_CHANCE  0.45f  /* En fazla %45 elite spawn şansı        */
#define DIR_MAX_CLEAR_SEC     90.0f  /* Bu süreden uzun = düşük zaman skoru   */
#define DIR_EMA_ALPHA         0.35f  /* Üstel hareketli ortalama ağırlığı     */
#define DIR_FLAWLESS_NEEDED   2      /* Elite açılması için gereken mükemmel  */

/* ============================================================
 * InitDirector
 * ============================================================ */
void InitDirector(DirectorAI *d, int startingLives) {
    d->performanceScore    = 0.5f;
    d->eliteChance         = 0.0f;
    d->consecutiveFlawless = 0;
    d->eliteModeActive     = false;
    d->waveStartTime       = 0.0f;
    d->livesAtWaveStart    = startingLives;
}

/* ============================================================
 * DirectorOnWaveStart — Dalga başında snapshot
 * ============================================================ */
void DirectorOnWaveStart(DirectorAI *d, int currentLives) {
    d->waveStartTime    = (float)GetTime();
    d->livesAtWaveStart = currentLives;
}

/* ============================================================
 * DirectorOnWaveClear — Performans skoru hesapla ve elite güncelle
 * ============================================================ */
void DirectorOnWaveClear(DirectorAI *d, int currentLives) {
    float clearTime = (float)GetTime() - d->waveStartTime;
    if (clearTime < 0.1f) clearTime = 0.1f;

    /* Can skoru: kaç can korundu */
    int livesMax = (d->livesAtWaveStart > 0) ? d->livesAtWaveStart : STARTING_LIVES;
    float livesScore = (float)currentLives / (float)livesMax;
    if (livesScore > 1.0f) livesScore = 1.0f;

    /* Zaman skoru: ne kadar hızlı temizlendi (90s referans) */
    float timeScore = 1.0f - (clearTime / DIR_MAX_CLEAR_SEC);
    if (timeScore < 0.0f) timeScore = 0.0f;
    if (timeScore > 1.0f) timeScore = 1.0f;

    float newScore = 0.6f * livesScore + 0.4f * timeScore;

    /* Üstel hareketli ortalama */
    d->performanceScore = (1.0f - DIR_EMA_ALPHA) * d->performanceScore
                        + DIR_EMA_ALPHA * newScore;

    /* Flawless streak (can kaybetmeden temizlendi mi) */
    if (currentLives >= d->livesAtWaveStart)
        d->consecutiveFlawless++;
    else
        d->consecutiveFlawless = 0;

    /* Elite mod kararı */
    if (d->performanceScore >= DIR_ELITE_THRESHOLD
        && d->consecutiveFlawless >= DIR_FLAWLESS_NEEDED) {
        d->eliteModeActive = true;
        /* Elite şansı: puan eşiği üzerindeki fazla × 2.0 → max %45 */
        float excess = d->performanceScore - DIR_ELITE_THRESHOLD;
        d->eliteChance = excess * 2.0f;
        if (d->eliteChance > DIR_ELITE_MAX_CHANCE)
            d->eliteChance = DIR_ELITE_MAX_CHANCE;
    } else {
        d->eliteModeActive = false;
        /* Yavaşça azalt — çok iyi oynamasa da birdenbire sıfırlanmasın */
        d->eliteChance *= 0.6f;
    }
}

/* ============================================================
 * DirectorApplyElite — SpawnEnemy sonrası elite mod uygula
 * ============================================================ */
void DirectorApplyElite(DirectorAI *d, Enemy *e) {
    if (!d->eliteModeActive || e->isBoss) return;
    int roll = GetRandomValue(0, 9999);
    if ((float)roll / 10000.0f >= d->eliteChance) return;

    e->isElite   = true;
    e->maxHp    *= 1.6f;
    e->currentHp = e->maxHp;
    e->speed    *= 1.25f;
    e->radius   *= 1.15f;
}

/* ============================================================
 * DrawDirectorStatus — HUD'da küçük durum göstergesi
 * ============================================================ */
void DrawDirectorStatus(const DirectorAI *d) {
    if (!d->eliteModeActive) return;

    char buf[48];
    snprintf(buf, sizeof(buf) - 1,
             "DIRECTOR: ELIT %%%.0f", d->eliteChance * 100.0f);
    int tw = MeasureText(buf, 11);
    int px = SCREEN_WIDTH / 2 - tw / 2;
    int py = 8;
    DrawRectangle(px - 4, py - 2, tw + 8, 16, (Color){0, 0, 0, 160});
    DrawText(buf, px, py, 11, (Color){255, 200, 50, 230});
}
