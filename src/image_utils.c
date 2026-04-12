#include "../includes/database.h"

int save_helicopter_photo(int heli_id, const char *image_path) {
    char dest[256];
    snprintf(dest, sizeof(dest), "images/heli_%d.jpg", heli_id);
    FILE *src = fopen(image_path, "rb");
    if (!src) return 0;
    FILE *dst = fopen(dest, "wb");
    if (!dst) { fclose(src); return 0; }
    char buf[1024];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), src)) > 0)
        fwrite(buf, 1, n, dst);
    fclose(src); fclose(dst);
    char sql[512];
    snprintf(sql, sizeof(sql), "UPDATE helicopters SET photo_path = '%s' WHERE helicopter_id = %d", dest, heli_id);
    sqlite3_exec(db, sql, NULL, NULL, NULL);
    return 1;
}