#include<iostream>
#include<string>
#include<stdio.h>
#include<fstream>
using namespace std;
class server{
    private:
        int PORT;
        string WEB_ROOT;
        int THREAD_NUMBER;
        int server_file_descriptor;
    public:
        server(string configuration_file);
        ~server(){};
        int get_port();
        int get_thread_number();
        string get_web_root();
        void print_server_info();
        void set_file_descriptor(int fd);
        int get_file_descriptor();
};