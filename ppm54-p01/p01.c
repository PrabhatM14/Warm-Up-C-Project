#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

void to_lowercase(char *str) {
    while (*str) {
        *str = tolower((unsigned char)*str);
        str++;
    }
}

int count_substring(const char *text, const char *substr) {
    int count = 0;
    size_t len = strlen(substr);
    if (len == 0) return 0;
    
    const char *pos = text;
    while ((pos = strcasestr(pos, substr)) != NULL) {
        count++;
        pos++;  
    }
    return count;
}

char *read_file_stdio(const char *prog_name, const char *filename, long *file_size) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "%s: %s\n", prog_name, strerror(errno));
        return NULL;
    }
    
    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return NULL;
    }
    *file_size = ftell(file);
    if (*file_size == -1) {
        fclose(file);
        return NULL;
    }
    rewind(file);

    char *content = malloc(*file_size + 1);
    if (!content) {
        fclose(file);
        return NULL;
    }

    if (fread(content, 1, *file_size, file) != (size_t)*file_size) {
        free(content);
        fclose(file);
        return NULL;
    }
    content[*file_size] = '\0';  
    fclose(file);
    return content;
}

char *read_file_system_calls(const char *filename, long *file_size) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "%s: %s\n", filename, strerror(errno));
        return NULL;
    }

    char *content = NULL;
    char *temp;
    char ch;
    long total_size = 0;

    while (read(fd, &ch, 1) == 1) { 
        temp = realloc(content, total_size + 2);  
        if (!temp) {
            free(content);
            close(fd);
            return NULL;
        }
        content = temp;
        content[total_size] = ch;
        total_size++;
    }

    close(fd);

    if (total_size == 0) {
        free(content);
        return NULL;
    }

    content[total_size] = '\0';
    *file_size = total_size;
    return content;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <filename> <substring1> [substring2 ...] [--systemcalls]\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    int use_system_calls = 0;
    if (strcmp(argv[argc - 1], "--systemcalls") == 0) {
        use_system_calls = 1;
        argc--; 
    }
    
    long file_size;
    char *content = use_system_calls
        ? read_file_system_calls(argv[1], &file_size) 
        : read_file_stdio(argv[0], argv[1], &file_size);
    
    if (!content) return EXIT_FAILURE;
    
    char *content_lower = strdup(content);
    if (!content_lower) {
        free(content);
        return EXIT_FAILURE;
    }
    to_lowercase(content_lower);
    
    for (int i = 2; i < argc; i++) {
        char *substr_lower = strdup(argv[i]);
        if (!substr_lower) {
            free(content);
            free(content_lower);
            return EXIT_FAILURE;
        }
        to_lowercase(substr_lower);
        
        int occurrences = count_substring(content_lower, substr_lower);
        printf("%d\n", occurrences);
        
        free(substr_lower);
    }
    
    free(content);
    free(content_lower);
    return EXIT_SUCCESS;
}
