/* i18n.c — T59: Lokalizasyon altyapısı */

#include "i18n.h"
#include "logger.h"
#include <stdio.h>
#include <string.h>

typedef struct {
    char key[MAX_STRING_KEY_LEN];
    char val[MAX_STRING_VAL_LEN];
} StringEntry;

static StringEntry g_strings[MAX_STRING_ENTRIES];
static int         g_count = 0;
static Language    g_lang  = LANG_TR;

static void LoadFromFile(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        LOG_WARN("i18n: dosya acilamadi: %s", path);
        return;
    }
    g_count = 0;
    char line[MAX_STRING_KEY_LEN + MAX_STRING_VAL_LEN + 4];
    while (g_count < MAX_STRING_ENTRIES && fgets(line, sizeof(line), f)) {
        /* Yorum ve boş satırları atla */
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;
        char *eq = strchr(line, '=');
        if (!eq) continue;
        *eq = '\0';
        /* Anahtar kopyala */
        strncpy(g_strings[g_count].key, line, MAX_STRING_KEY_LEN - 1);
        g_strings[g_count].key[MAX_STRING_KEY_LEN - 1] = '\0';
        /* Değer kopyala, satır sonu temizle */
        char *val = eq + 1;
        int vlen = (int)strlen(val);
        while (vlen > 0 && (val[vlen-1] == '\n' || val[vlen-1] == '\r'))
            val[--vlen] = '\0';
        strncpy(g_strings[g_count].val, val, MAX_STRING_VAL_LEN - 1);
        g_strings[g_count].val[MAX_STRING_VAL_LEN - 1] = '\0';
        g_count++;
    }
    fclose(f);
    LOG_INFO("i18n: %d string yuklendi (%s)", g_count, path);
}

void I18nLoad(Language lang) {
    g_lang = lang;
    g_count = 0;
    if (lang == LANG_EN)
        LoadFromFile("strings_en.txt");
    else
        LoadFromFile("strings_tr.txt");
}

const char *T(const char *key) {
    for (int i = 0; i < g_count; i++)
        if (strcmp(g_strings[i].key, key) == 0)
            return g_strings[i].val;
    return key; /* Bulunamazsa anahtarı olduğu gibi döndür */
}
