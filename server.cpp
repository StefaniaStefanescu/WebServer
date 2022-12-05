#include"server.h"
server::server(string configuration_file){
    ifstream confile;
    confile.open(configuration_file);
    string message;
    confile>>message>>this->PORT;
    confile>>message>>this->WEB_ROOT;
    confile>>message>>this->THREAD_NUMBER;
}
 int server::get_port(){
    return this->PORT;
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
    cout<<"Server maximum thread number"<<this->THREAD_NUMBER<<endl;
}
void server::print_server_info(){
    cout<<"Server port::"<<this->PORT<<endl;
    cout<<"Server root::"<<this->WEB_ROOT<<endl;
    cout<<"Server maximum thread number"<<this->THREAD_NUMBER<<endl;
}
void server::set_file_descriptor(int fd){
    this->server_file_descriptor=fd;
}
int server::get_file_descriptor(){
    return this->server_file_descriptor;
}