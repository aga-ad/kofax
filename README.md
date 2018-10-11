# Тестовое задание для Kofax

## Сборка и запуск
* С помощью make
```
cd kofax
make
cd build
./kofax
```
* С помощью Visual Studio 2015
## Функциональность и ограничения
* Программа получает на вход
  * Арифметическое выражение, состоящее из имен переменных, 16 битных беззнаковых чисел, круглых скобок, бинарных операций +,-,*,/
  * Присваивание переменной значения. Значение является арифметическим выражением, в котором все переменные определены
  * "calculate" - считается значение последнего введенного выражения
* Если во время вычислений значение не помещается в 64 битное знаковое число, то программа сообщает об ошибке.
* При делении на ноль программа сообщает об ошибке.
* При любом другом некорректном вводе программа так же сообщает об ошибке.
## Пример работы
```
> a=11111
< Variable a has been added
> a * b
< Expression has been set
> b = 11111
< Variable b has been added
> calculate
< 123454321
> a + b + c
< Expression has been set
> calculate
< Error: Unknown variable c
> c = 0 - 22222
< Variable c has been added
> calculate
< 0

```
