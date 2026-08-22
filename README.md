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
│       ├── Logger.h
│       └── CompositeSink.h (опционально)
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
   
Находясь в папке `/build` введите `./src/tests/logger_tests` для запуска юнит-теста.
   
<img width="672" height="98" alt="{232ED630-9207-4DD9-BE3B-A37857BF55A6}" src="https://github.com/user-attachments/assets/0006058a-a847-4692-a3fc-afbc97e20f21" />

## Консольное приложение `logger_app`
