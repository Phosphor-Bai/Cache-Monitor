#include <iostream>
#include <stdint.h>
#include <string.h>
#include "method.h"
using namespace std;

const int cache_size = (1<<17);

enum class Replace_Algorithm{
    BT, LRU, PLRU
};
enum class Write_Hit_Algorithm{
    Write_Through, Write_Back
};
enum class Write_Miss_Algorithm{
    Write_Allocate, No_Write_Allocate
};

class Monitor{
private:
    //cache settings
    int block_size;
    int set_num;            //组数
    int way_num;            //路数
    Write_Miss_Algorithm write_miss_algorithm;
    Write_Hit_Algorithm write_hit_algorithm;
    //address，从0开始编号
    int addr_tag_start;
    int addr_index_start;
    //metadata
    uint8_t* metadata;
    int metadata_num;
    int metadata_length;    //uint8的个数
    int dirty_pos;
    int tag_length;
    int metadata_tag_start_pos;
    int valid_pos;
    //find result
    bool hit;
    bool has_empty_line;
    //replace method
    Method* method;

    //仿bitmap访存每一个metadata（根据第x组第y路定位到这个metadata，把它当做一个bitmap）
    bool test(int k, int set, int way);
    void set(int k, int set, int way);
    void clear(int k, int set, int way);
    //处理地址
    uint64_t get_tag(uint64_t address);
    uint64_t get_index(uint64_t address);
    int find_in_cache(uint64_t address, bool write=false);     //返回-1则为没找到且没空位，否则返回的可能是hit的也可能是invalid的，看hit和has_empty_line
    void replace_in_cache(uint64_t address, int way, bool write=false);  
public:
    Monitor(int _block_size=8, int _way_num=8, Replace_Algorithm ra=Replace_Algorithm::BT, Write_Miss_Algorithm wma=Write_Miss_Algorithm::Write_Allocate, Write_Hit_Algorithm wha=Write_Hit_Algorithm::Write_Back);    
    ~Monitor();
    bool read(uint64_t address);
    bool write(uint64_t address);
};