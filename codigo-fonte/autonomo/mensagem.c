#include <string.h>
#include <ftw.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>

int main(void){
    //abra um arquivo com essa funćão open
    const char * string = "teste Leuender!";
    
    int fd = open("/dev/pts/0" , O_WRONLY);
    write(fd, string, strlen(string));
    close(fd);

    return 0;
}
