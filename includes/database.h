#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Структуры
typedef struct {
    int id;
    char serial_number[20];
    char model[50];
    char manufacture_date[11];
    double capacity;
    char last_overhaul[11];
    int flight_resource;
    char photo_path[256];
} Helicopter;

typedef struct {
    int id;
    char personnel_number[20];
    char full_name[100];
    char position[50];
    int experience;
    char address[200];
    int birth_year;
    int helicopter_id;
} Crew;

typedef struct {
    int id;
    char flight_date[11];
    int helicopter_id;
    char flight_code[10];
    double cargo_mass;
    int passenger_count;
    int duration;
    double cost;
} Flight;

// Глобальный дескриптор БД
extern sqlite3 *db;

// Инициализация и закрытие
int init_db(const char *db_path);
void close_db();

// Аутентификация
int authenticate(const char *username, const char *password, int *is_commander, int *crew_id);

// SELECT запросы (задачи 3‑9)
void select_flight_hours_and_resource();
void select_flights_by_helicopter_and_period(int heli_id, const char *start, const char *end);
void select_special_flights_stats();
void select_regular_flights_stats();
void select_helicopter_max_flights();
void select_crew_max_earnings();
void select_flights_by_crew_member(int crew_id);

// Триггер проверки ресурса (логика на уровне C)
int check_flight_resource(int helicopter_id, int new_duration);

// Вставка рейса (с проверкой)
int insert_flight(Flight *f);

// Функции начисления (задачи 11‑13)
double calculate_crew_salary_period(const char *start, const char *end);
double calculate_crew_member_salary_period(int crew_id, const char *start, const char *end);
double calculate_crew_member_salary_for_flight(int crew_id, int flight_id);

// CRUD операции (задача 14)
int insert_helicopter(Helicopter *h);
int update_helicopter(Helicopter *h);
int delete_helicopter(int id);
// аналогично для crew и flights (можно добавить при необходимости)

// Работа с изображениями
int save_helicopter_photo(int heli_id, const char *image_path);
int load_helicopter_photo(int heli_id, const char *output_path);

#endif