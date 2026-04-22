# SYSTEM PROMPT: C & Raylib Tower Defense — Kapsamlı Mimari ve Tam Uygulama

Senden C programlama dilini kullanarak, üstten bakış (top-down) perspektifine sahip, tam oynanabilir
bir **Tower Defense (Kule Savunma)** oyunu için kapsamlı bir teknik iskelet ve çalışan başlangıç kodu
oluşturmanı istiyorum. Oyun, aşağıda tanımlanan tüm sistemleri içermeli ve Raylib kütüphanesi yüklü
bir sistemde `gcc main.c -lraylib -lm -o game && ./game` komutuyla doğrudan derlenip çalışabilmelidir.

---

## 1. TEKNİK GEREKSİNİMLER

| Alan               | Detay                                                                 |
|---------------------|-----------------------------------------------------------------------|
| **Dil**            | Saf C (C99 veya C11 standartları, C++ uzantısı yok)                  |
| **Kütüphane**      | Raylib (v4.5+) — grafik, girdi, ses alt yapısı                       |
| **Derleme**        | Tek dosya (`main.c`), bağımlılık: `-lraylib -lm -lpthread -ldl -lrt` |
| **Hedef Çözünürlük** | 1280×720 piksel, 60 FPS hedefli                                    |
| **Mimari**         | Struct-based modüler yapı, temiz Game Loop (Input → Update → Draw)   |
| **Bellek Yönetimi** | Statik diziler (malloc/free kullanma), `#define` ile sabit limitler  |

---

## 2. SABİTLER VE KONFIGÜRASYON (`#define`)

Oyunun tüm dengeleme parametreleri dosyanın başında tanımlanmalı:

```
#define SCREEN_WIDTH           1280
#define SCREEN_HEIGHT          720
#define GRID_COLS              20        // Harita sütun sayısı
#define GRID_ROWS              12        // Harita satır sayısı
#define CELL_SIZE              48        // Her grid hücresinin piksel boyutu (48×48)
#define GRID_OFFSET_X          ((SCREEN_WIDTH - GRID_COLS * CELL_SIZE) / 2)
#define GRID_OFFSET_Y          80        // Üst kısımda HUD için boşluk

#define MAX_ENEMIES            50
#define MAX_TOWERS             30
#define MAX_PROJECTILES        100
#define MAX_PARTICLES          200       // Görsel efektler için
#define MAX_WAYPOINTS          20
#define MAX_WAVES              10

#define ENEMY_BASE_SPEED       60.0f     // piksel/saniye
#define ENEMY_BASE_HP          100.0f

#define TOWER_COST_BASIC       50
#define TOWER_COST_SNIPER      100
#define TOWER_COST_SPLASH      150

#define STARTING_GOLD          200
#define STARTING_LIVES         20
#define KILL_REWARD            15
```

---

## 3. VERİ YAPILARI (Structs)

### 3.1 Enum Tanımları

```c
typedef enum { CELL_EMPTY = 0, CELL_PATH = 1, CELL_BUILDABLE = 2, CELL_TOWER = 3 } CellType;
typedef enum { TOWER_BASIC, TOWER_SNIPER, TOWER_SPLASH, TOWER_TYPE_COUNT } TowerType;
typedef enum { ENEMY_NORMAL, ENEMY_FAST, ENEMY_TANK, ENEMY_TYPE_COUNT } EnemyType;
typedef enum { STATE_MENU, STATE_PLAYING, STATE_PAUSED, STATE_WAVE_CLEAR, STATE_GAMEOVER, STATE_VICTORY } GameState;
```

### 3.2 Enemy (Düşman)

```c
typedef struct {
    Vector2 position;          // Dünya koordinatı (piksel)
    Vector2 direction;         // Normalize edilmiş hareket vektörü
    float speed;               // piksel/saniye
    float maxHp;
    float currentHp;
    float slowTimer;           // Yavaşlatma efekti süresi (saniye)
    float slowFactor;          // 0.0–1.0 arası yavaşlama oranı
    int currentWaypoint;       // Takip edilen waypoint indeksi
    EnemyType type;
    Color color;               // Düşman tipi rengi
    float radius;              // Çarpışma ve çizim yarıçapı
    bool active;
} Enemy;
```

### 3.3 Tower (Kule)

```c
typedef struct {
    Vector2 position;          // Grid merkez koordinatı (piksel)
    int gridX, gridY;          // Grid hücre koordinatları
    TowerType type;
    float range;               // Menzil (piksel)
    float damage;
    float fireRate;            // Atış/saniye
    float fireCooldown;        // Kalan bekleme süresi
    float splashRadius;        // TOWER_SPLASH için patlama yarıçapı (diğerleri 0)
    int level;                 // Yükseltme seviyesi (1–3)
    float rotation;            // Namlu yönü (derece, çizim için)
    int targetEnemyIndex;      // Hedef düşman dizin numarası (-1 = hedef yok)
    Color color;
    bool active;
} Tower;
```

### 3.4 Projectile (Mermi)

```c
typedef struct {
    Vector2 position;
    Vector2 velocity;          // Hız vektörü (piksel/saniye)
    float damage;
    float splashRadius;        // 0 ise splash yok
    float speed;               // Mermi hızı (piksel/saniye)
    int targetIndex;           // Hedef düşman indeksi
    TowerType sourceType;      // Hangi kule tipinden atıldı (görsel ayrım)
    Color color;
    bool active;
} Projectile;
```

### 3.5 Particle (Görsel Efekt)

```c
typedef struct {
    Vector2 position;
    Vector2 velocity;
    float lifetime;            // Kalan ömür (saniye)
    float maxLifetime;
    Color color;
    float size;
    bool active;
} Particle;
```

### 3.6 Wave (Dalga Sistemi)

```c
typedef struct {
    EnemyType enemyType;       // Bu dalgadaki düşman tipi
    int enemyCount;            // Toplam düşman sayısı
    float spawnInterval;       // İki düşman arası süre (saniye)
    float spawnTimer;          // Spawn zamanlayıcı
    int spawnedCount;          // Şimdiye dek spawn edilen
    float preDelay;            // Dalga başlamadan önceki bekleme (saniye)
    bool started;
} Wave;
```

### 3.7 Game (Ana Oyun Durumu)

```c
typedef struct {
    int grid[GRID_ROWS][GRID_COLS];         // Harita grid'i (CellType)
    Vector2 waypoints[MAX_WAYPOINTS];       // Düşman yol noktaları (piksel)
    int waypointCount;

    Enemy enemies[MAX_ENEMIES];
    Tower towers[MAX_TOWERS];
    Projectile projectiles[MAX_PROJECTILES];
    Particle particles[MAX_PARTICLES];
    Wave waves[MAX_WAVES];

    int currentWave;
    int totalWaves;
    bool waveActive;

    int gold;
    int lives;
    int score;
    int enemiesKilled;

    GameState state;
    TowerType selectedTowerType;            // Yerleştirmek için seçili kule tipi
    float gameSpeed;                        // 1.0 = normal, 2.0 = hızlı
    bool showGrid;                          // Grid çizgilerini göster/gizle
} Game;
```

---

## 4. OYUN HARİTASI VE WAYPOINT SİSTEMİ

### 4.1 Grid Düzeni
- 2D integer dizisi (`grid[GRID_ROWS][GRID_COLS]`) ile temsil et.
- Değerler: `CELL_EMPTY (0)`, `CELL_PATH (1)`, `CELL_BUILDABLE (2)`, `CELL_TOWER (3)`.
- Haritayı `InitMap(Game *game)` fonksiyonunda hardcode olarak tanımla.
- Yol, sol kenardan başlayıp sağ kenarda biten, en az 2-3 viraj içeren bir rota olmalı.
- Yol kenarlarındaki tüm hücreler `CELL_BUILDABLE` olarak işaretlenmeli.

### 4.2 Waypoint Sistemi
- Yolun dönüş noktaları `waypoints[]` dizisinde piksel koordinatı olarak saklanmalı.
- Düşmanlar `waypoints[0]`'dan `waypoints[waypointCount-1]`'e doğru ilerlemeli.
- Waypoint'e ulaşma mesafesi: `< 4.0f piksel` — bu eşik aşıldığında `currentWaypoint++`.
- Son waypoint'e ulaşan düşman → `lives--`, düşman deaktive edilir.

---

## 5. DÜŞMAN SİSTEMİ

### 5.1 Düşman Tipleri ve Özellik Tablosu

| Tip           | HP     | Hız Çarpanı | Yarıçap | Renk      | Ödül Çarpanı |
|---------------|--------|-------------|---------|-----------|---------------|
| ENEMY_NORMAL  | 100    | 1.0×        | 10      | RED       | 1.0×          |
| ENEMY_FAST    | 60     | 1.8×        | 8       | ORANGE    | 1.2×          |
| ENEMY_TANK    | 300    | 0.6×        | 14      | DARKPURPLE| 2.0×          |

### 5.2 Hareket Algoritması
```
Her frame için (dt = GetFrameTime()):
  1. Hedef waypoint'e yön vektörü hesapla:
       direction = Normalize(waypoints[currentWaypoint] - position)
  2. Yavaşlatma efektini uygula:
       effectiveSpeed = speed * (slowTimer > 0 ? slowFactor : 1.0)
  3. Pozisyonu güncelle:
       position += direction * effectiveSpeed * dt
  4. Hedefe ulaşıldı mı kontrol et:
       if (Distance(position, waypoints[currentWaypoint]) < 4.0f)
           currentWaypoint++
  5. Son waypoint aşıldıysa:
       lives--, enemy.active = false
```

### 5.3 Sağlık Çubuğu
- Her düşmanın üstünde yatay HP bar çiz (genişlik: `radius * 2.5`).
- Arkaplan: DARKGRAY, dolgu: yeşilden kırmızıya gradyan (HP oranına göre).
- Sadece `currentHp < maxHp` olduğunda göster.

---

## 6. KULE SİSTEMİ

### 6.1 Kule Tipleri ve Özellik Tablosu

| Tip          | Maliyet | Menzil | Hasar | Atış Hızı | Splash | Renk      |
|--------------|---------|--------|-------|-----------|--------|-----------|
| TOWER_BASIC  | 50      | 150px  | 20    | 2.0/s     | 0      | BLUE      |
| TOWER_SNIPER | 100     | 300px  | 80    | 0.5/s     | 0      | DARKGREEN |
| TOWER_SPLASH | 150     | 120px  | 30    | 1.0/s     | 60px   | MAROON    |

### 6.2 Hedef Seçim Algoritması (Targeting)
Kuleler "en yakın düşman" stratejisini kullanmalı:
```
int FindTarget(Game *game, Tower *tower):
    minDist = FLOAT_MAX
    targetIdx = -1
    for each active enemy:
        dist = Distance(tower->position, enemy.position)
        if (dist <= tower->range AND dist < minDist):
            minDist = dist
            targetIdx = i
    return targetIdx
```

### 6.3 Ateş Etme Mekanizması
```
Her frame (dt):
    tower->fireCooldown -= dt
    if (fireCooldown <= 0 AND target geçerli):
        CreateProjectile(tower->position, targetIndex, tower->damage, tower->splashRadius)
        fireCooldown = 1.0 / tower->fireRate
```

### 6.4 Kule Yerleştirme Kuralları
1. Fare pozisyonunu grid koordinatına çevir:
   `gridX = (mouseX - GRID_OFFSET_X) / CELL_SIZE`
   `gridY = (mouseY - GRID_OFFSET_Y) / CELL_SIZE`
2. Sınır kontrolü: `0 ≤ gridX < GRID_COLS` ve `0 ≤ gridY < GRID_ROWS`
3. Hücre kontrolü: `grid[gridY][gridX] == CELL_BUILDABLE`
4. Altın kontrolü: `gold >= towerCost`
5. Başarılıysa: hücreyi `CELL_TOWER` yap, altını düş, Tower struct'ı aktifleştir.

### 6.5 Kule Yükseltme (Upgrade)
- Mevcut kuleye sağ tık → seviye yükselt (maks 3).
- Her seviye: hasar ×1.3, menzil ×1.1, maliyet = başlangıç maliyeti × seviye.
- Görsel olarak kule boyutunu ve rengini koyulaştırarak seviyeleri ayırt et.

---

## 7. MERMİ (PROJECTILE) SİSTEMİ

### 7.1 Mermi Hareketi
```
Her frame (dt):
    if (hedef aktif):
        direction = Normalize(target.position - projectile.position)
        projectile.velocity = direction * projectile.speed
    position += velocity * dt
    if (Distance(position, target.position) < target.radius):
        → İsabet! Hasar uygula
        → splashRadius > 0 ise: Çevredeki tüm düşmanlara splash hasar uygula
        → Particle efekti oluştur
        → Mermiyi deaktive et
    if (hedef öldüyse):
        → Mermiyi deaktive et (veya son bilinen pozisyona ilerlet)
```

### 7.2 Splash Hasar
- TOWER_SPLASH mermileri isabet noktasında `splashRadius` içindeki tüm düşmanlara hasar verir.
- Splash hasarı mesafeye göre azalsın: `damage * (1.0 - dist/splashRadius)`.

---

## 8. DALGA (WAVE) SİSTEMİ

### 8.1 Dalga Tanımları
```c
void InitWaves(Game *game) {
    // Dalga 1: 8 normal düşman, 1.2 saniye aralıkla
    // Dalga 2: 12 normal + 3 hızlı karışık
    // Dalga 3: 6 tank düşman, 2 saniye aralıkla
    // ...progresif zorluk artışı
}
```

### 8.2 Dalga Akışı
1. `STATE_WAVE_CLEAR` durumunda "Sonraki Dalga" butonu veya 5 saniyelik otomatik başlatma.
2. Dalga aktifken: `spawnTimer` sayar, her aralıkta yeni düşman spawn eder.
3. Tüm düşmanlar öldürüldüğünde veya yolun sonuna ulaştığında → dalga tamamlandı.
4. Son dalga temizlendiğinde → `STATE_VICTORY`.

---

## 9. KULLANICI ARAYÜZÜ (HUD)

### 9.1 Üst Panel (y: 0–GRID_OFFSET_Y)
Sol taraf: `❤ Canlar: 20  |  💰 Altın: 200  |  ⚔ Skor: 0`
Sağ taraf: `Dalga: 1/10  |  Hız: 1x/2x  |  ⏸ Duraklat`

### 9.2 Alt Panel veya Yan Panel
- Kule seçim butonları (3 adet, kule tipi + maliyet bilgisi).
- Seçili kulenin özelliklerini gösteren tooltip.
- Fare grid üzerindeyken: yerleştirilebilirlik önizlemesi (yeşil/kırmızı yarı saydam kare).
- Seçili kulenin menzilini gösteren yarı saydam daire.

### 9.3 Oyun Durumu Ekranları
- **STATE_MENU**: Oyun başlığı + "Başla" butonu (basit dikdörtgen + metin).
- **STATE_PAUSED**: Yarı saydam karartma + "Devam Et" / "Çık" butonları.
- **STATE_GAMEOVER**: "GAME OVER" + skor + "Yeniden Başla" butonu.
- **STATE_VICTORY**: "TEBRİKLER!" + final skor + "Yeniden Başla" butonu.

---

## 10. PARTİKÜL (EFEKT) SİSTEMİ

### 10.1 Efekt Tetikleyicileri
- **Düşman ölümü**: 8–12 parçacık, düşman renginde, dışa doğru dağılır (lifetime: 0.4s).
- **Mermi isabet**: 4–6 parçacık, mermi renginde (lifetime: 0.2s).
- **Splash patlama**: 10–15 parçacık, turuncu/sarı, geniş dağılım (lifetime: 0.5s).

### 10.2 Parçacık Güncelleme
```
position += velocity * dt
lifetime -= dt
size *= (lifetime / maxLifetime)    // Zamanla küçülme
color.a = (unsigned char)(255 * (lifetime / maxLifetime))  // Solma
if (lifetime <= 0): active = false
```

---

## 11. GİRDİ YÖNETİMİ

| Girdi                    | Eylem                                          |
|--------------------------|-------------------------------------------------|
| Sol Tık (Grid üzerinde)  | Seçili kuleyi yerleştir                         |
| Sağ Tık (Kule üzerinde)  | Kuleyi yükselt (upgrade)                        |
| Tuş `1`, `2`, `3`        | Kule tipini seç (Basic, Sniper, Splash)         |
| Tuş `P` veya `ESCAPE`    | Oyunu duraklat / devam ettir                    |
| Tuş `F`                  | Oyun hızını değiştir (1× ↔ 2×)                 |
| Tuş `G`                  | Grid çizgilerini göster/gizle                   |
| Tuş `SPACE`              | Sonraki dalgayı başlat (wave clear durumunda)   |
| Tuş `R`                  | Oyunu yeniden başlat (game over / victory'de)   |

---

## 12. ÇİZİM KATMANLARI (Draw Order)

Çizim sırası (arkadan öne):

1. **Arkaplan**: Koyu yeşil/gri zemin rengi (`ClearBackground`)
2. **Grid Hücreleri**: Hücre tipine göre renk — yol: açık kahverengi, boş: koyu yeşil, buildable: hafif yeşil
3. **Grid Çizgileri**: İnce, yarı saydam çizgiler (opsiyonel, `showGrid` ile toggle)
4. **Menzil Önizleme**: Fare grid üzerindeyken seçili kulenin menzil dairesi (yarı saydam)
5. **Kuleler**: Kare gövde + yönlendirilmiş namlu çizgisi (rotation ile)
6. **Düşmanlar**: Daire (tip renginde) + sağlık çubuğu
7. **Mermiler**: Küçük daire veya çizgi (kule tipine göre renk)
8. **Parçacıklar**: Alpha blended küçük kareler/daireler
9. **HUD**: Üst panel, alt panel, butonlar, metinler
10. **Durum Ekranları**: Menu, Pause, GameOver, Victory overlay'leri

---

## 13. YARDIMCI FONKSİYONLAR

Aşağıdaki utility fonksiyonlarını implemente et:

```c
// Vektör işlemleri
float Vector2Distance(Vector2 a, Vector2 b);
Vector2 Vector2Normalize(Vector2 v);
Vector2 Vector2Subtract(Vector2 a, Vector2 b);
Vector2 Vector2Scale(Vector2 v, float scale);
Vector2 Vector2Add(Vector2 a, Vector2 b);
float Vector2Length(Vector2 v);

// Grid ↔ Dünya koordinat dönüşümleri
Vector2 GridToWorld(int gridX, int gridY);     // Grid hücresinin merkez piksel koordinatı
bool WorldToGrid(Vector2 worldPos, int *gx, int *gy);  // Piksel → grid indeks

// Oyun mantığı yardımcıları
void SpawnEnemy(Game *game, EnemyType type);
void CreateProjectile(Game *game, Vector2 origin, int targetIdx, float damage, float splash);
void SpawnParticles(Game *game, Vector2 pos, Color color, int count, float speed, float lifetime);
int FindNearestEnemy(Game *game, Vector2 pos, float range);
bool CanPlaceTower(Game *game, int gx, int gy);
int GetTowerCost(TowerType type);
```

---

## 14. ANA GAME LOOP YAPISI

```c
int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tower Defense - C & Raylib");
    SetTargetFPS(60);

    Game game = {0};
    InitGame(&game);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime() * game.gameSpeed;

        switch (game.state) {
            case STATE_MENU:    UpdateMenu(&game);    break;
            case STATE_PLAYING: HandleInput(&game);
                                UpdateEnemies(&game, dt);
                                UpdateTowers(&game, dt);
                                UpdateProjectiles(&game, dt);
                                UpdateParticles(&game, dt);
                                UpdateWaves(&game, dt);
                                CheckGameConditions(&game);
                                break;
            case STATE_PAUSED:  UpdatePause(&game);   break;
            // ... diğer durumlar
        }

        BeginDrawing();
            ClearBackground((Color){34, 40, 49, 255});
            switch (game.state) {
                case STATE_MENU:    DrawMenu(&game);    break;
                case STATE_PLAYING: DrawGame(&game);
                                    DrawHUD(&game);     break;
                case STATE_PAUSED:  DrawGame(&game);
                                    DrawHUD(&game);
                                    DrawPauseOverlay(); break;
                // ... diğer durumlar
            }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
```

---

## 15. KOD KALİTESİ VE YORUM GEREKSİNİMLERİ

- **Her fonksiyonun** başına ne yaptığını açıklayan 1-2 satır Türkçe yorum yaz.
- **Karmaşık algoritmalar** (vektör hesaplamaları, hedef seçimi, splash hasar, grid dönüşümleri) adım adım açıkla.
- Sabit değerlerin yanına (ör. `< 4.0f`) neden o değerin seçildiğini açıkla.
- Fonksiyon isimlendirmesi: `PascalCase` (Raylib convention'a uygun).
- Struct üyeleri: `camelCase`.

---

## 16. ÇIKTI FORMATI

### 16.1 Önerilen Proje Yapısı
```
tower_defense/
├── src/
│   ├── main.c          ← Tüm oyun kodu (tek dosya)
│   └── (ileride bölünebilir: game.c, enemy.c, tower.c, ui.c, ...)
├── assets/             ← (Şimdilik boş, ileride sprite/ses dosyaları)
├── Makefile            ← Derleme komutu
└── README.md           ← Kurulum ve çalıştırma talimatları
```

### 16.2 Beklenen Çıktı
1. Yukarıdaki **tüm** struct tanımlarını, sabitleri, enum'ları ve fonksiyonları içeren tek bir `main.c`.
2. Kod derlendiğinde çalışan, oynanan bir oyun olmalı:
   - Harita ve yol görünür
   - Düşmanlar waypoint'leri takip ederek hareket eder
   - 1-2-3 tuşlarıyla kule seçilir, tıkla yerleştirilir
   - Kuleler ateş eder, mermiler hedefe gider, düşmanlar hasar alır
   - Can ve altın sistemi işler
   - Dalga sistemi çalışır
   - Game Over / Victory ekranları görüntülenir
3. Kodu olduğu gibi kopyalayıp derleyebileyim, ek dosya gerektirmesin.
4. Makefile örneği de sağla.

---

## 17. BONUS (ZORUNLU DEĞİL AMA İSTENİR)

- Kule satma (sağ tık menüsü, maliyetin %50'si geri döner).
- Düşman yolu üzerinde yön okları (görsel ipucu).
- Mini harita veya dalga ilerleme çubuğu.
- Düşmanların farklı geometrik şekillerle çizilmesi (normal: daire, hızlı: üçgen, tank: kare).
- Kule ateş ederken kısa bir "flash" animasyonu.
- Ses efektleri (`LoadSound` / `PlaySound` ile basit .wav dosyaları, yoksa atlansın).