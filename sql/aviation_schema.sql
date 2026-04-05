-- =============================================
-- База данных: Воздушный извозчик
-- =============================================

-- Включение поддержки внешних ключей
PRAGMA foreign_keys = ON;

-- -----------------------------------------------------
-- Таблица: helicopters
-- -----------------------------------------------------
CREATE TABLE helicopters (
    helicopter_id INTEGER PRIMARY KEY AUTOINCREMENT,
    serial_number TEXT NOT NULL UNIQUE,
    model TEXT NOT NULL,
    manufacture_date TEXT NOT NULL,
    capacity REAL NOT NULL,
    last_overhaul TEXT NOT NULL,
    flight_resource INTEGER NOT NULL
);

-- -----------------------------------------------------
-- Таблица: crew
-- -----------------------------------------------------
CREATE TABLE crew (
    crew_id INTEGER PRIMARY KEY AUTOINCREMENT,
    personnel_number TEXT NOT NULL UNIQUE,
    full_name TEXT NOT NULL,
    position TEXT NOT NULL,
    experience INTEGER NOT NULL,
    address TEXT NOT NULL,
    birth_year INTEGER NOT NULL,
    helicopter_id INTEGER NOT NULL,
    FOREIGN KEY (helicopter_id) REFERENCES helicopters(helicopter_id)
);

-- -----------------------------------------------------
-- Таблица: flights
-- -----------------------------------------------------
CREATE TABLE flights (
    flight_id INTEGER PRIMARY KEY AUTOINCREMENT,
    flight_date TEXT NOT NULL,
    helicopter_id INTEGER NOT NULL,
    flight_code TEXT NOT NULL CHECK (flight_code IN ('обычный', 'спец')),
    cargo_mass REAL NOT NULL,
    passenger_count INTEGER NOT NULL,
    duration INTEGER NOT NULL,
    cost REAL NOT NULL,
    FOREIGN KEY (helicopter_id) REFERENCES helicopters(helicopter_id)
);

-- -----------------------------------------------------
-- Таблица: salary_results
-- -----------------------------------------------------
CREATE TABLE salary_results (
    result_id INTEGER PRIMARY KEY AUTOINCREMENT,
    period_start TEXT NOT NULL,
    period_end TEXT NOT NULL,
    total_amount REAL NOT NULL
);

-- -----------------------------------------------------
-- Таблица: app_users
-- -----------------------------------------------------
CREATE TABLE app_users (
    user_id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT NOT NULL UNIQUE,
    password_hash TEXT NOT NULL,
    role TEXT NOT NULL CHECK (role IN ('COMMANDER', 'CREW')),
    crew_id INTEGER NULL,
    FOREIGN KEY (crew_id) REFERENCES crew(crew_id)
);

-- -----------------------------------------------------
-- Таблица: app_config
-- -----------------------------------------------------
CREATE TABLE app_config (
    config_key TEXT PRIMARY KEY,
    config_value TEXT NOT NULL
);

-- -----------------------------------------------------
-- Начальные конфигурационные данные
-- -----------------------------------------------------
INSERT INTO app_config (config_key, config_value) VALUES
('regular_rate', '0.05'),
('special_rate', '0.10'),
('max_attempts', '3'),
('lockout_minutes', '5');

-- -----------------------------------------------------
-- Тестовые данные
-- -----------------------------------------------------

-- Вертолёты
INSERT INTO helicopters (serial_number, model, manufacture_date, capacity, last_overhaul, flight_resource) VALUES
('RA-12345', 'Ми-8', '2010-06-15', 4000, '2023-01-10', 500),
('RA-67890', 'Ми-26', '2015-09-20', 20000, '2024-02-01', 800),
('RA-11111', 'Ка-32', '2018-11-05', 5000, '2024-03-15', 600);

-- Экипаж
INSERT INTO crew (personnel_number, full_name, position, experience, address, birth_year, helicopter_id) VALUES
('P001', 'Иванов Иван Иванович', 'Командир', 15, 'ул. Ленина 1', 1980, 1),
('P002', 'Петров Пётр Петрович', 'Второй пилот', 10, 'ул. Ленина 2', 1985, 1),
('P003', 'Сидоров Сидор Сидорович', 'Бортмеханик', 12, 'ул. Ленина 3', 1983, 1),
('P004', 'Смирнов Алексей Владимирович', 'Командир', 20, 'ул. Гагарина 5', 1975, 2),
('P005', 'Кузнецов Дмитрий Сергеевич', 'Второй пилот', 8, 'ул. Гагарина 6', 1990, 2),
('P006', 'Попов Андрей Викторович', 'Бортмеханик', 9, 'ул. Гагарина 7', 1988, 2),
('P007', 'Васильев Олег Иванович', 'Командир', 7, 'ул. Мира 10', 1992, 3),
('P008', 'Зайцев Илья Петрович', 'Второй пилот', 5, 'ул. Мира 11', 1995, 3),
('P009', 'Соколов Максим Андреевич', 'Бортмеханик', 6, 'ул. Мира 12', 1994, 3);

-- Рейсы
INSERT INTO flights (flight_date, helicopter_id, flight_code, cargo_mass, passenger_count, duration, cost) VALUES
('2025-01-10', 1, 'обычный', 2000, 0, 3, 50000),
('2025-01-15', 1, 'спец', 1500, 5, 4, 80000),
('2025-02-01', 2, 'обычный', 10000, 2, 6, 150000),
('2025-02-10', 2, 'спец', 8000, 10, 5, 200000),
('2025-02-20', 1, 'обычный', 2500, 0, 2, 45000),
('2025-03-01', 3, 'обычный', 3000, 3, 4, 70000),
('2025-03-05', 3, 'спец', 2000, 8, 3, 90000),
('2025-03-10', 2, 'обычный', 12000, 0, 7, 180000),
('2025-03-15', 1, 'спец', 1800, 6, 4, 85000);

-- Пользователи приложения
INSERT INTO app_users (username, password_hash, role, crew_id) VALUES
('commander', 'hash1', 'COMMANDER', NULL),
('ivanov', 'hash2', 'CREW', 1),
('petrov', 'hash3', 'CREW', 2),
('sidorov', 'hash4', 'CREW', 3),
('smirnov', 'hash5', 'CREW', 4);
