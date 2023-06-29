#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dynamic_string.h"
#include "hash_map.h"
#include "csv.h"
#include <locale.h>
#include <errno.h>

#define MAX_CSV_LINE 512
#define BOOK_COLUMNS_COUNT 5
#define max(a,b) (((a) > (b)) ? (a) : (b))

char* book_columns[5] = 
{
    "isbn",
    "authors",
    "title",
    "count",
    "available_count"
};

typedef struct 
{
    dynamic_string isbn;
    dynamic_string authors;
    dynamic_string title;
    dynamic_string count;
    dynamic_string available_count;
} 
book;

void free_book(book* x)
{
    dynamic_string_free(&(x->isbn));
    dynamic_string_free(&(x->authors));
    dynamic_string_free(&(x->title));
    dynamic_string_free(&(x->count));
    dynamic_string_free(&(x->available_count));
    free(x);
}

hash_map books;

int try_read_books(char* path)
{
    FILE* f = fopen(path, "r");
    if (!f)
    {
        printf("read_books - failed to open a file '%s', error: %s\n", path, strerror(errno));
        return 1;
    }

    char line[MAX_CSV_LINE];
    while (fgets(line, MAX_CSV_LINE, f) != NULL)
    {
        char* unprocessed = line;
        book* b = malloc(sizeof(book));

        b->isbn = csv_read_next(&unprocessed);
        b->authors = csv_read_next(&unprocessed);
        b->title = csv_read_next(&unprocessed);
        b->count = csv_read_next(&unprocessed);
        b->available_count = csv_read_next(&unprocessed);

        hash_map_add(&books, dynamic_string_copy(&(b->isbn)), b);
    }

    fclose(f);
    return 0;
}

int read_command() 
{
    printf("Available commands:\n");
    printf("1 - Add book\n");
    printf("2 - Delete book\n");
    printf("3 - Chenge book info\n");
    printf("4 - Change amount of books\n");
    printf("5 - Exit\n");

    int command;
    printf("Enter command number: ");
    scanf("%d", &command);
    getchar();

    return command;
}

dynamic_string read_string(char* text) 
{
    dynamic_string s;
    dynamic_string_init(&s);

    printf(text);

    int c;
    while ((c = getchar()) != EOF)
    {
        if (c == '\n')
            break;

        dynamic_string_push_back(&s, (char)c);
    }

    return s;
}

void add_book()
{
    book* b = malloc(sizeof(book));

    b->isbn = read_string("Enter ISBN: ");
    
    if (b->isbn.length == 0)
    {
        dynamic_string_free(&b->isbn);
        free(b);
        printf("ISBN have to be non empty\n");
        return;
    }

    hash_map_entry* entry = hash_map_find(&books, &b->isbn);
    if (entry->key != NULL) 
    {
        dynamic_string_free(&b->isbn);
        free(b);
        printf("A book with this ISBN already exist\n");
        return;
    }

    b->authors = read_string("Enter authors: ");
    b->title = read_string("Enter title: ");
    b->count = read_string("Enter count: ");
    b->available_count = read_string("Enter available count: ");

    hash_map_add(&books, dynamic_string_copy(&b->isbn), b);
}

void delete_book() 
{
    dynamic_string isbn = read_string("Enter ISBN: ");
    hash_map_entry* entry = hash_map_find(&books, &isbn);

    if (entry->key == NULL)
    {
        printf("A book with this ISBN is not found\n\n");
    }
    else
    {
        hash_map_delete(&books, entry);
    }

    dynamic_string_free(&isbn);
}

void change_book() 
{
    dynamic_string isbn = read_string("Enter ISBN: ");
    hash_map_entry* entry = hash_map_find(&books, &isbn);

    if (entry->key == NULL)
    {
        printf("A book with this ISBN is not found\n\n");
    }
    else
    {
        book* b = entry->value;
        b->authors = read_string("Enter authors: ");
        b->title = read_string("Enter title: ");
        b->count = read_string("Enter count: ");
        b->available_count = read_string("Enter available count: ");

        hash_map_add(&books, dynamic_string_copy(&b->isbn), b);
    }

    dynamic_string_free(&isbn);
}

void change_book_amount() 
{
    dynamic_string isbn = read_string("Enter ISBN: ");
    hash_map_entry* entry = hash_map_find(&books, &isbn);

    if (entry->key == NULL)
    {
        printf("A book with this ISBN is not found\n\n");
    }
    else
    {
        book* b = entry->value;
        dynamic_string k = b->count;
        dynamic_string new_k = read_string("Enter new count: ");
        b->count = new_k;
        // int x = _strtoi64(k)
        // b->available_count = ;

        hash_map_add(&books, dynamic_string_copy(&b->isbn), b);
    }

    dynamic_string_free(&isbn);
}

int compare_book(book** a, book** b) 
{
    return strcmp((*a)->isbn.buffer, (*b)->isbn.buffer);
}


void die_on_error(int error) 
{
    if (!error)
        return;

    printf("save - failed to write to a file, error: %s\n", strerror(errno));
    exit(1);
}

int save(char* path)
{
    FILE* f = fopen(path, "w");
    if (!f)
    {
        printf("save - failed to open a file '%s', error: %s\n", path, strerror(errno));
        return 1;
    }

    for (size_t i = 0; i < books.capacity; ++i)
    {
        if (books.entries[i].key != NULL)
        {
            book* b = books.entries[i].value;
            die_on_error(csv_write_entry(f, &b->isbn));
            die_on_error(csv_write_separator(f));
            die_on_error(csv_write_entry(f, &b->authors));
            die_on_error(csv_write_separator(f));
            die_on_error(csv_write_entry(f, &b->title));
            die_on_error(csv_write_separator(f));
            die_on_error(csv_write_entry(f, &b->count));
            die_on_error(csv_write_separator(f));
            die_on_error(csv_write_entry(f, &b->available_count));
            die_on_error(csv_write_new_line(f));
        }
    }

    fclose(f);
    return 0;
}

int main() 
{
    system("chcp 1251");
    setlocale(LC_ALL, "UTF8");

    hash_map_init(&books, &free_book);

    if (try_read_books("books.csv"))
        return 1;

    while (1) 
    {
        int command = read_command();

        switch (command)
        {
        case 1:
            add_book();
            break;
        case 2:
            delete_book();
            break;
        case 3:
            change_book();
            break;
        case 4:
            // print_all_books();
            break;
        case 5:
            return save("books.csv");
        default:
            printf("Invalid command number\n");
            break;
        }
    }

    return 0;
}