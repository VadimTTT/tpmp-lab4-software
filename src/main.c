#include "../includes/database.h"
#include <stdio.h>

int main() {
    if (init_db("aviation.db") != SQLITE_OK) {
        fprintf(stderr, "Can't open database\n");
        return 1;
    }

    char username[50], password[50];
    int is_commander, crew_id;

    printf("=== Aviation System ===\n");
    printf("Username: "); scanf("%s", username);
    printf("Password: "); scanf("%s", password);

    if (!authenticate(username, password, &is_commander, &crew_id)) {
        printf("Authentication failed.\n");
        close_db();
        return 1;
    }

    printf("Welcome, %s!\n", username);

    int choice;
    do {
        printf("\n1. Flight hours and resource\n");
        printf("2. Helicopter flights (period)\n");
        printf("3. Special flights stats\n");
        printf("4. Regular flights stats\n");
        printf("5. Helicopter with max flights\n");
        printf("6. Crew with max earnings\n");
        if (!is_commander) {
            printf("7. My flights\n");
        } else {
            printf("7. Insert flight (with resource check)\n");
            printf("8. Calculate crew salary (period)\n");
            printf("9. Calculate my salary (period)\n");
            printf("10. Calculate my salary for flight\n");
        }
        printf("0. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: select_flight_hours_and_resource(); break;
            case 2: {
                int hid; char start[11], end[11];
                printf("Helicopter ID: "); scanf("%d", &hid);
                printf("Start date (YYYY-MM-DD): "); scanf("%s", start);
                printf("End date: "); scanf("%s", end);
                select_flights_by_helicopter_and_period(hid, start, end);
                break;
            }
            case 3: select_special_flights_stats(); break;
            case 4: select_regular_flights_stats(); break;
            case 5: select_helicopter_max_flights(); break;
            case 6: select_crew_max_earnings(); break;
            case 7:
                if (!is_commander) {
                    select_flights_by_crew_member(crew_id);
                } else {
                    Flight f;
                    printf("Date, heli_id, code, cargo, passengers, duration, cost: ");
                    scanf("%s %d %s %lf %d %d %lf", f.flight_date, &f.helicopter_id,
                          f.flight_code, &f.cargo_mass, &f.passenger_count,
                          &f.duration, &f.cost);
                    insert_flight(&f);
                }
                break;
            case 8:
                if (is_commander) {
                    char s[11], e[11];
                    printf("Period (start end): "); scanf("%s %s", s, e);
                    double total = calculate_crew_salary_period(s, e);
                    printf("Total crew salary: %.2f\n", total);
                }
                break;
            case 9:
                if (is_commander) {
                    char s[11], e[11];
                    printf("Period (start end): "); scanf("%s %s", s, e);
                    double my = calculate_crew_member_salary_period(crew_id, s, e);
                    printf("Your salary: %.2f\n", my);
                }
                break;
            case 10:
                if (is_commander) {
                    int fid;
                    printf("Flight ID: "); scanf("%d", &fid);
                    double f_sal = calculate_crew_member_salary_for_flight(crew_id, fid);
                    printf("Salary for this flight: %.2f\n", f_sal);
                }
                break;
        }
    } while (choice != 0);

    close_db();
    return 0;
}