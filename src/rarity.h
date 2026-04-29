#ifndef RARITY_H
#define RARITY_H
#include "types.h"

/* T92 — Nadirlik yardımcı fonksiyonları */
Color       RarityColor(ItemRarity r);
const char *RarityLabel(ItemRarity r);
const char *RarityFlavor(const char *itemName, ItemRarity r);

/* Eşya nadirliğini üreteç fonksiyonu — dalga numarası + rastgelelik */
ItemRarity  RollRarity(int waveSeed, int itemIdx);

#endif
