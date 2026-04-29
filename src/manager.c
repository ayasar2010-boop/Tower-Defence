#include "manager.h"
#include "map.h"
#include "hud.h"
#include <string.h>

/* Ekonomi tick (4 Hz): Market binası başına sabit gelir + Outpost kaynakları */
static void TickEconomy(Game *g) {
    int income = 0;
    for (int i = 0; i < g->buildingCount; i++) {
        Building *b = &g->buildings[i];
        if (!b->active) continue;
        if (b->type == BUILDING_MARKET)
            income += 3;
    }
    /* Outpost kaynakları: belirli eşikte köylü teslim eder */
    for (int i = 0; i < g->outpostCount; i++) {
        Outpost *op = &g->outposts[i];
        if (!op->active || op->resources < 20.0f) continue;
        op->resources -= 20.0f;
        income += 10;
    }
    g->gold += income;
    g->managers.marketIncome = income;
}

/* AI tick (5 Hz): Aktif/çarpışan birim sayısını güncelle */
static void TickAI(Game *g) {
    int active = 0, engaged = 0;
    for (int i = 0; i < MAX_FRIENDLY_UNITS; i++) {
        FriendlyUnit *fu = &g->friendlyUnits[i];
        if (!fu->active) continue;
        active++;
        if (fu->engagedEnemyIdx >= 0) engaged++;
    }
    g->managers.activeUnits  = active;
    g->managers.engagedUnits = engaged;
}

void InitManagers(Managers *m) {
    memset(m, 0, sizeof(Managers));
}

void UpdateManagers(Game *g, float dt) {
    g->managers.ecoTimer += dt;
    if (g->managers.ecoTimer >= MGR_TICK_ECO) {
        g->managers.ecoTimer -= MGR_TICK_ECO;
        TickEconomy(g);
    }
    g->managers.aiTimer += dt;
    if (g->managers.aiTimer >= MGR_TICK_AI) {
        g->managers.aiTimer -= MGR_TICK_AI;
        TickAI(g);
    }
}
