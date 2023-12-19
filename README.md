# URLConnect
Программа для запуска программ и команд на компьютере пользователя из веб интерфейса Zabbix. 

## Функциональность ПО

Основные функции программы:
* Получение команд из веб-браузера
* Запуск программ и команд на компьютере пользователя
* Передача логина и пароля для авторизации на устройстве куда происходит подключение
* Интеграция с Passwork (https://passwork.ru)
* Наличие графического интерфейса для настройки программы

URLConnect поддерживает следующие команды для запуска:
* ssh - Запуск клиента ssh (PuTTY или MobaXTerm)
* rdp - Запуск стандартного клиента RDP Windows
* vnc - Запуск клиента vnc (Протестирована работа только с клиентом TightVNC)
* winbox - Запуск winbox'а
* old_winbox - Запуск старой версии winbox'а (путь указывается в настройках)
* ie - Запуск Internet Explorer через скрипт для доступа к старым сервисам поддерживающим только этот браузер
* telnet - Запуск клиента telnet (PuTTY или MobaXTerm)
* ping, traceroute - Запуск соответствующих утилит в командной строке

## Принцип работы ПО
Программа состоит из 2-х частей: клиента-обработчика URL и сервера-основной части программы. Клиент обрабатывает запросы из браузера по протоколу connect://. URL должен содержать обязательно ip адрес или доменное имя устройства и команду, при необходимости также нужно добавить ID пароля из Passwork, в качестве разделителя используется символ плюс «+».
Пример корректных URL:
```
connect://192.168.0.1+ssh+46586g96hi7hk79
connect://192.168.0.2+ping
```
Путь к клиенту прописывается в реестре, по пути HKEY_CLASSES_ROOT\connect\shell\open\command
После получения запроса из браузера запускается клиент, разбирает URL на 3 части (адрес хоста, команда и id пароля если есть) и отправляет через именованный канал серверу, после чего завершает свою работу, если от сервера было получено сообщение о получении данных.
![Диаграмма процесса передачи команды от веб браузера в сервер URLConnect](https://github.com/NorthBridgeKholmsk/URLConnect/assets/109929249/d99a2bf6-aa14-43a9-a4cd-5b9c84961c36)

Сервер представляет из себя ПО, обрабатывающее данные от клиента и запускающее соответствующую программу. После получения данных от клиента, сервер в зависимости от полученной команды от клиента (2 блок данных) получает данные из Passwork по API, соответствующие полученному id пароля (3 блок данных), после чего запускает через командную строку соответствующие программы, например:
```
URL:
connect://192.168.0.1+ping
Клиент передаёт серверу:
1 блок: 192.168.0.1, 2 блок: ping
Сервер запускает в cmd:
ping 192.168.0.1 -t
```
![Диаграмма процесса выполнения команды сервером URLConnect](https://github.com/NorthBridgeKholmsk/URLConnect/assets/109929249/9db937e1-f12a-4601-b641-fcb09027f473)

Для настройки и диагностики сервера, в системном трее есть значок программы, клик ПКМ по нему вызывает контекстное меню со следующими опциями: настройки, обновление ПО, логи и выход. 
В меню настроек есть возможность включить автозапуск, добавить ключ API Passwork, выбрать программы для запуска, а также указать пути к их исполняемым файлам (.exe). 
![image](https://github.com/NorthBridgeKholmsk/URLConnect/assets/109929249/3c7d2895-536e-4bbc-8d4d-65715a524c3f)

Все настройки записываются в реестр по пути HKEY_CURRENT_USER\SOFTWARE\Северный мост\URLConnect\settings
Наличие обновления ПО происходит при каждом запуске сервера URLConnect или при нажатии на соответствующий пункт меню в трее. Обновление проходит следующим образом:
1. Сервер читает файл version в репозитории GitHub, в котором прописана последняя стабильная версия ПО.
2. Если текущая версия ПО отличается от версии в файле, то сервер предлагает скачать и обновится до версии из файла.
3. Если пользователь соглашается, то сервер скачивает новую версию, запускает установщик новой версии и завершает работу.
4. Пользователь завершает установку новой версии.
В разделе логов можно прочитать файл логов, содержащий отчеты по каждому важному этапу работы сервера и клиента, а также найти описания возникших ошибок.
![image](https://github.com/NorthBridgeKholmsk/URLConnect/assets/109929249/ac80c2ab-6553-4b15-b28d-226b264615eb)



