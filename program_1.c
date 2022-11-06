#pragma region HEADERE

#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/sendfile.h>
#include<sys/stat.h>
#include<errno.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<pthread.h>


#pragma endregion

#pragma region MACROURI SI DEFINITII

#define PORT 8080

char http_header[25]="HTTP/1.1 200 Ok\r\n";
#pragma endregion

#pragma region GET
/*          GET /img/prf/img.jpg HTTP/1.1
            Host:..
            Connection:..
            User-Agent:..
            Accept:...
            Refer:...
            Accept-Encoding:...
            Accept-Language:...
            */
#pragma endregion

#pragma region PARSARE + ACTIUNI PENTRU POST
//functia de parsare primeste calea si extensia fisierului 
//aici parsez extensia
char* parse(char line[],const char*symbol){
    char*copy=(char*)malloc(strlen(line)+1);
    strcpy(copy,line);
    char*message;
    char*token=strtok(copy,symbol); //parsarea se face dupa un string cheie
    int current=0;

    while(token!=NULL){
        token=strtok(NULL," ");
        message=token;
        if(current==0)
        {   
            if(message==NULL){
                message="";
            }
            return message;
        }
        current=current+1;
    }
    free(token);
    free(copy);
    return message;

}
//aici metoda GET/POST
char*parse_method(char line[],const char symbol[]){
    char*copy=(char*)malloc(strlen(line)+1);
    strcpy(copy,line);
    char*message;
    char*token=strtok(copy,symbol);
    int current=0;
    while(token!=NULL){
        if(current==0){
            message=token;
            if(message==NULL){
                message="";

            }
            return message;
        }
        current=current+1;
    }
    free(copy);
    free(token);
    return message;

}
//aici ma ocup de actiunea din get 
char*find_token(char line[],const char symbol[], const char match[]){

    char*copy=(char*)malloc(strlen(line)+1);
    strcpy(copy,line);
    char*message;
    char*token=strtok(copy,symbol);
    while(token!=NULL){
        if(strlen(match)<=strlen(token)){
            int match_char=0;
            for(int i=0;i<strlen(match);i++){
                if(token[i]==match[i]){
                    match_char++;
                }
            }
            if(match_char==strlen(match)){
                message=token;
                return message;
            }

        }
        token=strtok(NULL,symbol);
    }
    free(copy);
    free(token);
    message="";
    return message;
}

#pragma endregion

#pragma region TRANSMITEREA DATELOR 
int send_message(int fd,char path[],char head[]){
    struct stat stat_buf;//retine info despre fisierul de intrare
    write(fd,head,strlen(head)); //ce fel de continut trimit
    int fd_img=open(path,O_RDONLY); //deschid continutul
    if(fd_img<0){
        printf("Can not open file %s with error %d\n",path,fd_img);
    }
    fstat(fd_img,&stat_buf); //infos
    int total_size=stat_buf.st_size;
    int block_size=stat_buf.st_blksize;
    printf("Block size: %d\n",block_size);
    printf("Total size: %d\n",total_size);
    if(fd_img>=0){
        ssize_t sent_size;
        while(total_size>0){
            int send_bytes=((total_size<block_size) ? total_size : block_size);
            int done_bytes=sendfile(fd,fd_img,NULL,block_size);
            total_size=total_size-done_bytes;
        }
       close(fd_img);
    }


}
#pragma endregion

int main(int argc,const char*argv[]){
    int server_file_descriptor;
    int new_socket;
    int pid;
    long value_read;
    struct sockaddr_in address;
    int address_length=sizeof(address);

    if((server_file_descriptor=socket(AF_INET,SOCK_STREAM,0))==0)
    {
            perror("Error in creating server descriptor:");
            exit(-1);
    }

   address.sin_family = AF_INET;
   address.sin_addr.s_addr=INADDR_ANY;
   address.sin_port=htons( PORT );
   memset(address.sin_zero,'\0',sizeof(address.sin_zero));
   if(bind(server_file_descriptor,(struct sockaddr*)&address,sizeof(address))<0){
        perror("Error while binding:");
        close(server_file_descriptor);
        exit(-1);
   }
   if(listen(server_file_descriptor,10)<0){
        perror("Error while listening:");
        exit(-1);
   }

   while(1){
       printf("LISTENING....\n");
       if((new_socket=accept(server_file_descriptor,(struct sockaddr*)&address,
       (socklen_t*)&address_length))<0){

        //aici am nevoie sa creez un pthread care sa se ocupe de functia de mai jos
        perror("Error on accepting connection:");
        exit(-1);
       }
       //am nevoie de un alt proces care sa se ocupe de cererile altui client
       pid=fork();
       if(pid<0){
            perror("Error on creating child process:");
            exit(-1);
       }
       //daca procesul a fost creat...
       if(pid==0){
            printf("In child proces...\n");
            char buffer[30000]={0};
            value_read=read(new_socket,buffer,30000);
            printf("\n BUFF MSG:%s \n",buffer);
            char*parse_string_method=parse_method(buffer," "); //calea catre care cere acces clientul
            printf("Client request:%s\n",parse_string_method);
            char*parse_string=parse(buffer," ");
            char*copy=(char*)malloc(strlen(parse_string)+1);
            strcpy(copy,parse_string); //vreau sa iau extensia fisierului.
            char*parse_extension=parse(copy,".");
            char*copy_head=(char*)malloc(strlen(http_header)+200);
            strcpy(copy_head,http_header);
            if(parse_string_method[0]=='G'&&parse_string_method[1]=='E'&&parse_string_method[2]=='T'){
                if(strlen(parse_string)<=1){
                    //parse_string=="/"->index.html
                    char path_head[500]=".";
                    strcat(path_head,"/index.html");
                    strcat(copy_head,"Content-Type: text/html\r\n\r\n");
                    send_message(new_socket,path_head,copy_head);

                }else if((parse_extension[0]=='j'&&parse_extension[1]=='p'&&parse_extension[2]=='g') || (parse_extension[0]=='J'&&parse_extension[1]=='P'&&parse_extension[2]=='G')){
                    //trimit imagine jpg/JPG
                    char path_head[500]=".";
                    strcat(path_head,parse_string);
                    strcat(copy_head,"Content-Type: image/jpeg\r\n\r\n");
                    send_message(new_socket,path_head,copy_head);
                }else if(parse_extension[0]=='i'&&parse_extension[1]=='c'&&parse_extension[2]=='o'){
                    //trimit ico
                    char path_head[500]=".";
                    strcat(path_head,"/img/favicon.png"); //toate site-urile o cer
                    strcat(copy_head,"Content-Type: image/vnd.microsoft.icon\r\n\r\n");
                    send_message(new_socket,path_head,copy_head);
                }else if(parse_extension[0]=='t'&&parse_extension[1]=='t'&&parse_extension[2]=='f'){
                    //font type
                    char path_head[500]=".";
                    strcat(path_head,parse_string); 
                    strcat(copy_head,"Content-Type: font/ttf\r\n\r\n");
                    send_message(new_socket,path_head,copy_head);
                }else if(parse_extension[strlen(parse_extension)-2]=='j'&&parse_extension[strlen(parse_extension)-1]=='s'){
                    //javascript file
                    char path_head[500]=".";
                    strcat(path_head,parse_string);
                    strcat(copy_head,"Content-Type: text/javascript\r\n\r\n");
                    send_message(new_socket,path_head,copy_head);
                }else if(parse_extension[strlen(parse_extension)-3]=='c'&&parse_extension[strlen(parse_extension)-2]=='s'&&parse_extension[strlen(parse_extension)-1]=='s'){
                    //css file
                    char path_head[500]=".";
                    strcat(path_head,parse_string);
                    strcat(copy_head,"Content-Type: text/css\r\n\r\n");
                    send_message(new_socket,path_head,copy_head);

                }else if(parse_extension[0]=='w'&&parse_extension[1]=='o'&&parse_extension[2]=='f'){
                    //woff
                    char path_head[500]=".";
                    strcat(path_head,parse_string);
                    strcat(copy_head,"Content-Type: font/woff\r\n\r\n");
                    send_message(new_socket,path_head,copy_head);
                }else if(parse_extension[0]=='m'&&parse_extension[1]=='3'&&parse_extension[2]=='u' && parse_extension[3]=='8'){
                    //m3u8
                    char path_head[500]=".";
                    strcat(path_head,parse_string);
                    strcat(copy_head,"Content-Type: application/vnd.apple.mpegurl\r\n\r\n");
                    send_message(new_socket,path_head,copy_head);
                }else if(parse_extension[0]=='t'&&parse_extension[1]=='s'){
                    //web ts
                    char path_head[500]=".";
                    strcat(path_head,parse_string);
                    strcat(copy_head,"Content-Type: video/mp2t\r\n\r\n");
                    send_message(new_socket,path_head,copy_head);
                }else{
                    char path_head[500]=".";
                    strcat(copy_head,"Content-Type: text/plain\r\n\r\n");
                    send_message(new_socket,path_head,copy_head);
                }
                printf("------Sent!\n");
                
            }else if(parse_string_method[0]=='P'&&parse_string_method[1]=='O'&&parse_string_method[2]=='S'&&parse_string_method[3]=='T'){
                char *find_string=malloc(200*sizeof(char));
                find_string=find_token(buffer,"\r\n","action");
                strcat(copy_head,"Content-Type: text/plain\r\n\r\n");
                strcat(copy_head,"User Action: ");
                strcat(copy_head,find_string);
                write(new_socket,copy_head,strlen(copy_head));
            }
            printf("Closing socket...");
            close(new_socket);
            free(copy);
            free(copy_head);

       }
       else{
        printf("In parent process..\n");
        printf("Closing socket...");
        printf("Child pid:%d<<<<",pid);
         close(new_socket);
       }
   }
   printf("Closing server...\n");
   close(server_file_descriptor);
   return 0;

}