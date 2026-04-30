/* loot_table.c — T96: Ağırlıklı loot tabloları + çekiliş mantığı */
#include "loot_table.h"
#include "rarity.h"
#include <string.h>

/* ============================================================
 * Statik Loot Tabloları (EnemyType başına bir tablo)
 * dropChance: dalga olaylarından bağımsız temel ihtimal
 * weight toplamı önemli değil; oransal seçim yapılır
 * ============================================================ */

static const LootTable TABLE_NORMAL = {
    0.15f, /* %15 düşürme ihtimali */
    {
        { ITEM_POTION, "Sağlık İksiri",   50.0f, RARITY_COMMON   },
        { ITEM_RUNE,   "Küçük Rün",       30.0f, RARITY_COMMON   },
        { ITEM_GEAR,   "Hasarlı Bıçak",   20.0f, RARITY_COMMON   },
    },
    3
};

static const LootTable TABLE_FAST = {
    0.22f, /* Hızlı düşmanlar biraz daha fazla düşürür */
    {
        { ITEM_POTION, "Hız İksiri",       35.0f, RARITY_COMMON   },
        { ITEM_RUNE,   "Hız Rünü",         35.0f, RARITY_UNCOMMON },
        { ITEM_GEAR,   "Hafif Zırh",       20.0f, RARITY_COMMON   },
        { ITEM_GEAR,   "Kısa Yay",         10.0f, RARITY_UNCOMMON },
    },
    4
};

static const LootTable TABLE_TANK = {
    0.40f, /* Tank ölümünde %40 şans */
    {
        { ITEM_POTION, "Güç İksiri",       20.0f, RARITY_UNCOMMON },
        { ITEM_RUNE,   "Demir Rünü",       30.0f, RARITY_UNCOMMON },
        { ITEM_GEAR,   "Ağır Zırh",        25.0f, RARITY_RARE     },
        { ITEM_GEAR,   "Savaş Baltası",    15.0f, RARITY_UNCOMMON },
        { ITEM_RUNE,   "Kırık Güç Rünü",  10.0f, RARITY_RARE     },
    },
    5
};

static const LootTable TABLE_BOSS = {
    1.00f, /* Boss her zaman loot düşürür */
    {
        { ITEM_GEAR,   "Boss Kılıcı",      20.0f, RARITY_EPIC     },
        { ITEM_GEAR,   "Boss Zırhı",       20.0f, RARITY_EPIC     },
        { ITEM_RUNE,   "Ejder Rünü",       25.0f, RARITY_RARE     },
        { ITEM_POTION, "Efsunlu İksir",    15.0f, RARITY_RARE     },
        { ITEM_GEAR,   "Komutan Kalkanı",  10.0f, RARITY_MYTHICAL },
        { ITEM_RUNE,   "Mitsel Rün",       10.0f, RARITY_MYTHICAL },
    },
    6
};

const LootTable *GetLootTable(EnemyType type) {
    switch (type) {
    case ENEMY_FAST:  return &TABLE_FAST;
    case ENEMY_TANK:  return &TABLE_TANK;
    case ENEMY_BOSS:  return &TABLE_BOSS;
    default:          return &TABLE_NORMAL;
    }
}

/* ============================================================
 * RollLootDrop — ağırlıklı çekiliş + nadirlik boyutu
 * ============================================================ */
bool RollLootDrop(const LootTable *lt, int wave, Item *out) {
    if (!lt || !out || lt->entryCount <= 0) return false;

    /* Dalga ilerledikçe artan drop bonusu (her 5 dalgada +3%) */
    float bonusChance = (float)(wave / 5) * 0.03f;
    float chance = lt->dropChance + bonusChance;
    if (chance > 1.0f) chance = 1.0f;

    /* Drop ihtimali kontrolü: 0–10000 aralığında rastgele */
    int roll = GetRandomValue(0, 9999);
    if ((float)roll / 10000.0f >= chance) return false;

    /* Ağırlık toplamı hesapla */
    float totalWeight = 0.0f;
    for (int i = 0; i < lt->entryCount; i++)
        totalWeight += lt->entries[i].weight;

    /* Ağırlıklı seçim: rastgele bir nokta seç, kümülatif ağırlık geç */
    float pick = ((float)GetRandomValue(0, 9999) / 10000.0f) * totalWeight;
    float cum  = 0.0f;
    const LootEntry *chosen = &lt->entries[lt->entryCount - 1]; /* fallback */
    for (int i = 0; i < lt->entryCount; i++) {
        cum += lt->entries[i].weight;
        if (pick <= cum) {
            chosen = &lt->entries[i];
            break;
        }
    }

    /* Nadirlik: temel min rarity + dalga bonusu üzerinden RollRarity */
    ItemRarity rar = RollRarity(wave, roll);
    if (rar < chosen->minRarity) rar = chosen->minRarity;

    memset(out, 0, sizeof(Item));
    out->type   = chosen->itemType;
    out->amount = 1;
    out->rarity = (int)rar;
    strncpy(out->name, chosen->name, 31);
    out->name[31] = '\0';

    return true;
}

/* ============================================================
 * T99 — GenerateCursedItem
 * Elite düşman öldürülünce lanetli ITEM_GEAR üretir.
 * Rarity: en az RARITY_RARE, çoğunlukla RARITY_EPIC.
 * ============================================================ */
static const char *CURSED_NAMES[] = {
    "Lanetli Kılıç",
    "Lanetten Zırh",
    "Lanetli Kask",
    "Lanetli Kolyesi",
    "Kanlı Bıçak",
    "Gece Zırhı",
};
#define CURSED_NAME_COUNT 6

bool GenerateCursedItem(EnemyType type, int wave, Item *out) {
    if (!out) return false;
    memset(out, 0, sizeof(Item));

    int idx = GetRandomValue(0, CURSED_NAME_COUNT - 1);

    /* Boss öldürülünce her zaman lanetli eşya çıkabilir; diğerleri %40 */
    if (type != ENEMY_BOSS) {
        if (GetRandomValue(0, 99) >= 40) return false;
    }

    /* Dalga ilerledikçe daha yüksek rarity */
    ItemRarity rar = RARITY_RARE;
    if (wave >= 15)      rar = RARITY_MYTHICAL;
    else if (wave >= 10) rar = RARITY_EPIC;

    out->type     = ITEM_GEAR;
    out->amount   = 1;
    out->rarity   = (int)rar;
    out->isCursed = true;
    strncpy(out->name, CURSED_NAMES[idx], 31);
    out->name[31] = '\0';

    return true;
}
