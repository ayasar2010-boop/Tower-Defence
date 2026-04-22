# TASKS — Tower Defense Geliştirme Planı

Tüm kod `src/main.c` tek dosyasında toplanır.  
Her görev tamamlandığında `[x]` ile işaretlenir.

---

## AŞAMA 1 — İskelet ve Temel Yapı
- [ ] **T01** — Dizin yapısını oluştur (`src/`, `assets/`)
- [ ] **T02** — `Makefile` yaz (Linux + Windows hedefleri)
- [ ] **T03** — `src/main.c` iskeleti: `#include`, `#define` sabitleri, enum'lar
- [ ] **T04** — Tüm struct tanımları: `Enemy`, `Tower`, `Projectile`, `Particle`, `Wave`, `Game`
- [ ] **T05** — `main()` game loop iskelet: pencere, 60 FPS, state switch, `BeginDrawing/EndDrawing`

## AŞAMA 2 — Harita ve Navigasyon
- [ ] **T06** — `InitMap()`: 20×12 grid, hardcoded yol + buildable hücreler
- [ ] **T07** — `InitWaypoints()`: Piksel waypoint dizisi
- [ ] **T08** — `DrawMap()`: Hücre tipine göre renkli grid çizimi
- [ ] **T09** — Grid çizgisi toggle (`showGrid`)
- [ ] **T10** — `GridToWorld()` / `WorldToGrid()` dönüşüm fonksiyonları

## AŞAMA 3 — Düşman Sistemi
- [ ] **T11** — `SpawnEnemy()`: Tip tablosuna göre HP/hız/renk/yarıçap ata
- [ ] **T12** — `UpdateEnemies()`: Waypoint takibi, `slowTimer`, son waypoint → `lives--`
- [ ] **T13** — `DrawEnemies()`: Tip şekli (daire/üçgen/kare) + HP bar

## AŞAMA 4 — Kule Sistemi
- [ ] **T14** — `CanPlaceTower()` doğrulama (sınır + buildable + altın)
- [ ] **T15** — Kule yerleştirme: Sol tık → hücreyi `CELL_TOWER` yap, altın düş
- [ ] **T16** — `FindNearestEnemy()`: Menzil içindeki en yakın aktif düşman
- [ ] **T17** — `UpdateTowers()`: `fireCooldown`, hedef seçimi, `CreateProjectile()`
- [ ] **T18** — `DrawTowers()`: Kare gövde + namlu çizgisi (rotation)
- [ ] **T19** — Kule yükseltme: Sağ tık → level++, hasar/menzil artışı

## AŞAMA 5 — Mermi ve Parçacık Sistemi
- [ ] **T20** — `CreateProjectile()`: Homing vektör, splash bilgisi
- [ ] **T21** — `UpdateProjectiles()`: Hareket, isabet tespiti, splash hasar
- [ ] **T22** — `DrawProjectiles()`: Kule tipine göre renk/şekil
- [ ] **T23** — `SpawnParticles()`: Ölüm / isabet / splash efektleri
- [ ] **T24** — `UpdateParticles()`: Fade + shrink, `lifetime <= 0` deaktive
- [ ] **T25** — `DrawParticles()`: Alpha-blended daire/kare

## AŞAMA 6 — Dalga Sistemi
- [ ] **T26** — `InitWaves()`: 10 dalganın tanımı (tip, sayı, aralık, ön-gecikme)
- [ ] **T27** — `UpdateWaves()`: `spawnTimer`, `SpawnEnemy()` tetikleme
- [ ] **T28** — Dalga tamamlanma tespiti: Tüm düşmanlar öldü/geçti → `STATE_WAVE_CLEAR`
- [ ] **T29** — Son dalga → `STATE_VICTORY`

## AŞAMA 7 — HUD ve Girdi
- [ ] **T30** — `DrawHUD()`: Üst panel (can/altın/skor/dalga), kule seçim butonları
- [ ] **T31** — Yerleştirme önizlemesi: Fare grid'deyken yeşil/kırmızı yarı saydam kare + menzil dairesi
- [ ] **T32** — `HandleInput()`: 1/2/3 kule seçimi, sol tık yerleştir, sağ tık yükselt, P/ESC/F/G/SPACE/R

## AŞAMA 8 — Durum Ekranları
- [ ] **T33** — `DrawMenu()` + `UpdateMenu()`: Başlık + "Başla" butonu
- [ ] **T34** — `DrawPauseOverlay()` + `UpdatePause()`: Karartma + Devam/Çık
- [ ] **T35** — `DrawGameOver()`: GAME OVER + skor + Yeniden Başla
- [ ] **T36** — `DrawVictory()`: TEBRİKLER + final skor + Yeniden Başla
- [ ] **T37** — `InitGame()` / `RestartGame()`: Sıfırlama fonksiyonu

## AŞAMA 9 — Derleme ve Test
- [ ] **T38** — `Makefile` doğrula, `make` ile temiz derleme
- [ ] **T39** — Oynanabilirlik testi: Tüm dalgalar, kule yerleştir/yükselt, game over/victory
- [ ] **T40** — Bellek / sınır kontrolü: Dizi overflow yok, grid sınır aşımı yok

## BONUS (İsteğe Bağlı)
- [ ] **B01** — Kule satma (sağ tık menüsü, %50 geri)
- [ ] **B02** — Yol yön okları
- [ ] **B03** — Dalga ilerleme çubuğu (mini progress bar)
- [ ] **B04** — Kule ateş flash animasyonu

---

## Durum Özeti
| Aşama | Toplam | Tamamlanan |
|-------|--------|------------|
| 1 — İskelet      | 5  | 0 |
| 2 — Harita       | 5  | 0 |
| 3 — Düşman       | 3  | 0 |
| 4 — Kule         | 6  | 0 |
| 5 — Mermi/Partik | 6  | 0 |
| 6 — Dalga        | 4  | 0 |
| 7 — HUD/Girdi    | 3  | 0 |
| 8 — Ekranlar     | 5  | 0 |
| 9 — Test         | 3  | 0 |
| **Toplam**       | **40** | **0** |
