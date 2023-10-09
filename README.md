# Framework for register app API in DBUS.

### Requirements (packages):
- dbus-dev
- cmake

### Сервис, который даёт возможность:
- регистрировать файлы для обработки и способ вызова приложерия
- клиенту запускать приложения для открытия файлов

## Тестовое приложение sender (обёртка над фреймворком)

### Сборка
`./build.sh`

На данный момент под капотом запускает сервер, хранящий имена программ и параметров. У него нет кеша на диске, для восстановления старых записей. Получаем server PID, с помощью которого в будущем можно убить сервер.

### Зарегистрировать программу для работы с файлом:
`./bin/sender test.txt 'vim -v'`

Здесь `test.txt` -- имя файла, `'vim -v'` -- программа для его открытия.

### Открыть файл:
`./bin/sender open test.txt`

#### Произойдет вызов
`vim -v test.txt`