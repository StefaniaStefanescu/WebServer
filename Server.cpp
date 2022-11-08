#include "Server.h"

Server::Server(string config_file)
{
    ifstream confile;
    confile.open(config_file);
    string message;
    confile >> message >> this->PORT;
    cout << message << ":" << this->PORT << endl;
    confile >> message >> this->WEB_ROOT;
    cout << message << ":" << this->WEB_ROOT << endl;
    confile >> message >> this->THREAD_NUMBER;
    cout << message << ":" << this->THREAD_NUMBER << endl;
}

int Server::get_port()
{
    return this->PORT;
}

int Server::get_thread_number()
{
    return this->THREAD_NUMBER;
}

string Server::get_web_root()
{
    return this->WEB_ROOT;
}

void Server::print_server()
{
    cout << "Server port:" << this->PORT << endl;
    cout << "Server web_root:" << this->WEB_ROOT << endl;
    cout << "Server max. thread number:" << this->THREAD_NUMBER << endl;
}
