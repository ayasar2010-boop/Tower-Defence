#ifndef GUARDIAN_H
#define GUARDIAN_H
#include "types.h"

void InitGuardians(Game *g);
void UpdateGuardians(Game *g, float dt);
void DrawGuardians(Game *g);
/* Friendly unit guardian'a hasar verdi; idx = guardians[] indexi */
void DamageGuardian(Game *g, int idx, float dmg);

#endif
