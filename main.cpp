#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/sendfile.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<errno.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<pthread.h>
#include<limits.h>
#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<ctime>
using namespace std;


//am de rezolvat faptul ca la multe threaduri mi se deschid fisierele concurent

//Comanda testare: ab -n 1000 -c 21 http://127.0.0.1:8080/index.html
////////////////////////////////////////////////////COADA DE THREADURI//////////////////////////////////////////////////////////////////////////////////
struct node {
    struct node*next;
    int *client_socket;
};
typedef struct node node_t;
node_t*head =NULL;
node_t*tail =NULL;
void enqueue(int*client_socket){
    node_t *new_node=(node_t*)malloc(sizeof(node_t));
    new_node->client_socket=client_socket;
    new_node->next=NULL;
    if(tail==NULL){
        head=new_node;

    }else{
        tail->next=new_node;
    }
    tail=new_node;
}
int* dequeue(){
    if(head==NULL){
        return NULL;
    }else{
        int*result=head->client_socket;
        node_t*temp=head;
        head=head->next;
        if(head==NULL){
            tail=NULL;
        }
        free(temp);
        return result;
    }
}
///////////////////////////////////////////////CLASA LOGGER /////////////////////////////////////////////////////////////////////////////////////////////
class Logger{
    public:
        static Logger& getInstance(string err,string logg);
        static void destroyInstance();
        void log_request(string buffer);
        void log_error(string buffer);
        ofstream error_file;
        ofstream logger_file;
    private:
        
        static Logger*instance;
        Logger(string err, string logg){ 
            logger_file.open(logg,ios_base::app);
            if(logger_file.fail()){
               printf("Error on opening request file\n");
               exit(0);
            }else{
                printf("Logger file opened\n");
            }
            
            error_file.open(err, ios_base::app);
            if(error_file.fail()){
                 printf("Error on opening error file\n");
                exit(0);
            }else{
                printf("Error file opened\n");
            }
        };
        Logger(const Logger&){};
        ~Logger(){
             this->logger_file.close();
             this->error_file.close();
        };
};
Logger*Logger::instance = nullptr;
Logger& Logger::getInstance(string err,string logg){
    if(!Logger::instance){
        Logger::instance=new Logger(err,logg);
    }
    return (*Logger::instance);
}
void Logger::destroyInstance(){
    if(Logger::instance){
        delete Logger::instance;
        Logger::instance=nullptr;
    }
}
void Logger::log_request(string buffer){
    
    time_t t=time(0);
    tm*now=localtime(&t);
    Logger::logger_file<<"Request time:"<<(now->tm_year+1900)<<"-"<<(now->tm_mon+1)<<"-"<<(now->tm_mday)<<"-"<<(now->tm_hour)<<":"<<(now->tm_min)<<":"<<(now->tm_sec)<<endl;
    Logger::logger_file<<buffer<<endl;
   

}
void Logger::log_error(string buffer){

    printf("Error file opened\n...");
    time_t t=time(0);
    tm*now=localtime(&t);
    Logger::error_file<<"Error time:"<<(now->tm_year+1900)<<"-"<<(now->tm_mon+1)<<"-"<<(now->tm_mday)<<"-"<<(now->tm_hour)<<":"<<(now->tm_min)<<":"<<(now->tm_sec)<<endl;
    Logger::error_file<<buffer<<endl;
    
}
///////////////////////////////////////////////CLASE DICTIONAR////////////////////////////////////////////////////////////////////////////////////////////
class ContentPair{
    private:
        string Content;
        string value;
    public:
        ContentPair(){};
        ~ContentPair(){};
        ContentPair(string content,string value);
        string get_content();
        string get_value();
        void set_value(string new_value);
        string merge_cpair();
};
ContentPair::ContentPair(string content,string value){
    this->Content=content;
    this->value=value;
}
string ContentPair::get_content(){
    return this->Content;
}
string ContentPair::get_value(){
    return this->value;
}
void ContentPair::set_value(string new_value){
    this->value=new_value;
}   
string ContentPair::merge_cpair(){
    //Content-Type: font/ttf\r\n\r\n
    string merged=this->Content+": ";
    merged+=this->value;
    merged+="\r\n\r\n";
    return merged;
}

class Dictionary{
    private:
        string head;
        vector<ContentPair>pairs;
    public:
        ~Dictionary(){};
        Dictionary();
        void add_value(string content,string value);
        string merge_all();
};
Dictionary::Dictionary(){
   this->head="HTTP/1.1 200 Ok\r\n";
}
void Dictionary::add_value(string content,string value){
     for(ContentPair i: this->pairs){
        if(i.get_content() ==content){
            i.set_value(value);
            return;
        }
     }
     ContentPair p(content,value);
     this->pairs.push_back(p);
}
string Dictionary::merge_all(){
    string headers;
    headers+=this->head;
    for(ContentPair i: this->pairs){
        headers+=i.merge_cpair();
    }
    return headers;
}

//////////////////////////////////////////CLASA SERVER////////////////////////////////////////////////////////////////////////////////////////////////////
class server{
    private:
        int PORT;
        string WEB_ROOT;
        int THREAD_NUMBER;
        int server_file_descriptor;
        string ERROR_FILE;
        string LOG_FILE;
        
        
    public:
        string PHP_FILE;
        string PHP_USE;
        server(string configuration_file);
        ~server(){};
        int get_port();
        int get_thread_number();
        string get_web_root();
        void set_file_descriptor(int fd);
        int get_file_descriptor();
        string get_error_file();
        string get_log_gile();
        void print_server_info();
};
server::server(string configuration_file){
    ifstream confile;
    confile.open(configuration_file);
    string message;
    confile>>message>>this->PORT;
    confile>>message>>this->WEB_ROOT;
    confile>>message>>this->THREAD_NUMBER;
    confile>>message>>this->ERROR_FILE;
    confile>>message>>this->LOG_FILE;
    confile>>message>>this->PHP_FILE;
    confile>>message>>this->PHP_USE;
}
 int server::get_port(){
    return this->PORT;
 }
string server::get_error_file(){
    return this->ERROR_FILE;
}
string server::get_log_gile(){
    return this->LOG_FILE;
}
int server::get_thread_number(){
    return this->THREAD_NUMBER;
}
string server::get_web_root(){
    return this->WEB_ROOT;
}
void server::print_server_info(){
    cout<<"Server port::"<<this->PORT<<endl;
    cout<<"Server root::"<<this->WEB_ROOT<<endl;
    cout<<"Server maximum thread number::"<<this->THREAD_NUMBER<<endl;
    cout<<"Error file::"<<this->ERROR_FILE<<endl;
    cout<<"Log file::"<<this->LOG_FILE<<endl;
}
void server::set_file_descriptor(int fd){
    this->server_file_descriptor=fd;
}
int server::get_file_descriptor(){
    return this->server_file_descriptor;
}



/// @brief 
server*webserver=new server("/home/nia/Desktop/serverweb/server_web/configurare.txt");
pthread_mutex_t mutex;
//am nevoie ca threadurile sa astepte pana cand apare ceva de facut.
pthread_cond_t conditional_variable = PTHREAD_COND_INITIALIZER;  //variabila conditionala, am nevoie de ea a.i serverul meu sa nu mai utilizeze toate resursele cpu
pthread_t thread_pool[1000];  //aici imi zice ca trebuie sa am mereu o valoare constanta ??
Logger& logger = Logger::getInstance(webserver->get_error_file(), webserver->get_log_gile());




/////////////////////////////////////////FUNCTII DE PARSARE/////////////////////////////////////////////////////////////////////////////////////////////
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
///////////////////////////////////////TRANSMITERE MESAJ + GESTIONARE CONEXIUNE///////////////////////////////////////////////////////////////////////
void send_message(int fd,char path[],char head[]){
    struct stat stat_buf;//retine info despre fisierul de intrare
    write(fd,head,strlen(head)); //ce fel de continut trimit
    int file_desc=open(path,O_RDONLY); //deschid continutul
    if(file_desc<0){
        printf("Can not open file %s with error %d\n",path,fd);
    }
    fstat(file_desc,&stat_buf); //infos
    int total_size=stat_buf.st_size;
    int block_size=stat_buf.st_blksize;
    if(file_desc>=0){
        ssize_t sent_size;
        while(total_size>0){
            int send_bytes=((total_size<block_size) ? total_size : block_size);
            int done_bytes=sendfile(fd,file_desc,NULL,block_size);
            total_size=total_size-done_bytes;
        }
       close(fd);
    }


}
string can_not_open_file(char*path,int file_desc){
    string str="Can not open file:";
    str+=path;
    str+=" with error::";
    str+=file_desc;
    return str;
}
int get_total_size_for_header(char path[]){
    struct stat stats_buffer;
    int file_descriptor=open(path, O_RDONLY);
    if(file_descriptor<0){
        //aici o sa scriu in logger
        printf("Cannot open file %s with error:: %d\n",path,file_descriptor);
        logger.log_error(can_not_open_file(path,file_descriptor));
        return 0;
    }
    fstat(file_descriptor,&stats_buffer);   
    int total_size=stats_buffer.st_size;
    close(file_descriptor);
    return total_size;
}
void fork_php(){

}

char* string_to_char(string str){
    int n=str.length();
    char* char_arr=(char*)malloc(n+1);
    strcpy(char_arr,str.c_str());
    return char_arr;
}


void php_interpreter(char*filename, char*token){
    //creez pipe
    int fd[2];
    pid_t pid;
    char*arguments[]={"php", "-c",string_to_char(webserver->PHP_FILE), string_to_char(webserver->PHP_USE),token,NULL};
    if (-1 == pipe(fd)) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    if (-1 == (pid = fork())) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        /* child */
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        execvp("php",arguments);
        /*i won't go there if i succed.*/
        perror("execvp");
        exit(EXIT_FAILURE);
    }

    /* parent */
    char result;
    close(fd[1]);
    int output=open("disable.txt", O_WRONLY | O_CREAT | O_TRUNC,0644);
    if(output<0){
        perror("disable.txt");
        exit(1);
    }
    while (read(fd[0], &result, 1) > 0)
        //write(STDOUT_FILENO, &result, 1);
        write(output,&result,1);
    close(fd[0]);
    wait(NULL);


}
void remove_file(char*filename){
    char *binarypath="/bin/rm";
    char*arg1="-f";
    char*arg2="filename";

    execl(binarypath,binarypath,arg1,arg2,NULL);
    return ;
}

void* connection_handle(void*client_socket){
    logger.log_request("Handling connection....");
    int new_socket=*((int*)client_socket);
    long value_read;
    free(client_socket);
    char buffer[30000]={0};
    value_read=read(new_socket,buffer,30000);
    printf("\n BUFF MSG:%s \n",buffer);
    logger.log_request(buffer);
    char*parse_string_method=parse_method(buffer," ");
    printf("Client request:%s\n",parse_string_method);
    logger.log_request(parse_string_method);
    char*parse_string=parse(buffer," ");
    char*copy=(char*)malloc(strlen(parse_string)+1);
    strcpy(copy,parse_string);
    char*parse_extension=parse(copy,".");
    Dictionary Header;
    string path_head=webserver->get_web_root();
    if(parse_string_method[0]=='G'&&parse_string_method[1]=='E'&&parse_string_method[2]=='T'){
        if(strlen(parse_string)<=1){
           
            path_head+="/index.html";
            Header.add_value("Content-Type","text/html");
            send_message(new_socket,string_to_char(path_head),string_to_char(Header.merge_all()));
            
        }else if((parse_extension[0]=='j'&&parse_extension[1]=='p'&&parse_extension[2]=='g') || (parse_extension[0]=='J'&&parse_extension[1]=='P'&&parse_extension[2]=='G')){
            path_head+=parse_string;
            Header.add_value("Content-Type","image/jpeg");
            send_message(new_socket,string_to_char(path_head),string_to_char(Header.merge_all()));
            
        }else if(parse_extension[0]=='i'&&parse_extension[1]=='c'&&parse_extension[2]=='o'){  
            path_head+="/img/favicon.png";
            Header.add_value("Content-Type","image/vnd.microsoft.icon");
            send_message(new_socket,string_to_char(path_head),string_to_char(Header.merge_all()));
            
        }else if(parse_extension[0]=='t'&&parse_extension[1]=='t'&&parse_extension[2]=='f'){
            path_head+=parse_string;
            Header.add_value("Content-Type","font/ttf");
            send_message(new_socket,string_to_char(path_head),string_to_char(Header.merge_all()));
            
        }else if(parse_extension[strlen(parse_extension)-2]=='j'&&parse_extension[strlen(parse_extension)-1]=='s'){
            path_head+=parse_string;
            Header.add_value("Content-Type","text/javascript");
            send_message(new_socket,string_to_char(path_head),string_to_char(Header.merge_all()));
            
        }else if(parse_extension[strlen(parse_extension)-3]=='c'&&parse_extension[strlen(parse_extension)-2]=='s'&&parse_extension[strlen(parse_extension)-1]=='s'){
            path_head+=parse_string;
            Header.add_value("Content-Type","text/css");
            send_message(new_socket,string_to_char(path_head),string_to_char(Header.merge_all()));
            
        }else if(parse_extension[0]=='w'&&parse_extension[1]=='o'&&parse_extension[2]=='f'){
            path_head+=parse_string;
            Header.add_value("Content-Type","font/woff");
            send_message(new_socket,string_to_char(path_head),string_to_char(Header.merge_all()));
            
        }else if(parse_extension[0]=='m'&&parse_extension[1]=='3'&&parse_extension[2]=='u' && parse_extension[3]=='8'){
            path_head+=parse_string;
            Header.add_value("Content-Type","application/vnd.apple.mpegurl");
            send_message(new_socket,string_to_char(path_head),string_to_char(Header.merge_all()));
          
        }else if(parse_extension[0]=='t'&&parse_extension[1]=='s'){
            path_head+=parse_string;
            Header.add_value("Content-Type","video/mp2t");
            send_message(new_socket,string_to_char(path_head),string_to_char(Header.merge_all()));
            
        }else if((parse_extension[0]=='p'&&parse_extension[1]=='h'&&parse_extension[2]=='p') || (parse_extension[0]=='p'&&parse_extension[1]=='h'&&parse_extension[2]=='t' &&parse_extension[2]=='m' && parse_extension[2]=='l')){
            //functie in care trebuie sa transmit catre interpretorul php fisierul .php
            path_head+=parse_string;
            printf("PHP REQUEST: %s\n",string_to_char(path_head));
            /*   /home/nia/Desktop/serverweb/handle.php?name=adsad&email=saddsa   */
            char*token;
            token=strtok(string_to_char(path_head),"?");
            char*file=strdup(token);
            token=strtok(NULL," \n");
            printf("\n\nTOKEN: %s\n\n",token);
            
            char*to_be_interpreted=strdup(token);
            
            
            php_interpreter(file, to_be_interpreted);
            Header.add_value("Content-Type","text/html");
            send_message(new_socket,"/home/nia/Desktop/serverweb/server_web/disable.txt",string_to_char(Header.merge_all()));

            
        }else{
            path_head+=parse_string;
            Header.add_value("Content-Type","text/plain");
            send_message(new_socket,string_to_char(path_head),string_to_char(Header.merge_all()));
            }
        
        printf("------Sent!\n");
        logger.log_request("----------------Sent---------------");

      
    }else if(parse_string_method[0]=='P'&&parse_string_method[1]=='O'&&parse_string_method[2]=='S'&&parse_string_method[3]=='T'){
        char *find_string=(char*)malloc(200*sizeof(char));
        find_string=find_token(buffer,"\r\n","action");
        Header.add_value("Content-Type","text/plain");
        Header.add_value("User Action",find_string);
        write(new_socket,string_to_char(Header.merge_all()),Header.merge_all().length()+1);
    }
        close(new_socket);
        free(copy);
        printf("Closing thread\n");
        logger.log_request("Handled the connection so far..");
        return NULL;
}

void*thread_pool_function(void*arg){
    while(1){
        pthread_mutex_lock(&mutex);
        
        int *pclient;
        if((pclient = dequeue())==NULL){
            pthread_cond_wait(&conditional_variable,&mutex);
            pclient=dequeue();
        }
        pthread_mutex_unlock(&mutex);
        
        if(pclient!=NULL){
            connection_handle(pclient);
        }
    }
}
int main(int argc,const char*argv[]){
    //pthread_mutex_init(&mutex,NULL);
    webserver->print_server_info();
    int server_file_descriptor;
    int new_socket;
    struct sockaddr_in address;
    int address_length=sizeof(address);

    for(int i=0;i<webserver->get_thread_number();i++)
    {
        pthread_create(&thread_pool[i],NULL,thread_pool_function,NULL);
    }
    if((server_file_descriptor=socket(AF_INET,SOCK_STREAM,0))==0)
    {
            //perror("Error in creating server descriptor:");
            logger.log_error("Error on creating server descriptor");
            exit(-1);
    }
    webserver->set_file_descriptor(server_file_descriptor);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr=INADDR_ANY;
    address.sin_port=htons(webserver->get_port());
    memset(address.sin_zero,'\0',sizeof(address.sin_zero));
    if(bind(server_file_descriptor,(struct sockaddr*)&address,sizeof(address))<0){
        //perror("Error while binding:");
        logger.log_error("Error while binding");
        close(server_file_descriptor);
        exit(-1);
    }
    if(listen(server_file_descriptor,10)<0){
        //perror("Error while listening:");
        logger.log_error("Error while listening");
        exit(-1);
    }
  
   
   while(1){
       printf("LISTENING....\n");
       logger.log_request("Server Started\nListening.....");

       if((new_socket=accept(server_file_descriptor,(struct sockaddr*)&address,(socklen_t*)&address_length))<0){
          //perror("Error on accepting connection:");
          logger.log_error("Error on accepting connection\n");
          exit(-1);
       }
        int*p_client=(int*)malloc(sizeof(int));
        *p_client=new_socket;
        pthread_mutex_lock(&mutex);
        enqueue(p_client);
        pthread_cond_signal(&conditional_variable);
        pthread_mutex_unlock(&mutex);

   }
   printf("Closing server...\n");
   logger.log_request("Closing server...");
   close(server_file_descriptor);
   return 0;

}