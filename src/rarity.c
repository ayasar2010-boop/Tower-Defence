#include "rarity.h"
#include <string.h>

/* ============================================================
 * T92 — Nadirlik renk, etiket, flavor tabloları
 * ============================================================ */

static const Color RARITY_COLORS[RARITY_COUNT] = {
    {180, 180, 180, 255}, /* Common    — gri    */
    {80,  200, 80,  255}, /* Uncommon  — yeşil  */
    {80,  140, 230, 255}, /* Rare      — mavi   */
    {180, 60,  240, 255}, /* Epic      — mor    */
    {255, 200, 40,  255}, /* Mythical  — altın  */
};

static const char *RARITY_LABELS[RARITY_COUNT] = {
    "Yaygın", "Nadir", "Seyrek", "Destansı", "Efsanevi"
};

/* Flavor metinleri — LORE.md bağlantısı için genişletilebilir */
static const char *FLAVOR_BY_RARITY[RARITY_COUNT] = {
    "Sıradan ama güvenilir.",
    "Ustanın elinden çıkmış.",
    "Eski bir savaşın tanığı.",
    "Ejderin kanıyla işlenmiş.",
    "Zamanın başında dövülmüş.",
};

/* İsme özel flavor tablosu (LORE.md → genişletilebilir) */
static const struct { const char *key; const char *flavor; } NAME_FLAVORS[] = {
    { "Keskin Kılıç",    "Her kesimde bir zafer." },
    { "Demir Zırh",      "Binlerce okun tanığı."  },
    { "Büyülü Rün",      "Söyleyene güç verir."   },
    { "Şifa İksiri",     "Hayat, bir şişede."      },
    { "Ejder Kolyesi",   "Ateşin özünden damıtıldı."},
    { "Komutan Kalkanı", "İrade kılıçtan serttir." },
};
#define NAME_FLAVOR_COUNT 6

Color RarityColor(ItemRarity r) {
    if (r < 0 || r >= RARITY_COUNT) r = RARITY_COMMON;
    return RARITY_COLORS[r];
}

const char *RarityLabel(ItemRarity r) {
    if (r < 0 || r >= RARITY_COUNT) r = RARITY_COMMON;
    return RARITY_LABELS[r];
}

const char *RarityFlavor(const char *itemName, ItemRarity r) {
    if (itemName) {
        for (int i = 0; i < NAME_FLAVOR_COUNT; i++) {
            if (strncmp(NAME_FLAVORS[i].key, itemName, 31) == 0)
                return NAME_FLAVORS[i].flavor;
        }
    }
    if (r < 0 || r >= RARITY_COUNT) r = RARITY_COMMON;
    return FLAVOR_BY_RARITY[r];
}

/* Basit deterministic rarity roll: waveSeed + itemIdx hash */
ItemRarity RollRarity(int waveSeed, int itemIdx) {
    unsigned int h = (unsigned int)(waveSeed * 2654435761u ^ (unsigned int)(itemIdx * 1234567891u));
    int v = (int)(h % 100);
    /* Common 50%, Uncommon 27%, Rare 14%, Epic 7%, Mythical 2% */
    if (v < 50) return RARITY_COMMON;
    if (v < 77) return RARITY_UNCOMMON;
    if (v < 91) return RARITY_RARE;
    if (v < 98) return RARITY_EPIC;
    return RARITY_MYTHICAL;
}
