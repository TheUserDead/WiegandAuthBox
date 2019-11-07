# WiegandAuthBox
## EN
Wiegand authenticator for 250 users. Based on adapted RC522 rfid reader example sketch and little extended.

### Features:
+ 250 Users if you use ATMEGA328 (Less if ATMEGA168 cause it's has 1k EEPROM)
+ Master UID it's first scanned code
+ Emergency button for clear all user with master id (USE AT YOUR OWN RISK! After cleaning user records, the master card will not work! only complete eeprom cleaning! The device will deny access to EVERYONE, read between strings --LOCKDOWN--)
+ Use 2 color led ^_^

### Used external libs
[Wiegand library] (https://github.com/monkeyboard/Wiegand-Protocol-Library-for-Arduino)
EEPROM (Internal Library)
~~RCSwitch (Testing extension to broadcast received number, for other devices like light enable in employee office)~~

### Little story
The device is created for other autonomic Attendance&access controller ZD2F20.
It's used for attendance but device open door relay for every scan. I need open other door just only for special users.
ZD2F20 has WG interface where i can read scanned fingerprint/card and define access to something like storage. Others just register their attendance. It's userful if attendance scanner standalone can't be used for register time and access to storage places at the same time for different users.
Or It's just wiegand controller, and you need just WG reader externally installed outside.
**(Wiring schematic add ASAP)**

## RUS
Wiegand авторизатор на 250 пользователей. Основан на адаптированном скетче-примере для модуля RC522 и немного доработан.

### Возможности
+ 250 пользователей как и сообщалось, если используете ATMEGA328 (Меньше если ATMEGA168 так как у нее 1к EEPROM)
+ Мастер карта - первая сканированная карта. Каждый ее скан, вход или выход в режим программирования.
+ Аварийная кнопка "отторжение" которая очищает все UID записи в EEPROM включая Мастер-карту. (ИСПОЛЬЗОВАТЬ ТОЛЬКО НА СВОЙ СТРАХ И РИСК, После очистки памяти устройство войтет в режим самоблокировки, это значит что ни ранее зарегистрированные пользователи ни мастер карта не откроют дверь и не введут устройство в режим программирования. Следовательно никто не войдет. Усли замок не предусматривает ключ, у вас возникнут проблемы. Даже после перезагрузки, устройство будет отвергать все карты\коды\сканы!)
+ Для индикации используется двухцветный индикатор ^_^

### Используемые внешние библиотеки
[Wiegand] (https://github.com/monkeyboard/Wiegand-Protocol-Library-for-Arduino) Протокол для Arduino
EEPROM (Встроенная библиотека)
~~RCSwitch (Тестирование разширения в котором устройство вещает полученный ID для автоматиацииб например для включения света в офисе сотрудника) ~~

### Краткая история
Данная коробочка достопа была создана с целью доработать функционал устройства учета рабочего времени (ZD2F20) рядом с которым была дверь в техническое помещение. 
При сканировании внутреннее реле отрабатывало всем зарегистрированным пользователям что не удобно, так как вход в основное помещение свободно, а в техническое должно быть только особым ID.
Так как ZD2F20 имеет интерфейс Wiegand, было разработанно устройство которое считывало текущий ID по Wiegand от устройства сканирования сверяло со своей базой пользователей и при совпадении открывало дверь в тех помещение. Возможно наработка вам пригодится.
Данное устройство является самостаятельным и законченным. Его можно использовать как контроллер доступа, просто установив wiegand считыватель снаружи. Так же можно доработать под свои нужды (Arduino, open source же!) 
**(Схема подключения будет загружена по возможности)**



