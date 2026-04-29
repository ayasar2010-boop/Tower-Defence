/* i18n.h — T59: Lokalizasyon altyapısı */

#ifndef I18N_H
#define I18N_H

#define MAX_STRING_ENTRIES 256
#define MAX_STRING_KEY_LEN  64
#define MAX_STRING_VAL_LEN 128

typedef enum { LANG_TR = 0, LANG_EN = 1 } Language;

/* Dil dosyasını yükler; yoksa varsayılan (TR) kullanır */
void        I18nLoad(Language lang);

/* Verilen anahtar için yerelleştirilmiş metni döndürür.
   Anahtar bulunamazsa anahtarı olduğu gibi döndürür. */
const char *T(const char *key);

#endif
