#include "../includes/database.h"
#include <string.h>

int authenticate(const char *username, const char *password, int *is_commander, int *crew_id) {
    const char *sql = "SELECT role, crew_id FROM app_users WHERE username=? AND password_hash=?";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return 0;
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const char *role = (const char *)sqlite3_column_text(stmt, 0);
        *is_commander = (strcmp(role, "COMMANDER") == 0);
        *crew_id = sqlite3_column_int(stmt, 1);
        sqlite3_finalize(stmt);
        return 1;
    }
    sqlite3_finalize(stmt);
    return 0;
}