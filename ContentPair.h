#include<iostream>
#include<string>
using namespace std;
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