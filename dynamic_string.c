#include "dynamic_string.h" // подключаем заголовочный файл для нашей структуры
#include <errno.h> // для работы с ошибками
#include <stdlib.h> // для работы с динамической памятью
#include <string.h> // для работы со строками

// Инициализация динамической строки
void dynamic_string_init(dynamic_string* s) 
{
  s->buffer = NULL; // указатель на буфер строки, изначально равен NULL
  s->length = 0; // длина строки, изначально равна 0
  s->capacity = 0; // вместимость (размер выделенной памяти), изначально равна 0
}

// Освобождение памяти, занимаемой динамической строкой
void dynamic_string_free(dynamic_string* s)
{
  free(s->buffer); // освобождаем память, выделенную для буфера строки
}

// Добавление символа в конец динамической строки
int dynamic_string_push_back(dynamic_string* s, char item) 
{
  if (!s)
    return EINVAL; // если переданный указатель равен NULL, возвращаем код ошибки EINVAL

  if (s->length + 2 > s->capacity) // если длина строки + 2 (1 символ + нулевой символ) превышает вместимость строки, то нужно увеличить выделенную память
  {
    size_t newCapacity = s->capacity * 2; // новая вместимость будет равна удвоенной текущей вместимости
    if (newCapacity < 2)
      newCapacity = 2; // устанавливаем минимальную вместимость строки в 2 символа
    char* newBuffer = realloc(s->buffer, newCapacity); // перевыделяем память под новую вместимость
   
    if (!newBuffer)
      return ENOMEM; // если не удалось выделить память, возвращаем код ошибки ENOMEM

    memset(newBuffer + s->capacity, 0, newCapacity - s->capacity); // заполняем новую память нулевыми символами

    s->buffer = newBuffer; // обновляем указатель на буфер строки
    s->capacity = newCapacity; // обновляем вместимость строки
  }

  s->buffer[s->length++] = item; // добавляем символ в конец строки
  return 0; // возвращаем успешное выполнение
}

// Вычисление хеш-значения динамической строки
size_t dynamic_string_get_hash(dynamic_string* s) 
{
  // Алгоритм взят с http://www.cse.yorku.ca/~oz/hash.html
  size_t hash = 5381; // начальное значение хеша
  for (size_t i = 0; s->buffer[i]; i++) // цикл по символам строки
    hash = hash * 33 ^ s->buffer[i]; // обновление хеша
  return hash; // возвращаем полученное хеш-значение
}

// Создание копии динамической строки
dynamic_string* dynamic_string_copy(dynamic_string* s)
{
  dynamic_string* output = malloc(sizeof(dynamic_string)); // выделяем память для новой структуры
  output->length = s->length; // копируем длину и вместимость исходной строки
  output->capacity = s->length;
  output->buffer = malloc(s->length + 1); // выделяем память для буфера строки
  strcpy(output->buffer, s->buffer); // копируем содержимое буфера строки
  return output; // возвращаем указатель на новую динамическую строку
}

/*
Всякий раз, когда указано возвращаемое значение функции, а также коды ошибок all_error[], данные могут отличаться от реальной ситуации в зависимости от контекста приложения или использования.
Необходимо самостоятельно проверять и обрабатывать ошибки при работе с этим кодом.
*/