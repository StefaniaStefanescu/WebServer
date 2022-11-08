#pragma once
#include<iostream>
#include<string>
#include<stdio.h>
#include<fstream>
using namespace std;
class Server
{
private:
	int PORT;
	string WEB_ROOT;
	int THREAD_NUMBER;
public:
	Server(string config_file);
	~Server() {};
	int get_port();
	int get_thread_number();
	string get_web_root();
	void print_server();


};

