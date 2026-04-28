# 👑 RULER — Tower Defense & RPG Hybrid

> **Sınırlarını koru, kahramanını geliştir ve krallığını yönet!**  
> *Ruler*, klasik Kule Savunma (Tower Defense) mekaniklerini, MOBA tarzı kahraman kontrolleri, RPG derinliği ve *Age of Empires* esintili "Ana Kent" (Home City) sistemiyle birleştiren destansı bir strateji oyunudur.

Arendalm Krallığı tehlike altında. Kuzey Bozkırları'nın acımasız savaş lordu General Krom, sınırları aştı. Sadece kuleler inşa etmek yetmeyecek; bizzat cepheye inmeli, kılıcını savurmalı ve başkentten destek çağırarak bu amansız işgali durdurmalısın!

---

## ✨ Öne Çıkan Özellikler

🛡️ **Statik Değil, Dinamik Savunma:**
Sadece kule dikip beklemek yok! Kuleler yan yana geldiğinde **Sinerji Sistemi** devreye girer. Kule kombinasyonları ile atış hızını artırın, splash (patlama) yarıçaplarını devasa boyutlara taşıyın.

⚔️ **MOBA Tarzı Kahraman Kontrolü (RPG):**
Bir kahraman seçin (Savaşçı, Büyücü, Okçu). Sağ tık ile savaş alanında serbestçe dolaşın. Level atlayın, statlarınızı geliştirin ve **Q, W, E, R** tuşlarıyla savaşın kaderini değiştirecek yeteneklerinizi (Skill) kullanın.

🏰 **Ana Kent (Home City) Sistemi:**
Siz savaştıkça krallığınızın *Refah Puanı* (Prosperity) artar. Dalgalar arasında (Prep Phase) kışla veya pazar inşa edin. Zor anlarda Ana Kent'ten piyade, okçu veya şövalye takviyesi (Reinforcement) çağırın!

🗺️ **Karanlık Zindanlar (Dungeon Mode):**
Dalga aralarında boş durmak yok! Kahramanınızla zindanlara inip tehlikeli canavarları avlayın. Topladığınız ganimetleri (Loot) ana haritaya altın ve buff olarak taşıyın.

👹 **Destansı Boss Savaşları:**
Sıradan düşman dalgaları yerini zamanla devasa Boss'lara bırakır. Çok fazlı (Multi-phase) boss savaşlarında stratejinizi anında değiştirmeniz gerekecek.

---

## 🎮 Nasıl Oynanır? (Kontroller)

Oyun, akıcı bir strateji ve aksiyon deneyimi sunmak için tasarlanmıştır:

| Tuş / Fare | Eylem |
| :--- | :--- |
| **Sol Tık** | Kule yerleştir / Hedef Seç / Menü Etkileşimi |
| **Sağ Tık** | Kule Bilgisi & Yükseltme / **Kahramanı Hareket Ettir (MOBA)** |
| **1, 2, 3** | Kule Tipini Seç (Basic, Sniper, Splash) |
| **4, 5, 6** | İnşa Fazında Bina Seç (Kışla, Pazar, Barikat) |
| **Q, W, E, R** | Kahraman Yeteneklerini (Skill) Kullan |
| **SPACE** | Sonraki Dalgayı Başlat / Diyalog Geç |
| **D** | Dalga Arasında Zindana (Dungeon) Gir |
| **C** | Ana Kent (Home City) Takviye Panelini Aç |
| **Scroll** | Kamerayı Yakınlaştır / Uzaklaştır |
| **F / P** | Oyun Hızını Değiştir (2x) / Oyunu Duraklat |

---

## 💻 Geliştiriciler İçin (Teknik Detaylar)

Bu proje, modern motorların (Unity/Unreal) arkasındaki temel oyun döngüsü mantığını göstermek amacıyla **saf C (C99) ve Raylib** kullanılarak geliştirilmiştir. *Object-Oriented* bir yapı yerine, *Data-Oriented* struct mimarisi ve *State Machine* kullanılmıştır. Dinamik bellek yönetimi (malloc/free) yerine, performans ve güvenilirlik için tamamen statik bellek (Object Pooling) tercih edilmiştir.

### Gereksinimler
- **C Derleyicisi:** GCC (MinGW-w64 Windows için)
- **Kütüphane:** [Raylib v4.5+](https://www.raylib.com/)
- **Araçlar:** Make

### Derleme ve Çalıştırma

**Windows (MSYS2/MinGW64):**
Proje dizininde terminali açın ve make komutunu çalıştırın:
```bash
# Oyunu derlemek için
make

# Derleyip hemen oynamak için
make run
```

**VS Code Ortamı Hazır!**
Proje içindeki `.vscode` klasörü sayesinde `Ctrl+Shift+B` ile anında derleyebilir, `F5` ile **GDB Debug** modunda çalıştırabilirsiniz.

### Mimari İskelet
- `main.c` — Game Loop, Kuleler, Düşmanlar, Mermiler, Çizim Katmanları
- `homecity.c` — Medeniyet gelişimi, puan tabanlı takviye sistemi
- `dungeon.c` — RPG kahraman kontrolleri, yetenekler, loot ve envanter
- `siege.c` — İzometrik hesaplamalar, sur savunması ve barikat inşası
- `ui.c` — Özelleştirilebilir "Epic" arayüz, Typewriter diyalog sistemi

---

## 🎵 Krediler ve Varlıklar (Assets)

* **Kodlama & Tasarım:** Ali Erdoğan (ve AI Asistanı 🤖)
* **Motor:** Raylib - Teşekkürler Ramon Santamaria!
* *(Varlıklar eklendikçe görsel ve ses tasarımcıları buraya eklenecektir.)*

---

> *"Savaş sadece surlarda değil, halkın kalbinde kazanılır." - Başkomutan Aldric*