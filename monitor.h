#include <iostream>
#include "method.h"
using namespace std;

enum struct Replace_Algorithm{
    BT, LRU
};
enum struct Write_Hit_Algorithm{
    Write_Through, Write_Back
};
enum struct Write_Miss_Algorithm{
    Write_Allocate, No_Write_Allocate
};

int log2(int number);       //TODO:

class Monitor{
private:
    //parameters
    int block_size;
    int way_num;
    Write_Miss_Algorithm write_miss_algorithm;
    Write_Hit_Algorithm write_hit_algorithm;
    //components
    uint8_t* metadata;
    Method* method;
public:
    Method method;
    Monitor(int _block_size=8, int _way_num=8, Replace_Algorithm ra=Replace_Algorithm::BT, Write_Miss_Algorithm wma=Write_Miss_Algorithm::Write_Allocate, Write_Hit_Algorithm wha=Write_Hit_Algorithm::Write_Back){
        block_size = _block_size;
        way_num = _way_num;
        write_miss_algorithm = wma;
        write_hit_algorithm = wha;

        //元数据
        //128K=2^17=set_num*way_num*block_size
        //medatada个数: 共set_num*way_num个，每个要包含dirty, tag, valid
        //metadata长度：(Dirty→1)+64-log2(set_num*block_size)+Valid→1
        //(metadata长度+7)/8为所需uint8个数
        //TODO:

        //替换策略
        if(ra==Replace_Algorithm::BT){
            //TODO:
        }
        else if(ra==Replace_Algorithm::LRU){
            //TODO:
        }
        else{
            //TODO:
        }
    }
    ~Monitor(){}
    bool read(uint64_t address){
        return false;
    }
    bool write(uint64_t address){
        return false;
    }
    bool find_in_cache(uint64_t address);
    int find_empty_line();      //看看有没有空行，如果没有则返回-1
    void replace();             //填充空位和替换共用一个函数，里面记得update一下method
};