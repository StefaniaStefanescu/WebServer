#include"ContentPair.h"
ContentPair::ContentPair(string content,string value){
    this->Content=content;
    this->value=value;
}
string ContentPair::get_content(){
    return this->Content;
}
string ContentPair::get_value(){
    return this->value;
}
void ContentPair::set_value(string new_value){
    this->value=new_value;
}   
string ContentPair::merge_cpair(){
    string merged=this->Content+": ";
    merged+=this->value;
    merged+="\r\n\r\n";
    return merged;
}