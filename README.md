# Razrabotchik-C

## Описание
Проект реализует библиотеку логирования с записью в файл и сокет, консольное многопоточное приложение для тестирования, а также дополнительное приложение для сбора статистики из сокета.

## Требования
- C++17
- CMake 3.10+
- gcc
- POSIX-система (Ubuntu/Debian)

## Структура
```text
.
├── include/
│   └── logger/
│       ├── LogLevel.h
│       ├── Sink.h
│       ├── FileSink.h
│       ├── SocketSink.h
│       └── Logger.h
├── src/
│   ├── logger/
│   │   ├── CMakeLists.txt
│   │   ├── Logger.cpp
│   │   ├── FileSink.cpp
│   │   └── SocketSink.cpp
│   ├── app/
│   │   ├── CMakeLists.txt
│   │   ├── main.cpp
│   │   └── ThreadSafeQueue.h
│   ├── tests/
│   │   ├── CMakeLists.txt
│   │   └── test_logger.cpp
│   └── stats_app/
│       ├── CMakeLists.txt
│       └── main.cpp
├── CMakeLists.txt
├── README.md
└── .gitignore
```

## Сборка
```bash
mkdir build && cd build
cmake .. -DBUILD_SHARED_LIBS=OFF   # статическая библиотека
# или -DBUILD_SHARED_LIBS=ON      # динамическая
make -j$(nproc)
make
```
   
<img width="761" height="289" alt="{520AE2F6-8032-47DD-983F-5FFC21AEA85C}" src="https://github.com/user-attachments/assets/db515768-8e60-4c15-a00a-f561574825cd" />

## Тестирование
Юнит-тесты проверяют:
- Запись в файл с фильтрацией по уровню
- Смену уровня
- Многопоточную запись
   
Находясь в папке `/build` введите `./src/tests/logger_tests` для запуска юнит-теста.<br>
<img width="672" height="98" alt="{232ED630-9207-4DD9-BE3B-A37857BF55A6}" src="https://github.com/user-attachments/assets/0006058a-a847-4692-a3fc-afbc97e20f21" />

## Консольное приложение `logger_app`
Многопоточное приложение, которое читает сообщения из консоли и передаёт их в отдельный поток для записи через очередь.<br>
Запуск:<br>
Файловый режим: ./logger_app <log_file> <DEBUG|INFO|ERROR><br>
Сокетный режим: ./logger_app --socket <ip> <port> <DEBUG|INFO|ERROR><br>
Ввод:<br>
Сообщение может начинаться с [DEBUG], [INFO] или [ERROR] – тогда используется указанный уровень.<br>
Если тега нет – используется уровень по умолчанию.<br>
Для выхода введите exit.

### Файловый режим
Находясь в папке `/build` введите `./src/app/logger_app log.txt INFO` для запуска файлового режима.<br>
<img width="918" height="154" alt="{433EE48A-5F64-4C4C-86B7-911C4917A3B2}" src="https://github.com/user-attachments/assets/9349544c-1070-4c2f-9ce0-8ec1ad88b632" />

### Сокетный режим
