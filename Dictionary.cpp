#include "Dictionary.h"

Dictionary::Dictionary()
{
	ContentPair p1("Content type", "");
	ContentPair p2("Content length", "");
	ContentPair p3("Content encoding", "");
	ContentPair  p4("Date", "");
	pairs.push_back(p1);
	pairs.push_back(p2);
	pairs.push_back(p3);
	pairs.push_back(p4);
	cout<<"Generated pairs : "<<endl;
	for (ContentPair i : this->pairs)
		cout << "Content = " << i.get_content()<<"\nValue:"<<i.get_value() << endl;
}

void Dictionary::add_value(string content, string value)
{
	for (ContentPair i : this->pairs) {
		if (i.get_content() == content) {
			i.set_value(value);
			return;
		}
	}
	ContentPair p(content, value);
	this->pairs.push_back(p);
}

string Dictionary::merge_all()
{
	string header;
	for (ContentPair i : this->pairs) {
		header += i.merge();
		header += "\n";
	}
	return header;
}
