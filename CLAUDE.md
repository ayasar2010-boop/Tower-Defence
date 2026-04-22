# Tower Defense — CLAUDE.md

## Proje Özeti
C99 + Raylib ile yazılmış, üstten bakış (top-down) perspektifli Tower Defense oyunu.
Derleme: `gcc src/main.c -lraylib -lm -lpthread -ldl -lrt -o game`

## Proje Yapısı
```
TD_Test/
├── src/
│   └── main.c          ← Tek dosya, tüm oyun kodu
├── assets/             ← Sprite/ses (şimdilik boş)
├── Makefile
├── CLAUDE.md
├── TASKS.md
├── .gitignore
└── guidance.md         ← Teknik şartname / sistem prompt
```

## Teknik Kısıtlar
- **Dil**: Saf C (C99/C11), C++ yok
- **Kütüphane**: Raylib v4.5+
- **Derleme**: Tek dosya (`src/main.c`)
- **Çözünürlük**: 1280×720 @ 60 FPS
- **Bellek**: Sadece statik diziler — `malloc/free` kullanma, `#define` limitler

## Mimari
Game Loop: `Input → Update → Draw`  
State machine: `STATE_MENU → STATE_PLAYING → STATE_WAVE_CLEAR → STATE_GAMEOVER / STATE_VICTORY`

## Temel Sistemler
| Sistem       | Durum    | Notlar                                      |
|--------------|----------|---------------------------------------------|
| Map/Grid     | -        | 20×12, hardcoded, `InitMap()`               |
| Waypoints    | -        | Pixel koordinatları, `< 4.0f` eşiği         |
| Enemy        | -        | 3 tip: Normal/Fast/Tank, statik dizi        |
| Tower        | -        | 3 tip: Basic/Sniper/Splash, upgrade sistemi |
| Projectile   | -        | Homing, splash hasar                        |
| Particle     | -        | Alpha-fade, size shrink                     |
| Wave         | -        | 10 dalga, progresif zorluk                  |
| HUD          | -        | Üst panel, kule seçim, durum ekranları      |

## Önemli Sabitler
```c
GRID_COLS 20 / GRID_ROWS 12 / CELL_SIZE 48
MAX_ENEMIES 50 / MAX_TOWERS 30 / MAX_PROJECTILES 100
STARTING_GOLD 200 / STARTING_LIVES 20
```

## Kod Kuralları
- Fonksiyon isimleri: `PascalCase` (Raylib convention)
- Struct üyeleri: `camelCase`
- Her fonksiyon başına 1-2 satır Türkçe yorum
- Karmaşık algoritmalar adım adım açıklanmalı

## Kontroller
| Girdi         | Eylem                              |
|---------------|------------------------------------|
| Sol Tık       | Kule yerleştir                     |
| Sağ Tık       | Kule yükselt                       |
| 1 / 2 / 3     | Basic / Sniper / Splash seç        |
| P / ESC       | Duraklat                           |
| F             | Hız: 1× ↔ 2×                       |
| G             | Grid göster/gizle                  |
| SPACE         | Sonraki dalgayı başlat             |
| R             | Yeniden başlat                     |
