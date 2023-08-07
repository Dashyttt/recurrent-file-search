# Рекурсивный поиск файлов, начиная с указанного каталога

## Цель работы: 

Разработать на языке C для ОС Linux программу, позволяющую выполнять рекурсивный поиск файлов, начиная с указанного каталога, в соответствии с определенными условиями.

## Описание: 

Программа выполняет обход заданного каталога и выводит информацию о каждом элементе. Кроме того, программа выполняет поиск заданной последовательности в кодировке UTF-8 в содержимом каждого файла, встречающегося в каталоге, и выводит результаты поиска (количество вхождений и позиции начала и конца последовательности).

## В коде определены следующие функции:

- print_entry - функция, которая выводит информацию о каждом элементе каталога;
- walk_dir_impl - функция, которая рекурсивно обходит каталог;
- walk_dir - функция, которая вызывает walk_dir_impl для заданного каталога и выводит информацию о каталоге;
- search_sequence - функция, которая выполняет поиск заданной последовательности байт в содержимом файла.

## Программа принимает три аргумента командной строки: 

Имя файла, путь к каталогу, который нужно обойти, и последовательность, которую нужно найти в файлах. 

## Также программа позволяет использовать следующие опции:

- --help (-h) - выводит справочную информацию о программе;
- --version (-v) - выводит информацию о версии программы.

Если включена переменная окружения LAB11DEBUG, то программа будет выводить дополнительную отладочную информацию в стандартный вывод (начало и конец вхождений последовательности).