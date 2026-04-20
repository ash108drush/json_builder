#include "json.h"
#include <string.h>
#include <vector>
#include <iostream>
#include <map>
namespace json {

class Builder{
public:
    Builder() = default;
    ~Builder() = default;
    Builder& StartDict(){
        CheckValidCall("start dict");

        after_key_= false;
        dict_start_.push_back(true);

        json::Dict dict;
        dict.insert({"not initialized","true"});
        Node* node = new Node(dict);
        nodes_stack_.push_back(node);
        return *this;

    };
    Builder& EndDict(){

        if(dict_start_.empty()){
            throw std::logic_error("Dict not started" );
        }
        dict_start_.pop_back();
        size_t counter = 0;
        Node * current_node = nullptr;
        auto iter_array = nodes_stack_.rbegin();
        for(;iter_array != nodes_stack_.rend();++iter_array){
            current_node = *iter_array;
            if(current_node->IsDict()){
                Dict dict = current_node->AsDict();
                if(!dict.empty()){
                    if(dict.find("not initialized") != dict.end()){
                        break;
                    }
                }
            }
            ++counter;
        }

        if(counter == nodes_stack_.size()) {
            throw std::logic_error("Не найден StartDict");
            return *this;
        }

        //nodes_stack_.begin() +(nodes_stack_.size() - counter)
        json::Dict dict;
        size_t el_counter = 0;
        bool key = true;
        Node* key_node = nullptr;
        Node* value_node;

        for(auto iter =nodes_stack_.end()  - counter ; iter != nodes_stack_.end(); ++iter){
            if(key){
                key_node = *iter;
                key = false;
            }else if(!key){
                value_node = *iter;
                if(key_node->IsString()){
                    dict.insert({key_node->AsString(),*value_node});
                    key_node = nullptr;
                    key = true;
                }
            }
            ++el_counter;
        }
        // nodes_stack_.erase(nodes_stack_.end()  - counter);

        Node* node = new Node(dict);
        *iter_array = node;
        //nodes_stack_.erase(nodes_stack_.end()  - el_counter);
        nodes_stack_.resize(nodes_stack_.size() - el_counter);
        //std::cout << el_counter << " size: "<<nodes_stack_.size();

        return *this;
    };
    Builder& StartArray(){
        CheckValidCall("start array");
        array_start_.push_back(true);
        json::Array  array{"not initialized"};
        Node* node = new Node(array);
        nodes_stack_.push_back(node);
        after_key_ = false;
        return *this;
    }
    Builder& EndArray(){

        if(array_start_.empty()){
            throw std::logic_error("Array not started" );
        }
        array_start_.pop_back();

        size_t counter = 0;
        Node * current_node = nullptr;

        auto iter_array = nodes_stack_.rbegin();
        for(;iter_array != nodes_stack_.rend();++iter_array){
            current_node = *iter_array;
            if(current_node->IsArray()){
                Array arr = current_node->AsArray();
                if(!arr.empty()){
                    if(current_node->AsArray()[0] == "not initialized"){
                        break;
                    }
                }

            }
            ++counter;
        }


        if(counter == nodes_stack_.size()) {
            throw std::logic_error("Не найден StartArray");
            return *this;
        }
        //nodes_stack_.begin() +(nodes_stack_.size() - counter)
        json::Array array;
        size_t el_counter = 0;
        //if(counter == 1) counter=+2;
            for(auto iter =nodes_stack_.end()  - counter ; iter != nodes_stack_.end(); ++iter){
                Node *node = *iter;
                array.push_back(*node);
                ++el_counter;
            }

            Node* node = new Node(array);
           // ++iter_array;
            *iter_array = node;
            nodes_stack_.resize(nodes_stack_.size() - el_counter);


        return *this;
    };
    Builder& Key(std::string key ){
        if(dict_start_.empty()){
            throw std::logic_error("Key not in dict");
        }
        if(after_key_){
            throw std::logic_error("Key after key");
        }
        Node* node = new Node(key);
        nodes_stack_.push_back(node);
        after_key_ = true;
        return *this;

    }
    Builder& Value(Node node){
        //array_start_ = false;
        CheckValidCall("add value");
        Node* node_ptr = new Node(node);
        nodes_stack_.push_back(node_ptr);
        after_key_ = false;
        return *this;
    };
    Node Build(){
        if(nodes_stack_.size() == 1){
            root_ = *nodes_stack_[0];
            return root_;
        }

        throw std::logic_error("Нулевой стэк");
    }

private:
    bool CheckValidCall(std::string origin){

        if(first_call_){
            first_call_ = false;
            return true;
        }
        if( after_key_ ){
            return true;
        }
        if(!array_start_.empty() ){
            return true;
        }
    std::cout << first_call_ <<after_key_ << array_start_.empty() << std::endl;
        throw std::logic_error("Неверный вызов " + origin);

    }
    bool first_call_ = true;
    bool after_key_ = false;
    std::vector<bool> array_start_;
    std::vector<bool> dict_start_;

    Node root_;
    std::vector<Node*> nodes_stack_;

};

}
