# Micromouse

Проект двухколесного робота-мышки, способной автономно перемещаться по лабиринту.

Цель проекта - участие в соревнованиях Micromouse и подготовка к Robotics Tournament 2019.

## Аппаратное обеспечение
* **Arduino Nano**
* **ИК датчики расстояния (3 шт.)**
  Для фильтрации шумов датчиков используются медианный фильтр и фильтр Калмана.
* **Моторы с энкодерами (2 шт.)**

## Механизмы взаимодействия с роботом
1. Тумблеры:
    * **Питания** 	
	Необходимо включать, даже если контроллер питается от USB-порта
    * **Ускорение** 	
	Предназначен для переключения между режимом исследования лабиринта и режимом заеда
    * **не задействован**
2. Кнопки:
    * не задействована
	* не задействована
	
## Зависимости проекта

Для работы проекта необходимо подключить сторонние библиотеки из Arduino IDE: **SharpDistSensor**, **SimpleKalmanFilter**.

## Работа над проектом

### Получение информации о препятствиях
Для движения робота по лабиринту и обнаружения им препятствий используются три ИК-датчика расстояния, направленные по ходу движения. 
Для преобразования напряжения на выходе датчика в расстояние до препятствия используется библиотека *SharpDistSensor*.
Фильтрация информации от датчиков осуществляется фильтром Калмана, реализованного в библиотеке *SimpleKalmanFilter*.

### Определение положения
С помощью энкодеров определяется пройденное расстояние каждым из колес. Также, энкодеры позволяют определить направление вращения колеса.

