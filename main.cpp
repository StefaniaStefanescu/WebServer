#include<iostream>
#include"Server.h"
#include"Dictionary.h"
using namespace std;
int main() {
	Server* sv = new Server("configurare.txt");
	sv->print_server();
	ContentPair* pair = new ContentPair("Content type", "html");
	string m = pair->merge();
	Dictionary d;
	d.add_value("Content type", "txt");
	string me = d.merge_all();
	cout << me << endl;
	
}