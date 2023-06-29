#include "csv.h"
#include <stdlib.h>
#include <errno.h>

// Определение констант
#define END_OF_LINE '\0'  // Конец строки
#define SEPARATOR ';'     // Разделитель
#define QUOTES '"'        // Кавычки для обрамления записей

// Функция для определения символа новой строки
int is_new_line(char c)
{
  if (c == '\r' || c == '\n') // Если символ равен '\r' или '\n'
    return 1; // Возвращаем 1 (истина)

  return 0; // Возвращаем 0 (ложь)
}

// Функция для чтения следующей записи в CSV-файле
dynamic_string csv_read_next(char** input)
{
  dynamic_string output; // Создаем динамическую строку output
  dynamic_string_init(&output); // Инициализируем динамическую строку output
  char* i = *input; // Устанавливаем указатель i на текущую позицию входной строки

  if (i[0] == END_OF_LINE || is_new_line(i[0])) // Если символ равен концу строки или новой строке
    return output; // Возвращаем пустую динамическую строку output

  if (i[0] == QUOTES) // Если символ равен кавычкам
  {
    i++; // Переходим к следующему символу

    while (i[0] != END_OF_LINE) // Пока символ не равен концу строки
    {
      if (i[0] == QUOTES) // Если символ равен кавычкам
      {
        if (i[1] == QUOTES) // Если следующий символ равен кавычкам
        {
          dynamic_string_push_back(&output, QUOTES); // Добавляем символ кавычек в динамическую строку output
          i += 2; // Переходим к следующим двум символам
        }
        else
        {
          i++; // Переходим к следующему символу
          break; // Прерываем цикл
        }
      }
      else
      {
        dynamic_string_push_back(&output, i[0]); // Добавляем текущий символ в динамическую строку output
        i++; // Переходим к следующему символу
      }
    }
  }
  else
  {
    while (i[0] != END_OF_LINE && !is_new_line(i[0]) && i[0] != SEPARATOR) // Пока символ не равен концу строки, новой строке или разделителю
    {
      dynamic_string_push_back(&output, i[0]); // Добавляем текущий символ в динамическую строку output
      i++; // Переходим к следующему символу
    }
  }

  if (i[0] == SEPARATOR) // Если символ равен разделителю
    i++; // Переходим к следующему символу

  (*input) = i; // Обновляем указатель на текущую позицию входной строки
  return output; // Возвращаем динамическую строку output
}

// Функция для записи записи в CSV-файл
int csv_write_entry(FILE* file, dynamic_string* s)
{
  int escape = 0; // Переменная для определения, нужно ли экранировать запись

  for (size_t i = 0; i < s->length; ++i) // Для каждого символа в динамической строке
  {
    if (s->buffer[i] == SEPARATOR || s->buffer[i] == QUOTES) // Если символ равен разделителю или кавычкам
    {
      escape = 1; // Устанавливаем флаг экранирования в истину
      break; // Прерываем цикл
    }
  }

  if (!escape) // Если флаг экранирования равен лжи
  {
    size_t written = fwrite(s->buffer, sizeof(char), s->length, file); // Записываем динамическую строку в файл
    if (written != s->length) // Если количество записанных символов не равно длине динамической строки
      return errno; // Возвращаем код ошибки errno
    return 0; // Возвращаем 0 (успех)
  }

  if (putc('"', file) == EOF) // Записываем символ кавычек в файл
    return errno; // Возвращаем код ошибки errno

  for (size_t i = 0; i < s->length; ++i) // Для каждого символа в динамической строке
  {
    if (putc(s->buffer[i], file) == EOF) // Записываем символ в файл
      return errno; // Возвращаем код ошибки errno

    if (s->buffer[i] == QUOTES) // Если символ равен кавычкам
    {
      if (putc('"', file) == EOF) // Записываем символ кавычек в файл
        return errno; // Возвращаем код ошибки errno
    }
  }

  if (putc('"', file) == EOF) // Записываем символ кавычек в файл
    return errno; // Возвращаем код ошибки errno

  return 0; // Возвращаем 0 (успех)
}

// Функция для записи разделителя в CSV-файл
int csv_write_separator(FILE* file)
{
  if (putc(SEPARATOR, file) == EOF) // Записываем символ разделителя в файл
    return errno; // Возвращаем код ошибки errno

  return 0; // Возвращаем 0 (успех)
}

// Функция для записи новой строки в CSV-файл
int csv_write_new_line(FILE* file)
{
  if (putc('\r', file) == EOF) // Записываем символ новой строки в файл
    return errno; // Возвращаем код ошибки errno
  if (putc('\n', file) == EOF) // Записываем символ новой строки в файл
    return errno; // Возвращаем код ошибки errno
  
  return 0; // Возвращаем 0 (успех)
}