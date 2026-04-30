/* quest.c — T97: Görev sistemi implementasyonu */
#include "quest.h"
#include <stdio.h>
#include <string.h>

/* ============================================================
 * Statik görev tanımları (LORE.md §8'e göre)
 * ============================================================ */
static const char *QUEST_NAMES[QUEST_TYPE_COUNT] = {
    "Kayip Kervanin Intikami",
    "Lanetli Sunak Ayini",
    "Kusursuz Savunma",
};

static const char *QUEST_DESCS[QUEST_TYPE_COUNT] = {
    "3 yikilmis kervan arabasi bul",
    "Sunak yakininda 15 dusman oldur",
    "3 dalgayi can kaybetmeden gec",
};

static const int QUEST_TARGETS[QUEST_TYPE_COUNT] = { 3, 15, 3 };

void InitQuestManager(QuestManager *qm) {
    memset(qm, 0, sizeof(QuestManager));
    for (int i = 0; i < QUEST_TYPE_COUNT; i++) {
        qm->quests[i].type        = (QuestType)i;
        qm->quests[i].state       = QUEST_ACTIVE;
        qm->quests[i].name        = QUEST_NAMES[i];
        qm->quests[i].description = QUEST_DESCS[i];
        qm->quests[i].target      = QUEST_TARGETS[i];
        qm->quests[i].progress    = 0;
    }
    qm->perfectWaveStreak = 0;
    qm->livesAtWaveStart  = STARTING_LIVES;
    qm->notifyTimer       = 0.0f;
}

bool QuestAdvance(QuestManager *qm, QuestType type, int amount) {
    if (type < 0 || type >= QUEST_TYPE_COUNT) return false;
    Quest *q = &qm->quests[type];
    if (q->state != QUEST_ACTIVE) return false;

    q->progress += amount;
    if (q->progress >= q->target) {
        q->progress = q->target;
        q->state    = QUEST_COMPLETED;
        snprintf(qm->notifyText, sizeof(qm->notifyText) - 1,
                 "GOREV TAMAM: %s", q->name);
        qm->notifyTimer = 4.0f;
        return true;
    }
    return false;
}

bool IsQuestDone(const QuestManager *qm, QuestType type) {
    if (type < 0 || type >= QUEST_TYPE_COUNT) return false;
    return qm->quests[type].state == QUEST_COMPLETED;
}

void UpdateQuestNotify(QuestManager *qm, float dt) {
    if (qm->notifyTimer > 0.0f)
        qm->notifyTimer -= dt;
}

/* ============================================================
 * DrawQuestHUD — Sağ alt köşede görev listesi
 * ============================================================ */
void DrawQuestHUD(const QuestManager *qm) {
    const int panelW = 240;
    const int rowH   = 32;
    const int pad    = 4;
    int x = SCREEN_WIDTH  - panelW - 6;
    int y = SCREEN_HEIGHT - 6 - QUEST_TYPE_COUNT * rowH;

    for (int i = 0; i < QUEST_TYPE_COUNT; i++) {
        const Quest *q = &qm->quests[i];
        int iy = y + i * rowH;

        Color bg   = (Color){0, 0, 0, 150};
        Color bar  = (q->state == QUEST_COMPLETED)
                     ? (Color){50, 220, 50, 230}
                     : (Color){255, 180, 0, 220};
        Color text = (q->state == QUEST_COMPLETED) ? GREEN : WHITE;

        DrawRectangle(x, iy, panelW, rowH - 2, bg);
        DrawRectangleLinesEx((Rectangle){(float)x, (float)iy,
                                         (float)panelW, (float)(rowH - 2)}, 1, DARKGRAY);

        /* İlerleme çubuğu (altta 4px) */
        float pct = (q->target > 0) ? (float)q->progress / (float)q->target : 0.0f;
        if (pct > 1.0f) pct = 1.0f;
        int barW = (int)((panelW - 2) * pct);
        DrawRectangle(x + 1, iy + rowH - 6, barW, 4, bar);

        /* İsim ve sayaç */
        char buf[64];
        snprintf(buf, sizeof(buf) - 1, "%s  %d/%d",
                 q->name, q->progress, q->target);
        DrawText(buf, x + pad, iy + pad, 10, text);
    }

    /* Tamamlanma bildirimi */
    if (qm->notifyTimer > 0.0f) {
        float alpha = (qm->notifyTimer > 1.0f) ? 1.0f : qm->notifyTimer;
        Color nc    = (Color){255, 220, 50, (unsigned char)(alpha * 255.0f)};
        int tw = MeasureText(qm->notifyText, 20);
        DrawText(qm->notifyText, SCREEN_WIDTH / 2 - tw / 2,
                 SCREEN_HEIGHT / 2 - 80, 20, nc);
    }
}
