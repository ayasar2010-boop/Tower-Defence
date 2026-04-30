/* quest.h — T97: Görev (Quest) yönetim sistemi */
#ifndef QUEST_H
#define QUEST_H

#include "types.h"

/* Tüm görevleri ACTIVE durumunda başlatır */
void InitQuestManager(QuestManager *qm);

/* progress artır; görev BU çağrıyla tamamlandıysa true döner */
bool QuestAdvance(QuestManager *qm, QuestType type, int amount);

bool IsQuestDone(const QuestManager *qm, QuestType type);

/* Bildirim timer'ını ilerlet (her frame dt ile çağrılır) */
void UpdateQuestNotify(QuestManager *qm, float dt);

/* Aktif görevleri ekranın sağ altına, tamamlananları yeşil çizer */
void DrawQuestHUD(const QuestManager *qm);

#endif /* QUEST_H */
