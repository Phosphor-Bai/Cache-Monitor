#include <iostream>
#include "method.h"
using namespace std;

const int cache_size = (1<<17);

enum struct Replace_Algorithm{
    BT, LRU
};
enum struct Write_Hit_Algorithm{
    Write_Through, Write_Back
};
enum struct Write_Miss_Algorithm{
    Write_Allocate, No_Write_Allocate
};

int int_log2(int number);

class Monitor{
private:
    //cache settings
    int block_size;
    int set_num;            //组数
    int way_num;            //路数
    Write_Miss_Algorithm write_miss_algorithm;
    Write_Hit_Algorithm write_hit_algorithm;
    //metadata
    uint8_t* metadata;
    int metadata_num;
    int metadata_length;    //uint8的个数
    int dirty_pos;
    int tag_start_pos;
    int valid_pos;
    //replace method
    Method* method;
public:
    Method method;
    Monitor(int _block_size=8, int _way_num=8, Replace_Algorithm ra=Replace_Algorithm::BT, Write_Miss_Algorithm wma=Write_Miss_Algorithm::Write_Allocate, Write_Hit_Algorithm wha=Write_Hit_Algorithm::Write_Back);
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