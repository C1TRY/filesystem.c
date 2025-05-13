#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 1.1 Функция открытия/создания файла файловой системы
FILE* open_or_create_fs_file(const char* filename) {
    FILE* file = fopen(filename, "a+"); // Открываем для чтения и дописывания
    if (file == NULL) {
        perror("Failed to open or create file");
        exit(EXIT_FAILURE);
    }
    return file;
}

// 1.2 Функция просмотра содержимого файла в файловой системе
char* view_file_in_fs(const char* fs_filename, const char* target_filename) {
    FILE* fs_file = fopen(fs_filename, "r");
    if (fs_file == NULL) {
        perror("Failed to open filesystem file");
        return NULL;
    }

    // Читаем весь файл
    fseek(fs_file, 0, SEEK_END);
    long length = ftell(fs_file);
    fseek(fs_file, 0, SEEK_SET);
    
    char* buffer = malloc(length + 1);
    if (buffer) {
        fread(buffer, 1, length, fs_file);
        buffer[length] = '\0';
    }
    fclose(fs_file);

    // Разбиваем на строки
    char** lines = NULL;
    char* line = strtok(buffer, "\n");
    int count = 0;
    int start_index = -1;
    int end_index = -1;

    while (line != NULL) {
        lines = realloc(lines, sizeof(char*) * (count + 1));
        lines[count] = line;
        
        // Ищем начало файла
        if (strcmp(line, target_filename) == 0) {
            start_index = count + 1;
        }
        
        // Ищем конец файла (после начала)
        if (start_index != -1 && end_index == -1 && line[0] == '/') {
            end_index = count;
            break;
        }
        
        line = strtok(NULL, "\n");
        count++;
    }

    if (start_index == -1) {
        free(buffer);
        free(lines);
        return NULL; // Файл не найден
    }

    // Собираем содержимое файла
    char* content = NULL;
    size_t content_len = 0;
    for (int i = start_index; i < end_index; i++) {
        content_len += strlen(lines[i]) + 1; // +1 для \n
    }

    content = malloc(content_len + 1);
    content[0] = '\0';

    for (int i = start_index; i < end_index; i++) {
        strcat(content, lines[i]);
        if (i != end_index - 1) {
            strcat(content, "\n");
        }
    }

    free(buffer);
    free(lines);
    return content;
}

// 1.3 Функция удаления файла из файловой системы
int delete_file_in_fs(const char* fs_filename, const char* target_filename) {
    FILE* fs_file = fopen(fs_filename, "r");
    if (fs_file == NULL) {
        perror("Failed to open filesystem file");
        return -1;
    }

    // Читаем весь файл
    fseek(fs_file, 0, SEEK_END);
    long length = ftell(fs_file);
    fseek(fs_file, 0, SEEK_SET);
    
    char* buffer = malloc(length + 1);
    if (buffer) {
        fread(buffer, 1, length, fs_file);
        buffer[length] = '\0';
    }
    fclose(fs_file);

    // Разбиваем на строки
    char** lines = NULL;
    char* line = strtok(buffer, "\n");
    int count = 0;
    int start_index = -1;
    int end_index = -1;
    int found = 0;

    while (line != NULL) {
        lines = realloc(lines, sizeof(char*) * (count + 1));
        lines[count] = line;
        
        if (!found && strcmp(line, target_filename) == 0) {
            start_index = count;
            found = 1;
        }
        
        if (found && end_index == -1 && line[0] == '/' && count > start_index) {
            end_index = count;
        }
        
        line = strtok(NULL, "\n");
        count++;
    }

    if (!found) {
        free(buffer);
        free(lines);
        return -1;
    }

    if (end_index == -1) {
        end_index = count;
    }

    // Создаем новый буфер без удаляемого файла
    char* new_buffer = malloc(length + 1);
    new_buffer[0] = '\0';
    int new_length = 0;

    for (int i = 0; i < count; i++) {
        if (i < start_index || i >= end_index) {
            strcat(new_buffer, lines[i]);
            strcat(new_buffer, "\n");
            new_length += strlen(lines[i]) + 1;
        }
    }
// Перезаписываем файл
    fs_file = fopen(fs_filename, "w");
    if (fs_file == NULL) {
        free(buffer);
        free(lines);
        free(new_buffer);
        perror("Failed to open filesystem file for writing");
        return -1;
    }

    fwrite(new_buffer, 1, new_length, fs_file);
    fclose(fs_file);

    free(buffer);
    free(lines);
    free(new_buffer);
  
    return 0;
}
