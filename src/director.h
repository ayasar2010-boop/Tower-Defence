/* director.h — T98: Director AI — performansa dayalı dinamik zorluk */
#ifndef DIRECTOR_H
#define DIRECTOR_H

#include "types.h"

void  InitDirector(DirectorAI *d, int startingLives);

/* Dalga başladığında çağrılır: zaman ve can snapshotu alır */
void  DirectorOnWaveStart(DirectorAI *d, int currentLives);

/* Dalga bittiğinde çağrılır: performans hesaplar, eliteChance günceller */
void  DirectorOnWaveClear(DirectorAI *d, int currentLives);

/* SpawnEnemy sonrası: eliteChance'a göre düşmanı elite'e yükseltir */
void  DirectorApplyElite(DirectorAI *d, Enemy *e);

/* HUD çizimi: sağ üst köşeye Director durumu */
void  DrawDirectorStatus(const DirectorAI *d);

#endif /* DIRECTOR_H */
