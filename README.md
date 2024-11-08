# Сервис для получения данных по API Elsevier
Является [фрагментом](https://github.com/AlexanderMeynik/dataAnalyticProject/blob/master/documentation/Architecture.md#3-%D1%8D%D1%82%D0%B0%D0%BF)
процедуры запроса данных для [проекта](https://github.com/AlexanderMeynik/dataAnalyticProject) аналитики данных с платформы Science Direct.

## Cтек технологий
IDE: Clion 2023.2
- Язык программирования: С++ 17
- Система сборки: cmake-3.20
- Стандарт языка: 17
- Установленные библиотеки:
  - libopenmp v5.0
  - libpqxx v7.8.1
  - libcpr v1.10.2
  - pugixml v1.14

## Описание приобретённых навыков и ограничений
При разработке данной процедуры были обнаружены следующие ограничения:
- Удалённое(относительно оборудования пользователя) расположение серверов приводило к задержкам(>500 мс  при объёме данных ~20 кб), по длительности занимающим больше времени, чем сама обработка содержимого 
- Ограничение на частоту отправки запросов для 1 API ключа.
- Разнородность данных, большое число записей с пустыми или полупустыми полями.

Для достижения оптимальной пропускной способности было принято решение использовать асинхронный http клиент
 для сохранения набора данных в массив из future.
С целью обойти ограничение на число запросов был создан набор API ключей(16) и каждый запускаемый поток имел свой уникальный API ключ.


Проводимые деятельности:
1. Были применены асинхронные http клиенты для непрерывного запроса информации и его сохранения в массив данных для обработки.
2. Были применены параллельные циклы реализованные на базе OpenMP.
3. Для динамического отображения прогресса исполнения в терминале были отрисованы индикаторы.
4. Для обхода существующего ограничения был создан набор api ключей.
5. Данные приводились к общему типу для унификации и проведения общих проверок.
6. Удалось разработать процедуру для получения необходимых данных из xml с различными вариантами его содержимого.
