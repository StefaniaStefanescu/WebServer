#pragma once
#include<iostream>
using namespace std;
class ContentPair
{
private:
	string Content;
	string Value;
public:
	ContentPair() {};
	ContentPair(string content, string value);
	~ContentPair() {};
	string get_content();
	string get_value();
	void set_value(string new_value);
	string merge();
};

