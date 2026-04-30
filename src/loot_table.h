/* loot_table.h — T96: Her düşman tipi için ağırlıklı loot tabloları */
#ifndef LOOT_TABLE_H
#define LOOT_TABLE_H

#include "types.h"
#include "dungeon.h"

/* Tek bir loot olasılığı: tip, isim, ağırlık, min nadirlik */
typedef struct {
    ItemType    itemType;
    const char *name;
    float       weight;     /* Ağırlıklı seçimde göreli pay */
    ItemRarity  minRarity;  /* Bu girdiden en düşük nadirlik */
} LootEntry;

/* Bir düşman tipinin loot tablosu */
typedef struct {
    float      dropChance;              /* 0.0–1.0: bir şey düşürme ihtimali */
    LootEntry  entries[6];
    int        entryCount;
} LootTable;

/* Düşman tipine göre statik tabloyu döndürür */
const LootTable *GetLootTable(EnemyType type);

/* Ağırlıklı rastgele çekiliş yapar; düşürülen Item'ı *out'a yazar.
 * wave: dalga numarası (nadirlik bonus eşiği için).
 * Döner: true → bir şey düştü, false → hiçbir şey düşmedi. */
bool RollLootDrop(const LootTable *lt, int wave, Item *out);

/* T99 — Elite düşmanlar öldürülünce üretilen lanetli eşya.
 * Döner: true → üretildi, false → slot boş kaldı. */
bool GenerateCursedItem(EnemyType type, int wave, Item *out);

#endif /* LOOT_TABLE_H */
