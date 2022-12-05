#include"Dictionary.h"
Dictionary::Dictionary(){
    ContentPair p1("Content type","");
    ContentPair p2("Content type","");
    ContentPair p3("Content type","");
    ContentPair p4("Content type","");
}
void Dictionary::add_value(string content,string value){
     for(ContentPair i: this->pairs){
        if(i.get_content() ==content){
            i.set_value(value);
            return;
        }
     }
     ContentPair p(content,value);
     this->pairs.push_back(p);
}
string Dictionary::merge_all(){
    string headers;
    for(ContentPair i: this->pairs){
        headers+=i.merge_cpair();
    }
    return headers;
}