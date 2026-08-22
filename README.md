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
## Приложение статистики stats_app (дополнительно)
Сервер, который принимает логи по TCP, выводит принятые сообщения и собирает статистику.<br>
Запуск:<br>
```bash
./stats_app <port> <N> <T>
```
port – порт для прослушивания<br>
N – выводить статистику после каждых N сообщений<br>
T – выводить статистику каждые T секунд, если она изменилась с момента последнего вывода<br>
Статистика:
- Общее количество сообщений
- Количество по уровням (DEBUG, INFO, ERROR)
- Количество сообщений за последний час
- Минимальная, максимальная и средняя длина сообщения

### Файловый режим
Находясь в папке `/build` введите `./src/app/logger_app log.txt INFO` для запуска файлового режима.<br>
<img width="988" height="215" alt="{FE095C2B-321A-411A-B026-E0DCDC13DD46}" src="https://github.com/user-attachments/assets/0f1fc1da-2a2c-48a4-b532-34aa762184e3" />
   
<img width="474" height="162" alt="{617D254E-E87E-4011-8D97-612738711645}" src="https://github.com/user-attachments/assets/5f930249-c859-4ffa-b33a-556952bd712c" />

### Сокетный режим
Здесь нужно терминала, в 1 терминале будет статистика, во 2 терминале сам сокетный режим в котором будут сообщения.<br>
Находясь в папке `/build` введите `./src/stats_app/stats_app 8080 3 5` для запуска статистики.<br>
Находясь в папке `/build` введите `./src/app/logger_app --socket 127.0.0.1 8080 DEBUG` для запуска файлового режима.<br>

<img width="913" height="154" alt="{1088785D-D50D-4158-906B-4DF83579EA7D}" src="https://github.com/user-attachments/assets/c0720015-11c2-4985-aade-ec13c1445d10" />
   
<img width="958" height="477" alt="{D617743A-19E5-4326-8F1D-BD5B3720C7A8}" src="https://github.com/user-attachments/assets/306fb28b-4423-415e-96d0-f3df87736ab1" />
   
<img width="608" height="240" alt="{AFB28BBA-9F38-44DB-8C18-13F270BE5E25}" src="https://github.com/user-attachments/assets/55deffb7-7918-48dd-9e0a-7ed42aa877b1" />

## Дополнительно
Проверка портов если заняты `sudo lsof -i :<port>` затем можно `sudo kill -9 <PID>` чтобы освободить порт или выбрать другой.<br>
Автор: l-Monarch-l
