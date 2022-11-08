#include "ContentPair.h"

ContentPair::ContentPair(string content, string value)
{
	this->Content = content;
	this->Value = value;

}

string ContentPair::get_content()
{
	return this->Content;
}

string ContentPair::get_value()
{
	return this->Value;
}

void ContentPair::set_value(string new_value)
{
	this->Value = new_value;
}

string ContentPair::merge()
{
	string merged = this->Content + ": ";
	merged += this->Value;
	cout << "Merged:" << merged << endl;
	return merged;

}
