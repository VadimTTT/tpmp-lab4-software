#include "../includes/database.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

sqlite3 *db = NULL;

int init_db(const char *db_path) {
    return sqlite3_open(db_path, &db);
}

void close_db() {
    if (db) sqlite3_close(db);
}

void select_flight_hours_and_resource() {
    const char *sql = "SELECT h.helicopter_id, h.serial_number, "
        "COALESCE(SUM(f.duration), 0) AS total, "
        "h.flight_resource - COALESCE(SUM(f.duration), 0) AS remaining "
        "FROM helicopters h LEFT JOIN flights f ON h.helicopter_id = f.helicopter_id "
        "GROUP BY h.helicopter_id;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return;
    }
    printf("\n=== Flight hours and remaining resource ===\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("Heli %d | %s | total %d h | remaining %d h\n",
            sqlite3_column_int(stmt, 0),
            sqlite3_column_text(stmt, 1),
            sqlite3_column_int(stmt, 2),
            sqlite3_column_int(stmt, 3));
    }
    sqlite3_finalize(stmt);
}

void select_flights_by_helicopter_and_period(int heli_id, const char *start, const char *end) {
    const char *sql = "SELECT flight_date, flight_code, cargo_mass, passenger_count "
        "FROM flights WHERE helicopter_id = ? AND flight_date BETWEEN ? AND ?";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, heli_id);
    sqlite3_bind_text(stmt, 2, start, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, end, -1, SQLITE_STATIC);
    printf("\n=== Flights for helicopter %d ===\n", heli_id);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("%s | %s | cargo %.0f | %d passengers\n",
            sqlite3_column_text(stmt, 0),
            sqlite3_column_text(stmt, 1),
            sqlite3_column_double(stmt, 2),
            sqlite3_column_int(stmt, 3));
    }
    sqlite3_finalize(stmt);
}

void select_special_flights_stats() {
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "SELECT COUNT(*), SUM(cargo_mass), SUM(cost) FROM flights WHERE flight_code = 'спец'", -1, &stmt, NULL);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("\n=== Special flights stats ===\nCount: %d | Total cargo: %.0f | Total earned: %.0f\n",
            sqlite3_column_int(stmt, 0),
            sqlite3_column_double(stmt, 1),
            sqlite3_column_double(stmt, 2));
    }
    sqlite3_finalize(stmt);
}

void select_regular_flights_stats() {
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "SELECT COUNT(*), SUM(cargo_mass), SUM(cost) FROM flights WHERE flight_code = 'обычный'", -1, &stmt, NULL);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("\n=== Regular flights stats ===\nCount: %d | Total cargo: %.0f | Total earned: %.0f\n",
            sqlite3_column_int(stmt, 0),
            sqlite3_column_double(stmt, 1),
            sqlite3_column_double(stmt, 2));
    }
    sqlite3_finalize(stmt);
}

void select_helicopter_max_flights() {
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "SELECT helicopter_id, COUNT(*) FROM flights GROUP BY helicopter_id ORDER BY COUNT(*) DESC LIMIT 1", -1, &stmt, NULL);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("\n=== Helicopter with max flights ===\nID: %d\n", sqlite3_column_int(stmt, 0));
    }
    sqlite3_finalize(stmt);
}

void select_crew_max_earnings() {
    printf("\n=== Crew max earnings ===\n(Requires salary calculation, not implemented yet)\n");
}

void select_flights_by_crew_member(int crew_id) {
    printf("\n=== Flights for crew member %d ===\n(Requires linking crew to flights, not implemented yet)\n", crew_id);
}

int check_flight_resource(int helicopter_id, int new_duration) {
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "SELECT COALESCE(SUM(duration),0), flight_resource FROM helicopters h "
        "LEFT JOIN flights f ON h.helicopter_id = f.helicopter_id WHERE h.helicopter_id = ? GROUP BY h.helicopter_id;", -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, helicopter_id);
    if (sqlite3_step(stmt) != SQLITE_ROW) { sqlite3_finalize(stmt); return 0; }
    int total = sqlite3_column_int(stmt, 0);
    int resource = sqlite3_column_int(stmt, 1);
    sqlite3_finalize(stmt);
    return (total + new_duration <= resource);
}

int insert_flight(Flight *f) {
    if (!check_flight_resource(f->helicopter_id, f->duration)) {
        printf("❌ Flight would exceed helicopter flight resource!\n");
        return 0;
    }
    const char *sql = "INSERT INTO flights (flight_date, helicopter_id, flight_code, cargo_mass, passenger_count, duration, cost) VALUES(?,?,?,?,?,?,?)";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, f->flight_date, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, f->helicopter_id);
    sqlite3_bind_text(stmt, 3, f->flight_code, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 4, f->cargo_mass);
    sqlite3_bind_int(stmt, 5, f->passenger_count);
    sqlite3_bind_int(stmt, 6, f->duration);
    sqlite3_bind_double(stmt, 7, f->cost);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        printf("❌ Insert flight failed\n");
        sqlite3_finalize(stmt);
        return 0;
    }
    sqlite3_finalize(stmt);
    printf("✅ Flight inserted\n");
    return 1;
}

double calculate_crew_salary_period(const char *start, const char *end) {
    printf("Calculate crew salary period: %s - %s (not fully implemented)\n", start, end);
    return 0.0;
}

double calculate_crew_member_salary_period(int crew_id, const char *start, const char *end) {
    printf("Calculate crew member %d salary period: %s - %s (not fully implemented)\n", crew_id, start, end);
    return 0.0;
}

double calculate_crew_member_salary_for_flight(int crew_id, int flight_id) {
    printf("Calculate crew member %d salary for flight %d (not fully implemented)\n", crew_id, flight_id);
    return 0.0;
}

int save_helicopter_photo(int heli_id, const char *image_path) {
    printf("Saving photo for helicopter %d from %s (not fully implemented)\n", heli_id, image_path);
    return 1;
}

int insert_helicopter(Helicopter *h) { return 0; }
int update_helicopter(Helicopter *h) { return 0; }
int delete_helicopter(int id) { return 0; }
int load_helicopter_photo(int heli_id, const char *output_path) { return 0; }