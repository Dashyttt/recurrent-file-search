#include <stdlib.h>         // Объявление функций выделения памяти и управления процессом

#include <stdio.h>          // Объявление функций ввода-вывода

#include <string.h>         // Объявление библиотеки для работы со строками

#include <errno.h>          // Объявление библиотеки для работы с глобальной переменной errno

#include <sys/types.h>      // Объявление библиотеки для типов данных для системных вызовов

#include <sys/param.h>      // Объявление библиотеки для макроса PATH_MAX

#include <getopt.h>	    // Объявление библиотеки для работы с аргументами командной строки

#include <dirent.h>         // Объявление библиотеки для работы с каталогами

#define MAX_INDENT_LEVEL 128

void walk_dir(char * dir,
  const char * sequence);
void search_sequence(const char * sequence,
  const char * filename);
void f_help(void);
void f_version(void);

const char * program_name;

int main(int argc, char * argv[]) {
  int opt = 0;
  program_name = argv[0]; // получаем название программы
  static struct option long_options[] = { {"help", no_argument, 0, 'h'}, {"version", no_argument, 0, 'v'}, {0, 0, 0, 0}}; // массив структур, содержащий опции командной строки  				  									
  //для функции getopt_long
  while ((opt = getopt_long(argc, argv, "hv", long_options, NULL)) != -1) {
    switch (opt) { // обработка опций командной строки
    case 'h':
      f_help();
      exit(EXIT_SUCCESS);
    case 'v':
      f_version();
      exit(EXIT_SUCCESS);
    default:
      printf("Try '%s --help' for more information.\n", program_name);
      exit(EXIT_FAILURE);
    }
  }
  if (argc != 3) {
    printf("Usage: %s <dir>\n\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  walk_dir(argv[1], argv[2]); // Запускаем обход каталога

  return EXIT_SUCCESS; // Выход из программы с успешным кодом возврата
}

/////////////////////////////////////////////////////////////////////////////

void print_entry(int level, int type,
  const char * path,
    const char * sequence) {
  if (!strcmp(path, ".") || !strcmp(path, "..")) // Пропускаем "." и ".."
    return;

  char indent[MAX_INDENT_LEVEL] = {
    0
  }; // Строка пробелов для отступа при выводе
  memset(indent, ' ', MIN((size_t) level, MAX_INDENT_LEVEL)); // Создаем отступ, ограничивая его максимальную длину

  printf("%s[%d] %s ", indent, type, path); // Выводим отступ, тип и имя элемента
  if (type != DT_DIR)
    search_sequence(sequence, path); // запускаем поиск последовательности
  printf("\n");
}

void walk_dir_impl(int ind_level, char * dir,
  const char * sequence) {
  DIR * d = opendir(dir); // Открываем каталог
  if (d == NULL) {
    printf("Failed to opendir() %s\n", dir);
    return;
  }
  struct dirent * p;
  errno = 0;
  while ((p = readdir(d)) != NULL) {
    if (strcmp(p -> d_name, ".") && strcmp(p -> d_name, "..")) { // Пропускаем "." и ".."
      char buf2[PATH_MAX] = {
        0
      };
      sprintf(buf2, "%s/%s", dir, p -> d_name);
      print_entry(ind_level + 1, p -> d_type, buf2, sequence); // Выводим отступ, тип и имя элемента
      if (p -> d_type == DT_DIR) {
        char buf[PATH_MAX] = {
          0
        };
        sprintf(buf, "%s/%s", dir, p -> d_name); // Составляем путь к каталогу
        walk_dir_impl(ind_level + 1, buf, sequence); // Рекурсивно запускаем обход каталога
      }
    }
    errno = 0;
  }
  closedir(d);
}

void walk_dir(char * dir,
  const char * sequence) {
  print_entry(0, DT_DIR, dir, sequence); // выводим имя директории
  walk_dir_impl(0, dir, sequence);
}

void search_sequence(const char * sequence,
  const char * filename) {
  errno = 0;
  FILE * fp = fopen(filename, "rb"); // открываем файл в бинарном режиме
  if (fp == NULL) {
    printf("Failed to open file: %s\n", filename);
    return;
  }

  int seq_len = strlen(sequence);
  int count = 0; // счетчик найденных последовательностей

  fseek(fp, 0, SEEK_END); // перемещаем указатель в конец файла
  long int filesize = ftell(fp); // определяем текущую позицию указателя
  rewind(fp); // перемещаем указатель в начало файла

  // читаем файл блоками по 1024 байта
  int block_size = 1024;
  char buffer[block_size];
  int pos = 0; // позиция в файле
  char * debug = getenv("LAB11DEBUG"); //переменная для хранения значения переменной окружения LAB11DEBUG
  size_t bytes_read = fread(buffer, 1, block_size, fp);
  while (bytes_read > 0) {
    for (int i = 0; i < (int) bytes_read; i++) {
      if (buffer[i] == sequence[0]) { // если первый байт совпадает, то сравниваем остальные
        int match = 1;
        for (int j = 1; j < seq_len; j++) {
          if (pos + i + j >= filesize) {
            // достигнут конец файла, последовательность не найдена
            match = 0;
            break;
          }
          if (buffer[i + j] != sequence[j]) { // проверяем остальные байты файла
            match = 0;
            break;
          }
        }
        if (match) { // если найдена последовательность
          count++;
          if (debug != NULL) // еперменная окружения LAB11DEBUG не пустая, выводим информацию о найденном совпадении в stderr
            printf("\nSequence found at position %d - %d.", pos + i, pos + i + seq_len - 1);
        }
      }
    }
    pos += bytes_read; // увеличиваем позицию в файле
    bytes_read = fread(buffer, 1, block_size, fp); // читаем следующий блок
  }

  if (count == 0) {
    printf("Sequence not found\n");
  } else {
    printf("Found %d sequences\n", count);
  }

  fclose(fp); // закрываем файл
}

void f_help(void) {
  printf("Usage: %s [OPTION]...\n", program_name);
  printf("Options:\n");
  printf(" -h, --help Print help information and exit the program\n");
  printf(" -v, --version Output version information and exit the program\n");
  exit(EXIT_SUCCESS);
}

void f_version(void) {
  printf("%s version 1.0\n", program_name);
  printf("Info: Search for a phrase in all files of the directory tree\n");
  printf("Author: Berezina Darya\n");
  printf("Group: N32511\n");
  printf("Lab: 1.1, variant 2\n");
  exit(EXIT_SUCCESS);
}
