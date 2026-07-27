#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    char *data;
    long int size;
}String;

String insertonfile(void);
void token_string(const char* data, const char* id);

int main(void)
{
    String data = insertonfile(); 
    token_string(data.data, "age");
   
    return 0;
}


void token_string(const char *data, const char *id)
{
   char buffer[64];
   snprintf(buffer, sizeof(buffer),"\"%s\" : ", id);
   char *ptr = strstr(data, buffer);
   if(ptr == NULL)
   {
        perror("erro");
        return;
   }
    ptr += strlen(buffer);
    printf("%s\n", ptr);
}

String insertonfile(void)
{
    String new;
    new.data == NULL;
    new.size = 0;

    FILE *fd = fopen("arquivo.json","rb");
    fseek(fd, 0, SEEK_END);
    long file_size = ftell(fd);
    
    rewind(fd);
    new.data = malloc(file_size);
    if(new.data == NULL) return new;

    size_t bytes = fread(new.data, 1, file_size, fd);
    if(bytes != file_size) return (String){NULL, 0};

    new.data[file_size] = '\0';
    new.size = strlen(new.data) - 2;
    fclose(fd);

    return new;
}
