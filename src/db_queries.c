#include "../includes/database.h"
#include <sqlite3.h>

sqlite3 *db = NULL;

int init_db(const char *db_path) {
    return sqlite3_open(db_path, &db);
}

void close_db() {
    if (db) sqlite3_close(db);
}

// Пример одного SELECT‑запроса (общее количество часов и ресурс)
void select_flight_hours_and_resource() {
    const char *sql =
        "SELECT h.helicopter_id, h.serial_number, "
        "COALESCE(SUM(f.duration), 0) AS total_flight_time, "
        "h.flight_resource - COALESCE(SUM(f.duration), 0) AS remaining_resource "
        "FROM helicopters h "
        "LEFT JOIN flights f ON h.helicopter_id = f.helicopter_id "
        "GROUP BY h.helicopter_id;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return;
    }

    printf("\n=== Flight hours and remaining resource ===\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *sn = (const char *)sqlite3_column_text(stmt, 1);
        int total = sqlite3_column_int(stmt, 2);
        int rem = sqlite3_column_int(stmt, 3);
        printf("Helicopter %d (%s): total = %d h, remaining = %d h\n", id, sn, total, rem);
    }
    sqlite3_finalize(stmt);
}

// Проверка ресурса
int check_flight_resource(int helicopter_id, int new_duration) {
    const char *sql = "SELECT COALESCE(SUM(duration),0), flight_resource FROM helicopters h "
                      "LEFT JOIN flights f ON h.helicopter_id = f.helicopter_id "
                      "WHERE h.helicopter_id = ? GROUP BY h.helicopter_id;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return 0;
    sqlite3_bind_int(stmt, 1, helicopter_id);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return 0;
    }
    int total = sqlite3_column_int(stmt, 0);
    int resource = sqlite3_column_int(stmt, 1);
    sqlite3_finalize(stmt);
    return (total + new_duration <= resource);
}

// Вставка рейса с проверкой
int insert_flight(Flight *f) {
    if (!check_flight_resource(f->helicopter_id, f->duration)) {
        fprintf(stderr, "Flight would exceed resource limit!\n");
        return 0;
    }
    const char *sql = "INSERT INTO flights (flight_date, helicopter_id, flight_code, "
                      "cargo_mass, passenger_count, duration, cost) "
                      "VALUES (?,?,?,?,?,?,?)";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return 0;
    sqlite3_bind_text(stmt, 1, f->flight_date, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, f->helicopter_id);
    sqlite3_bind_text(stmt, 3, f->flight_code, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 4, f->cargo_mass);
    sqlite3_bind_int(stmt, 5, f->passenger_count);
    sqlite3_bind_int(stmt, 6, f->duration);
    sqlite3_bind_double(stmt, 7, f->cost);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}