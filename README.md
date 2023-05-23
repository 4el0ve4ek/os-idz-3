# Аксенов Даниил Алексеевич | БПИ218 | ИДЗ-3 | Вариант 15
## Условие задачи
На клумбе растет 40 цветов, за ними непрерывно следят два процесса–садовника и поливают увядшие цветы, при этом оба садовника очень боятся полить один и тот же цветок, который еще не начал вянуть. Создать приложение, моделирующее состояния цветков на клумбе и действия садовников. Сервер используется для изменения состояния цве- тов, каждый садовник — отдельный клиент.

## Решение
Две сущности:
- Садовник (клиент) x 2
- Клумба (сервер)



  Функционал Клумбы:
-  Хранит информацию о состоянии всех 40 цветов.
-  С какой-то случайностью цветы "вянут" и их надо полить садовнику
-  Принимают запрос в котором лишь одно число - номер цветка который надо полить. Возвращает 0 если успешно полили и 1 иначе
-  Два садовника не могут полить один цветок, тк при проверке цветка берется мьютекс (при увядании тоже берется мьютекс)

  Функционал Садовника:
- Делает запросы на сервер про какой-то цветок
- Если сервер на запрос вернул 0 --значит садовник успешно полил цветок и сообщает об этом в логах

**Остановка** программы осуществляется посредством нажатия комбинации клавиш `Ctrl+C` в каждом из терминалов. Предпочтительно начинать с терминала Клиента.
### Компиляция
```sh
gcc client.c -o client
gcc server.c -o server -lpthread
```
### Запуск
**Начинать** запуск следует с сервера.
1. Клумба
```sh
sudo ./server <server port>
```

2. Садовник
```sh
sudo ./client <server ip> <server port> <порядковый номер садовника>
```
### Пример работы
1. Запуск Клумбы
```sh
sudo ./server 8080
```
2. Запуск Садовника
```sh
sudo ./client 127.0.0.1 8080 1
```

Логи Клумбы:
```
Server listening on port 8080...
All flowers are initially watered
New client connected.                 <- первый клиент подключился
17 flower now need water                  <- полил первый, тк второго не было
19 flower now need water                  <-
New client connected.                 <- второй клиент подключился
1 flower now need water                   <- первый
10 flower now need water                  <- второй
27 flower now need water                  <- первый
22 flower now need water                  <- второй
19 flower now need water                  <- второй
39 flower now need water                  <- первый
9 flower now need water
35 flower now need water                  ... и тд
26 flower now need water                 
19 flower now need water
9 flower now need water
25 flower now need water
6 flower now need water
26 flower now need water
7 flower now need water
23 flower now need water
22 flower now need water
7 flower now need water
28 flower now need water
1 flower now need water
9 flower now need water
14 flower now need water
26 flower now need water
25 flower now need water
3 flower now need water
30 flower now need water
10 flower now need water
```

Логи Садовника 1:
```
Connected to the server.
1 watered the 17 flower
1 watered the 19 flower
1 watered the 1 flower
1 watered the 27 flower
1 watered the 39 flower
1 watered the 9 flower
1 watered the 26 flower
1 watered the 9 flower
1 watered the 6 flower
1 watered the 26 flower
1 watered the 7 flower
1 watered the 23 flower
1 watered the 22 flower
1 watered the 26 flower
1 watered the 10 flower
```

Логи Садовника 2:
```
Connected to the server.
2 watered the 10 flower
2 watered the 22 flower
2 watered the 19 flower
2 watered the 35 flower
2 watered the 19 flower
2 watered the 25 flower
2 watered the 7 flower
2 watered the 28 flower
2 watered the 1 flower
2 watered the 9 flower
2 watered the 14 flower
2 watered the 25 flower
2 watered the 3 flower
2 watered the 30 flower
```