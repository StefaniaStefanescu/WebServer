#pragma region Headere
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<fcntl.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<sys/time.h>
#include<sys/sysinfo.h>
#pragma endregion

#pragma region Macro-uri
#define ERROR 42
#define LOG 44
#define FORBIDDEN 403
#define NOT_FOUND 404
#define FIFO_MODE 0
#define HPIC_MODE 1
#define HPHC_MODE 2
#define HTML_FILE 0
#define IMAGE_FILE 1
#define OTHER_FILE 2
#pragma endregion

#pragma region STRUCTURI
struct{
    char* extension;
    char*file_type;
}extensions[]={
    {"gif","image/gif"},
    {"jpeg","image/jpeg"},
    {"jpg","image/jpg"},
    {"png","image/png"},
    {"ico","image/ico"},
    {"zip","image/zip"},
    {"gz","image/gz"},
    {"tar","image/tar"},
    {"htm","text/html"},
    {"html","text/html"},
    {0,0}
};


/*cererile de tip GET

   GET /text.html HTTP/1.1


*/



#pragma endregion

int main(){
    return 0;
}