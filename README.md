# Practice
1) Запустить приложение клиента и сервер
2) Ввести в поле число и нажать на кнопку

<p>log.txt - log файл пол умолчанию
<p>settings.ini - файл с настройками
<p>Код написан в системе Ubuntu 22.04.1 LTS
<p>Для работы необходимы библиотеки: rabbitmq, protobuf

<h2> Схема взаимодействия между клиентом и сервером: </h2>
<p>https://www.websequencediagrams.com/cgi-bin/cdraw?lz=dGl0bGUg0KHRhdC10LzQsCDQstC30LDQuNC80L7QtNC10LnRgdGC0LLQuNGPINC80LXQttC00YMg0LrQu9C40LXQvdGC0L7QvCDQuCDRgdC10YDQsgABBb7QvAoKQ2xpZW50LT5RdWV1ZTogUmVxdWVzdFxubV9yZXBseV90b19xdWV1ZVxuY29ycmVsYXRpb25faWQKACwFLT5TZXJ2ZXI6CgACBi0-YW1xLmdlbjpSZXBseVxuAD0IAC4RACEHLT4AewY6Cg&s=default</p>

<h2>Тип exchange, который был выбран </h2>
<p>direct, так как он записывает сообщение в очередь, если совпадают ключи маршрутизации сообщения и очереди. Таким образом, сообщение попадет в нужную очередь, откуда впоследствии перейдет на сервер.</p>

<h2>Кто и в каком порядке создает exchange/queue</h2>
<p>Клиенты и сервер создают свои очереди. Сервер привязывается к очереди с параметрами exchange и bindingkey. Клиенты при отправке сообщения указывают параметры exchange и routingkey. При совпадении exchange и bindingkey с routingkey сообщение попадает в очередь сервера.</p>
