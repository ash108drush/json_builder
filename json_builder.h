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
        CheckValidCall();
        in_dict_.push_back(dict_level_);
        json::Dict dict;
        Node* node = new Node(dict);
        nodes_stack_.push_back(node);
        return *this;

    };
    Builder& EndDict(){
        if(in_dict_.empty()){
            throw std::logic_error("Dict not started");
        }
        size_t counter = 0;
        int skip = in_dict_[0];
        in_dict_.erase(in_dict_.begin());
        int step = 0;
        Node * current_node = nullptr;
        auto iter_array = nodes_stack_.rbegin();
        for(;iter_array != nodes_stack_.rend();++iter_array){
            current_node = *iter_array;
            if(current_node->IsDict()){
                if(step == skip){
                    break;
                }
                ++step;
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
        CheckValidCall();
        if(in_dict_.size() >0){
            std::fill(in_dict_.begin(), in_dict_.end(), 0);
        }
        in_array_.push_back(in_array_.size());
        json::Array  array;
        Node* node = new Node(array);
        nodes_stack_.push_back(node);
        return *this;
    }
    Builder& EndArray(){
        if(in_array_.empty()){
            throw std::logic_error("Array not started");
        }

        size_t counter = 0;
        Node * current_node = nullptr;
        int skip = in_array_[0];
        in_array_.erase(in_array_.begin());
        int step = 0;
        auto iter_array = nodes_stack_.rbegin();
        for(;iter_array != nodes_stack_.rend();++iter_array){
            current_node = *iter_array;
            if(current_node->IsArray()){
                if(step == skip){
                    break;
                }
                ++step;
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
        if(in_dict_.empty()){
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
        CheckValidCall();
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
    bool CheckValidCall(){
        if(first_call_){
            first_call_ = false;
            return true;
        }
        if( after_key_ ){
            return true;
        }
        if(!in_array_.empty()){
            return true;
        }
        throw std::logic_error("Неверный вызов");

    }
    bool first_call_ = true;
    bool after_key_ = false;
    std::vector<int> in_array_;
    std::vector<int> in_dict_;
    int dict_level_ = 0;
    Node root_;
    std::vector<Node*> nodes_stack_;

};

}
