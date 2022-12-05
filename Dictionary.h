#include<iostream>
#include<string>
#include<vector>
#include"ContentPair.h"
using namespace std;
class Dictionary{
    private:
        vector<ContentPair>pairs;
    public:
        ~Dictionary(){};
        Dictionary();
        void add_value(string content,string value);
        string merge_all();
};