#include "guardian.h"
#include "hud.h"
#include "map.h"
#include "util.h"
#include <math.h>
#include <string.h>

/* Sabit muhafız spawn noktaları: haritada kaynak değerli bölgeler */
static const struct { float x, y; int level; } SPAWN_POINTS[] = {
    { 480.0f,  320.0f, 1 },   /* batı ormanı girişi */
    { 240.0f,  560.0f, 1 },   /* güney-batı koru */
    { 960.0f,  640.0f, 2 },   /* doğu ormanı */
    { 680.0f,  180.0f, 2 },   /* kuzey kırası */
    { 1120.0f, 280.0f, 3 },   /* kuzey-doğu plato */
    { 380.0f,  740.0f, 2 },   /* güney ova */
    { 820.0f,  500.0f, 1 },   /* nehir kıyısı */
    { 1200.0f, 560.0f, 3 },   /* uzak doğu */
};

void InitGuardians(Game *g) {
    g->guardianCount = 0;
    int n = (int)(sizeof(SPAWN_POINTS) / sizeof(SPAWN_POINTS[0]));
    if (n > MAX_GUARDIANS) n = MAX_GUARDIANS;
    for (int i = 0; i < n; i++) {
        ResourceGuardian *gr = &g->guardians[i];
        memset(gr, 0, sizeof(ResourceGuardian));
        gr->patrolCenter.x = SPAWN_POINTS[i].x;
        gr->patrolCenter.y = SPAWN_POINTS[i].y;
        gr->position       = gr->patrolCenter;
        gr->patrolAngle    = (float)i * 0.78f;
        gr->level          = SPAWN_POINTS[i].level;
        switch (gr->level) {
        case 3: gr->maxHp = 250.0f; gr->attackDamage = 20.0f; gr->lootGold = 50; gr->radius = 11.0f; break;
        case 2: gr->maxHp = 120.0f; gr->attackDamage = 12.0f; gr->lootGold = 25; gr->radius = 9.0f;  break;
        default:gr->maxHp =  50.0f; gr->attackDamage =  6.0f; gr->lootGold = 10; gr->radius = 7.0f;  break;
        }
        gr->hp          = gr->maxHp;
        gr->attackRange = 22.0f;
        gr->active      = true;
        g->guardianCount++;
    }
}

void DamageGuardian(Game *g, int idx, float dmg) {
    if (idx < 0 || idx >= g->guardianCount) return;
    ResourceGuardian *gr = &g->guardians[idx];
    if (!gr->active) return;
    gr->hp -= dmg;
    if (gr->hp <= 0.0f) {
        g->gold += gr->lootGold;
        SpawnFloatingText(g, gr->position, (float)gr->lootGold, false, false);
        gr->active = false;
    }
}

void UpdateGuardians(Game *g, float dt) {
    for (int i = 0; i < g->guardianCount; i++) {
        ResourceGuardian *gr = &g->guardians[i];
        if (!gr->active) continue;

        /* Aggro: en yakın friendly unit'i bul */
        gr->aggro = false;
        int targetFU = -1;
        float nearest = GUARDIAN_AGGRO_RANGE;
        for (int j = 0; j < MAX_FRIENDLY_UNITS; j++) {
            if (!g->friendlyUnits[j].active) continue;
            float d = Vec2Distance(gr->position, g->friendlyUnits[j].position);
            if (d < nearest) { nearest = d; targetFU = j; }
        }

        if (targetFU >= 0) {
            gr->aggro = true;
            /* Yaklaş */
            Vector2 dir = Vec2Normalize(Vec2Subtract(g->friendlyUnits[targetFU].position,
                                                     gr->position));
            gr->position = Vec2Add(gr->position, Vec2Scale(dir, 40.0f * dt));
            /* Saldır */
            gr->attackCooldown -= dt;
            if (gr->attackCooldown <= 0.0f && nearest < gr->attackRange) {
                g->friendlyUnits[targetFU].hp -= gr->attackDamage;
                gr->attackCooldown = 2.0f;
                if (g->friendlyUnits[targetFU].hp <= 0.0f)
                    g->friendlyUnits[targetFU].active = false;
            }
        } else {
            /* Patrol: merkez etrafında döngü */
            gr->patrolAngle += 0.35f * dt;
            gr->position.x = gr->patrolCenter.x + cosf(gr->patrolAngle) * 28.0f;
            gr->position.y = gr->patrolCenter.y + sinf(gr->patrolAngle) * 14.0f;
        }

        /* Friendly unit saldırı geri dönüşü: guardian'a yakın savaşçılar hasar verir */
        for (int j = 0; j < MAX_FRIENDLY_UNITS; j++) {
            FriendlyUnit *fu = &g->friendlyUnits[j];
            if (!fu->active || fu->type == FUNIT_VILLAGER) continue;
            float d = Vec2Distance(gr->position, fu->position);
            if (d < gr->attackRange + fu->attackRange) {
                fu->attackCooldown -= dt;
                if (fu->attackCooldown <= 0.0f) {
                    DamageGuardian(g, i, fu->atk);
                    fu->attackCooldown = fu->attackSpeed;
                    if (!gr->active) break;
                }
            }
        }
    }
}

void DrawGuardians(Game *g) {
    for (int i = 0; i < g->guardianCount; i++) {
        ResourceGuardian *gr = &g->guardians[i];
        if (!gr->active) continue;

        /* Fog kontrolü */
        {
            int gc2, gr2;
            if (WorldToGrid(gr->position, &gc2, &gr2) && !g->fogVisible[gr2][gc2])
                continue;
        }

        /* Renk: seviyeye göre */
        Color body, outline;
        if (gr->level >= 3)      { body = (Color){160, 60, 200, 220}; outline = (Color){220, 100, 255, 255}; }
        else if (gr->level == 2) { body = (Color){200, 100, 30,  220}; outline = (Color){255, 160, 60,  255}; }
        else                     { body = (Color){80,  140, 60,  220}; outline = (Color){130, 200, 90,  255}; }

        /* Aggro durumunda nabız efekti */
        float r = gr->radius;
        if (gr->aggro) r *= 1.0f + sinf((float)GetTime() * 8.0f) * 0.12f;

        DrawCircleV(gr->position, r, body);
        DrawCircleLines((int)gr->position.x, (int)gr->position.y, r, outline);
        /* Seviye işareti */
        char lvl[4];
        lvl[0] = (char)('0' + gr->level);
        lvl[1] = '\0';
        DrawText(lvl, (int)gr->position.x - 3, (int)gr->position.y - 5, 10, WHITE);

        /* HP bar */
        if (gr->hp < gr->maxHp) {
            float bw = gr->radius * 2.8f;
            float bx = gr->position.x - bw * 0.5f;
            float by = gr->position.y - gr->radius - 7.0f;
            DrawRectangle((int)bx, (int)by, (int)bw, 4, (Color){60, 0, 0, 180});
            DrawRectangle((int)bx, (int)by, (int)(bw * gr->hp / gr->maxHp), 4,
                          (Color){200, 60, 20, 220});
        }
    }
}
