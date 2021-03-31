#include "monitor.h"
using namespace std;

int int_log2(int number){
    int result = 0;
    if(number<0){
        cout<<"error: log2 with negative input"<<endl;
        return 0;
    }
    while(number!=1){
        if(number%2==1){
            cout<<"error: log2 encountering odd number"<<endl;
            return 0;
        }
        number = number/2;
        result++;
    }
    return result;
}

void print_2(uint64_t number){
    cout<<bitset<sizeof(number)*8>(number)<<endl;
}

Monitor::Monitor(int _block_size, int _way_num, Replace_Algorithm ra, Write_Miss_Algorithm wma, Write_Hit_Algorithm wha){
    block_size = _block_size;
    way_num = _way_num;
    write_hit_algorithm = wha;
    write_miss_algorithm = wma;

    //元数据
    //128K=2^17=set_num*way_num*block_size
    //medatada个数: 共set_num*way_num个，每个要包含dirty, tag, valid
    //metadata长度：(Dirty→1)+64-log2(set_num*block_size)+Valid→1
    //(metadata长度+7)/8为所需uint8个数
    //TODO:
    set_num = cache_size/(way_num*block_size);
    cout<<way_num<<"-way cache with block_size "<<block_size<<endl;
    cout<<"algorithms:"<<endl;
    if(wha==Write_Hit_Algorithm::Write_Back){
        cout<<"-Write Back"<<endl;
    }
    else{
        cout<<"-Write Through"<<endl;
    }
    if(wma==Write_Miss_Algorithm::Write_Allocate){
        cout<<"-Write Allocate"<<endl;
    }
    else{
        cout<<"-Not Write Allocate"<<endl;
    }

    //metadata
    metadata_num = set_num*way_num;
    metadata_length = 1;        //Valid
    if(wha==Write_Hit_Algorithm::Write_Back){
        metadata_length++;      //Dirty
    }
    tag_length = (64-int_log2(set_num*block_size));
    metadata_length += tag_length;   //Tag = Whole-Index-Offset
    cout<<"metadata length in bits: "<<metadata_length<<endl;
    metadata_length = (metadata_length+7)/8;
    cout<<"metadata length in char: "<<metadata_length<<endl;
    dirty_pos = metadata_length*8-1;
    metadata_tag_start_pos = dirty_pos-tag_length;
    valid_pos = 0;
    metadata = new uint8_t[metadata_num*metadata_length];
    memset(metadata, 0, metadata_num*metadata_length);

    //address
    addr_index_start = int_log2(block_size);
    addr_tag_start = 64 - tag_length;
    cout<<"address: |63--tag--"<<addr_tag_start<<"|"<<(addr_tag_start-1)<<"--index--"<<addr_index_start<<"|"<<(addr_index_start-1)<<"--offset--0|"<<endl;

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

Monitor::~Monitor(){
    delete[] metadata;
}

//Function
int Monitor::find_in_cache(uint64_t address, bool write){
    uint64_t tag = get_tag(address);
    uint64_t index = get_index(address);
    cout<<"address: ";print_2(address);
    cout<<"tag: ";print_2(address);cout<<"index: ";print_2(index);
    hit = false;
    has_empty_line = false;
    int first_invalid_way;
    for(int way=0; way<way_num; way++){       //比较每一路
        bool hit_here = true;
        for(int k=metadata_tag_start_pos; k<metadata_tag_start_pos+tag_length;k++){ //比较tag的每一位
            bool bit = (tag % 2 == 1);
            if(test(k, index, way)!=bit){
                hit_here = false;
                break;
            }
            tag = tag >> 1;
        }
        if(hit_here){
            hit = test(valid_pos, index, way);
            return way;
        }
        else{
            if((!has_empty_line) && test(valid_pos, index, way)==0){
                has_empty_line = true;
                first_invalid_way = way;
            }
        }
    }
    hit = false;
    return first_invalid_way;
}


//Tools for metadata
bool Monitor::test(int k, int set, int way){
    uint8_t* M = metadata + (set * way_num + way) * metadata_length;
    return M[k>>3] & (0x80 >> (k & 0x07));
}
void Monitor::set(int k, int set, int way){
    uint8_t* M = metadata + (set * way_num + way) * metadata_length;
    M[k>>3] |= (0x80 >> (k & 0x07));
}
void Monitor::clear(int k, int set, int way){
    uint8_t* M = metadata + (set * way_num + way) * metadata_length;
    M[k>>3] &= ~(0x80 >> (k & 0x07));
}

//Tools for address
uint64_t Monitor::get_tag(uint64_t address){
    return (address>>addr_tag_start);
}
uint64_t Monitor::get_index(uint64_t address){
    uint64_t mask = (1<<addr_tag_start) -1;
    address = address & mask;
    return (address>>addr_index_start);
}
