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
    for(int i=width-1; i>=0; i--){
        if(test(way*width+i, index)){
            number += 1;
        }
        number *= 2;
    }
    return number / 2;
}

LeastRecentlyUsed::LeastRecentlyUsed(int _set_num, int _way_num): Method(_set_num, _way_num){
    width = 3;
    statedata_length = 3;
    statedata = new uint8_t[statedata_length*set_num];  //固定8-way
    //memset(statedata, 0, 3*set_num);
    for(int index=0; index<_set_num; index++){
        for(int way=0; way<_way_num; way++){    //一开始就按0~7写
            set_state(index, way, way);
        }
        //cout<<int(statedata[index*3])<<" "<<int(statedata[index*3+1])<<" "<<int(statedata[index*3+2])<<endl;
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
    cout<<int(statedata[index*3])<<" "<<int(statedata[index*3+1])<<" "<<int(statedata[index*3+2])<<endl;
    return -1;
}

void LeastRecentlyUsed::update(int index, int way){
    int former_value = get_state(index, way);
    //cout<<"former value: "<<former_value<<endl;
    //cout<<"bef: "<<int(statedata[index*3])<<" "<<int(statedata[index*3+1])<<" "<<int(statedata[index*3+2])<<endl;
    for(int w=0; w<way_num; w++){
        if(w==way){
            set_state(index, w, 7);     //放到栈底！！！
        }
        else if(get_state(index, w) > former_value){
            //cout<<w<<"-"<<get_state(index, w)<<": "<<int(statedata[index*3])<<" "<<int(statedata[index*3+1])<<" "<<int(statedata[index*3+2])<<endl;
            int new_value = get_state(index, w) - 1;
            set_state(index, w, new_value);
        }
        
    }
    //cout<<"aft: "<<int(statedata[index*3])<<" "<<int(statedata[index*3+1])<<" "<<int(statedata[index*3+2])<<endl;
    return;
}

void ProtectedLRU::set_state(int index, int way, int number, bool is_stack){
    int width = is_stack ? stack_width : count_width;
    int padding = is_stack ? 0 : 3;     //stack编号存在statedata的前半部分，计数存在后半部分
    for(int i=0; i<width; i++){
        if(number % 2 ==0){
            clear(way*statedata_length+padding+i, index);
        }
        else{
            set(way*statedata_length+padding+i, index);
        }
        number = number/2;
    }
}

int ProtectedLRU::get_state(int index, int way, bool is_stack){
    int width = is_stack ? stack_width : count_width;
    int padding = is_stack ? 0 : 3;
    int number = 0;
    for(int i=width-1; i>=0; i--){
        if(test(way*statedata_length+padding+i, index)){
            number += 1;
        }
        number *= 2;
    }
    return number / 2;
}


ProtectedLRU::ProtectedLRU(int _set_num, int _way_num): Method(_set_num, _way_num){
    set_num = _set_num;
    way_num = _way_num;
    stack_width = int_log2(way_num);
    count_width = 3;
    protect_num = 6;
    statedata_length = stack_width+count_width;
    cout<<"statedata_length"<<statedata_length<<endl;
    statedata = new uint8_t[statedata_length*set_num];

    for(int index=0; index<_set_num; index++){
        for(int way=0; way<_way_num; way++){    //一开始就按0~7写
            set_state(index, way, way, true);
            set_state(index, way, 0, false);
        }
        //cout<<int(statedata[index*3])<<" "<<int(statedata[index*3+1])<<" "<<int(statedata[index*3+2])<<endl;
    }
}

ProtectedLRU::~ProtectedLRU(){
    delete[] statedata;
}

int ProtectedLRU::find_victim(int index){
    int LRU_choice1 = 0, LRU_choice2 = 1;

    for(int way=2; way<way_num; way++){
        if((get_state(index, way, false)<get_state(index, LRU_choice1, false)) || (get_state(index, way, false)<get_state(index, LRU_choice2, false))){
            LRU_choice1 = (get_state(index, LRU_choice1, false)<get_state(index, LRU_choice2, false)) ? LRU_choice1 : LRU_choice2;
            LRU_choice2 = way;
        }
    }
    int victim = (get_state(index, LRU_choice1, true)<get_state(index, LRU_choice2, true)) ? LRU_choice1 : LRU_choice2;
    set_state(index, victim, 0, false);     //清零victim的count
    return victim;
}

void ProtectedLRU::update(int index, int way){
    int former_value = get_state(index, way, true);
    //修改栈index
    for(int w=0; w<way_num; w++){
        if(w==way){
            set_state(index, w, 7, true);     //放到栈底！！！
        }
        else if(get_state(index, w, true) > former_value){
            //cout<<w<<"-"<<get_state(index, w)<<": "<<int(statedata[index*3])<<" "<<int(statedata[index*3+1])<<" "<<int(statedata[index*3+2])<<endl;
            int new_value = get_state(index, w, true) - 1;
            set_state(index, w, new_value, true);
        }
    }
    //修改count
    int count = get_state(index, way, false);
    count ++;
    if(count==8){
        count = 4;
        for(int w=0; w<way_num; w++){
            if(w==way){
                set_state(index, w, count, false);
            }
            else{
                set_state(index, w, (get_state(index, w, false)>>1), false);
            }
        }
    }
    else{
        set_state(index, way, count, false);
    }
    return;
}
