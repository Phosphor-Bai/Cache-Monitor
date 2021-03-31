#include "method.h"
#include "tools.h"
#include <string.h>
using namespace std;

//Tools for statedata
bool Method::test(int k, int way){
    uint8_t* M = statedata + way * statedata_length;
    return M[k>>3] & (0x80 >> (k & 0x07));
}
void Method::set(int k, int way){
    uint8_t* M = statedata + way * statedata_length;
    M[k>>3] |= (0x80 >> (k & 0x07));
}
void Method::clear(int k, int way){
    uint8_t* M = statedata + way * statedata_length;
    M[k>>3] &= ~(0x80 >> (k & 0x07));
}

BinaryTree::BinaryTree(int _set_num, int _way_num): Method(_set_num, _way_num){
    statedata_length = (_way_num+7) / 8;
    statedata = new uint8_t[statedata_length*set_num];
    memset(statedata, 0, statedata_length*set_num);
}

BinaryTree::~BinaryTree(){
    delete[] statedata;
}

int BinaryTree::find_victim(){

}