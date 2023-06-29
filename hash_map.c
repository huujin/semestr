#include "hash_map.h" 
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 

void hash_map_init(hash_map* map, void (*free_value)(void*)) 
{
  map->length = 0; // устанавливаем длину хэш-карты в 0
  map->capacity = 0; // устанавливаем емкость хэш-карты в 0
  map->entries = NULL; // устанавливаем указатель на записи хэш-карты в NULL
  map->free_value = free_value; // устанавливаем указатель на функцию освобождения значения хэш-карты
}

int hash_map_add(hash_map* map, dynamic_string* key, void* value) 
{
  if (map->length * 2 >= map->capacity) // если длина*2 больше или равна ее емкости 
  {
    size_t new_capacity = map->capacity * 2; // вычисляем новую емкость как удвоенную текущую емкость
    if (new_capacity < 2) // если новая емкость меньше 2
      new_capacity = 2; // устанавливаем новую емкость в 2
    hash_map_entry* new_entries = calloc(new_capacity, sizeof(hash_map_entry)); // выделяем память для новых записей хэш-карты
    if (!new_entries) // если не удалось выделить память
      return ENOMEM; // возвращаем код ошибки ENOMEM (нехватка памяти)

    size_t old_capacity = map->capacity; // сохраняем текущую емкость хэш-карты
    hash_map_entry* old_entries = map->entries; // сохраняем указатель на текущие записи хэш-карты

    map->length = 0; // устанавливаем длину хэш-карты в 0
    map->capacity = new_capacity; // устанавливаем емкость хэш-карты равной новой емкости
    map->entries = new_entries; // устанавливаем указатель на новые записи хэш-карты

    for (size_t i = 0; i < old_capacity; i++) // для каждой старой записи хэш-карты
    {
      if (old_entries[i].key != NULL) // если ключ старой записи не равен NULL
        hash_map_add(map, old_entries[i].key, old_entries[i].value); // добавляем старую запись в новую хэш-карту
    }
  }

  hash_map_entry* entry = hash_map_find(map, key); // находим запись в хэш-карте по ключу

  if (entry->key != NULL) // если ключ записи не равен NULL, значит запись с таким ключом уже существует
    return 0; // возвращаем 0 (не удалось добавить запись)

  entry->key = key; // устанавливаем ключ записи
  entry->value = value; // устанавливаем значение записи
  map->length++; // увеличиваем длину хэш-карты на 1
  return 1; // возвращаем 1 (запись успешно добавлена)
}

void hash_map_free(hash_map* map) 
{
  for (size_t i = 0; i < map->capacity; i++) // для каждой записи хэш-карты
  {
    if (map->entries[i].key != NULL) // если ключ записи не равен NULL
    {
      (*(map->free_value))(map->entries[i].value); // вызываем функцию освобождения значения записи

      dynamic_string_free(map->entries[i].key); // освобождаем память, занятую ключом записи
      free(map->entries[i].key); // освобождаем память, занятую указателем на ключ записи
    }
  }

  free(map->entries); // освобождаем память, занятую записями хэш-карты
  hash_map_init(map, map->free_value); // инициализируем хэш-карту снова
}

hash_map_entry* hash_map_find(hash_map* map, dynamic_string* key) 
{
  size_t index = dynamic_string_get_hash(key) % map->capacity; // вычисляем индекс записи по хэшу ключа
  while (map->entries[index].key != NULL) // пока ключ записи не равен NULL
  {
    if (!strcmp(map->entries[index].key->buffer, key->buffer)) // если строки ключей равны
      return &(map->entries[index]); // возвращаем указатель на запись

    index = (index + 1) % map->capacity; // переходим к следующей записи по кругу
  }

  return &(map->entries[index]); // возвращаем указатель на запись (может быть пустой)
}

void hash_map_delete(hash_map* map, hash_map_entry* entry) 
{
  size_t index = entry - map->entries; // вычисляем индекс записи в хэш-карте

  dynamic_string_free(map->entries[index].key); // освобождаем память, занятую ключом записи
  map->free_value(map->entries[index].value); // вызываем функцию освобождения значения записи
  map->entries[index].key = NULL; // устанавливаем ключ записи в NULL
  map->entries[index].value = NULL; // устанавливаем значение записи в NULL
  map->length--; // уменьшаем длину хэш-карты на 1

  while (map->entries[index].key != NULL) // пока ключ записи не равен NULL
  {
    dynamic_string* tmpKey = map->entries[index].key; // сохраняем ключ записи
    void* tmpValue = map->entries[index].value; // сохраняем значение записи

    map->entries[index].key = NULL; // устанавливаем ключ записи в NULL
    map->entries[index].value = NULL; // устанавливаем значение записи в NULL

    hash_map_add(map, tmpKey, tmpValue); // добавляем запись в хэш-карту

    index = (index + 1) % map->capacity; // переходим к следующей записи по кругу
  }
}