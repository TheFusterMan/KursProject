import random
import datetime
import os

# --- НАСТРОЙКИ ГЕНЕРАЦИИ ---
NUM_CLIENTS = 5       # Сколько клиентов сгенерировать
NUM_LAWYERS = 10        # Сколько юристов (для разнообразия в консультациях)
NUM_CONSULTATIONS = 5 # Сколько консультаций сгенерировать

CLIENTS_FILENAME = "clients.txt"
CONSULTATIONS_FILENAME = "consultations.txt"
# --- КОНЕЦ НАСТРОЕК ---


# --- СПРАВОЧНИКИ ДАННЫХ (можно расширить) ---

# Мужские фамилии
SURNAMES_M = [
    "Иванов", "Петров", "Сидоров", "Смирнов", "Кузнецов", "Попов", "Васильев",
    "Михайлов", "Новиков", "Фёдоров", "Морозов", "Волков", "Алексеев", "Лебедев"
]
# Мужские имена
NAMES_M = [
    "Александр", "Дмитрий", "Максим", "Сергей", "Андрей", "Алексей", "Артём",
    "Илья", "Кирилл", "Михаил", "Никита", "Матвей", "Роман", "Егор"
]
# Мужские отчества
PATRONYMICS_M = [
    "Александрович", "Дмитриевич", "Сергеевич", "Андреевич", "Алексеевич", "Иванович",
    "Михайлович", "Владимирович", "Николаевич", "Викторович", "Петрович", "Павлович"
]

# Темы консультаций
TOPICS = [
    "Семейное право", "Наследственные споры", "Жилищное право", "Защита прав потребителей",
    "Трудовые споры", "Сопровождение сделок с недвижимостью", "Автомобильное право (ДТП)",
    "Корпоративное право", "Банкротство физических лиц", "Налоговые консультации"
]


# --- ФУНКЦИИ-ГЕНЕРАТОРЫ ---

def generate_fio():
    """Генерирует ФИО в формате 'Фамилия Имя Отчество'."""
    f = random.choice(SURNAMES_M)
    i = random.choice(NAMES_M)
    o = random.choice(PATRONYMICS_M)
    return f"{f} {i} {o}"

def generate_inn(existing_inns: set):
    """Генерирует уникальный 12-значный ИНН."""
    while True:
        inn = str(random.randint(10**11, (10**12) - 1))
        if inn not in existing_inns:
            existing_inns.add(inn)
            return inn

def generate_phone():
    """Генерирует телефон в формате '89XXXXXXXXX'."""
    return f"89{random.randint(10**8, (10**9) - 1)}"

def generate_date():
    """Генерирует дату в формате 'ДД.ММ.ГГГГ' в допустимом диапазоне."""
    start_date = datetime.date(2020, 1, 1)
    end_date = datetime.date(2024, 5, 31)
    
    time_between_dates = end_date - start_date
    days_between_dates = time_between_dates.days
    random_number_of_days = random.randrange(days_between_dates)
    
    random_date = start_date + datetime.timedelta(days=random_number_of_days)
    return random_date.strftime("%d.%m.%Y")


# --- ОСНОВНАЯ ЛОГИКА ---

def main():
    """Главная функция для генерации и сохранения данных."""
    print("Начинаю генерацию справочников...")

    # 1. Генерируем юристов (просто список ФИО)
    lawyers_fio = [generate_fio() for _ in range(NUM_LAWYERS)]
    print(f"Сгенерировано {len(lawyers_fio)} юристов.")

    # 2. Генерируем клиентов
    generated_inns = set()
    clients = []
    for _ in range(NUM_CLIENTS):
        client = {
            "inn": generate_inn(generated_inns),
            "fio": generate_fio(),
            "phone": generate_phone()
        }
        clients.append(client)
    
    # Получаем список ИНН для использования в консультациях
    client_inns = [c["inn"] for c in clients]
    print(f"Сгенерировано {len(clients)} клиентов.")
    
    # 3. Сохраняем клиентов в файл
    try:
        with open(CLIENTS_FILENAME, "w", encoding="utf-8") as f:
            for client in clients:
                line = f"{client['inn']};{client['fio']};{client['phone']}\n"
                f.write(line)
        print(f"Файл '{CLIENTS_FILENAME}' успешно сохранен.")
    except IOError as e:
        print(f"Ошибка при записи файла '{CLIENTS_FILENAME}': {e}")
        return

    # 4. Генерируем консультации
    consultations = []
    if not client_inns:
        print("Нет клиентов для создания консультаций. Пропускаю...")
    else:
        for _ in range(NUM_CONSULTATIONS):
            consultation = {
                "client_inn": random.choice(client_inns), # <-- Ключевой момент!
                "topic": random.choice(TOPICS),
                "lawyer_fio": random.choice(lawyers_fio),
                "date": generate_date()
            }
            consultations.append(consultation)
        print(f"Сгенерировано {len(consultations)} консультаций.")

    # 5. Сохраняем консультации в файл
    try:
        with open(CONSULTATIONS_FILENAME, "w", encoding="utf-8") as f:
            for cons in consultations:
                line = f"{cons['client_inn']};{cons['topic']};{cons['lawyer_fio']};{cons['date']}\n"
                f.write(line)
        print(f"Файл '{CONSULTATIONS_FILENAME}' успешно сохранен.")
    except IOError as e:
        print(f"Ошибка при записи файла '{CONSULTATIONS_FILENAME}': {e}")

    print("\nГенерация завершена.")
    print(f"Созданы файлы: '{os.path.abspath(CLIENTS_FILENAME)}' и '{os.path.abspath(CONSULTATIONS_FILENAME)}'")


if __name__ == "__main__":
    main()