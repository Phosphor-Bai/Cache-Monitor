#include <string.h>
#include "method.h"
#ifndef H_FILES
#define H_FILES
    #include "tools.h"
#endif
using namespace std;

//Tools for statedata
bool Method::test(int k, int set){
    uint8_t* M = statedata + set * statedata_length;
    return M[k>>3] & (0x80 >> (k & 0x07));
}
void Method::set(int k, int set){
    uint8_t* M = statedata + set * statedata_length;
    M[k>>3] |= (0x80 >> (k & 0x07));
}
void Method::clear(int k, int set){
    uint8_t* M = statedata + set * statedata_length;
    M[k>>3] &= ~(0x80 >> (k & 0x07));
}

BinaryTree::BinaryTree(int _set_num, int _way_num): Method(_set_num, _way_num){
    statedata_length = (_way_num+7) / 8;
    step = int_log2(way_num);
    statedata = new uint8_t[statedata_length*set_num];  //每一组有一个二叉树
    memset(statedata, 0, statedata_length*set_num);
}

BinaryTree::~BinaryTree(){
    delete[] statedata;
}

int BinaryTree::find_victim(int index){
    //若从1开始计数的话是对的，最后再减个1应该就行了（毕竟statedata是从0开始的）
    //TODO:检查正确性
    int selected_way_plus_one = 1;
    for(int current_step=0; current_step<step; current_step++){
        if(test(selected_way_plus_one-1, index)){   //节点上记的是1，右拐
            selected_way_plus_one *= 2;             //记得先读0还是1，然后×2
            selected_way_plus_one += 1;
        }
        else{
            selected_way_plus_one *= 2;
        } 
    }
    return (selected_way_plus_one - way_num);
}

void BinaryTree::update(int index, int way){
    int target_plus_one = way + way_num;
    for(int current_step=0; current_step<step; current_step++){
        bool origin_direct = (target_plus_one % 2 == 1);
        bool set_direct = !origin_direct;
        target_plus_one = target_plus_one / 2;
        if(set_direct){
            set(target_plus_one-1, index);
        }
        else{
            clear(target_plus_one-1, index);
        }
    }
}


void LeastRecentlyUsed::set_state(int index, int way, int number){
    for(int i=0; i<width; i++){
        if(number % 2 ==0){
            clear(way*width+i, index);
        }
        else{
            set(way*width+i, index);
        }
        number = number/2;
    }
}

int LeastRecentlyUsed::get_state(int index, int way){
    int number = 0;
    for(int i=0; i<width; i++){
        if(test(way*width+i, index)){
            number += 1;
        }
        number *= 2;
    }
    return number;
}

LeastRecentlyUsed::LeastRecentlyUsed(int _set_num, int _way_num): Method(_set_num, _way_num){
    width = 3;
    statedata_length = 3;
    statedata = new uint8_t[3*set_num];  //固定8-way
    memset(statedata, 0, 3*set_num);
    for(int index=0; index<_set_num; index++){
        for(int way=0; way<_way_num; way++){    //一开始就按0~7写
            set_state(index, way, way);
        }
    }
}

LeastRecentlyUsed::~LeastRecentlyUsed(){
    delete[] statedata;
}

int LeastRecentlyUsed::find_victim(int index){
    for(int way=0; way<way_num; way++){
        if(get_state(index, way) == 0){
            return way;
        }
    }
    cout<<"error in LRU stack (index: "<<index<<")"<<endl;
    return -1;
}

void LeastRecentlyUsed::update(int index, int way){
    int former_value = get_state(index, way);
    for(int w=0; w<way_num; w++){
        if(w==way){
            set_state(index, w, 0);
        }
        else if(get_state(index, w) < former_value){
            int new_value = get_state(index, w) + 1;
            set_state(index, w, new_value);
        }
    }
    return;
}