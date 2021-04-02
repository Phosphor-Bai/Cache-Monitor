#include "monitor.h"
#ifndef H_FILES
#define H_FILES
    #include "tools.h"
#endif
using namespace std;

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
    set_num = cache_size/(way_num*block_size);
    //cout<<way_num<<"-way cache with block_size "<<block_size<<endl;
    //cout<<"algorithms:"<<endl;
    if(wha==Write_Hit_Algorithm::Write_Back){
        //cout<<"-Write Back"<<endl;
    }
    else{
        //cout<<"-Write Through"<<endl;
    }
    if(wma==Write_Miss_Algorithm::Write_Allocate){
        //cout<<"-Write Allocate"<<endl;
    }
    else{
        //cout<<"-Not Write Allocate"<<endl;
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
    //cout<<"metadata length in char: "<<metadata_length<<endl;
    if(wha==Write_Hit_Algorithm::Write_Back){
        dirty_pos = metadata_length*8-1;
        metadata_tag_start_pos = dirty_pos-tag_length;
        valid_pos = 0;
        cout<<"metadata: |D|"<<dirty_pos-1<<"-tag-"<<metadata_tag_start_pos<<"|...|V|"<<endl; 
    }
    else{
        dirty_pos = metadata_length * 8;    //没有dirty位
        metadata_tag_start_pos = dirty_pos-tag_length;
        valid_pos = 0;
        cout<<"metadata: |"<<dirty_pos-1<<"-tag-"<<metadata_tag_start_pos<<"|...|V|"<<endl; 
    }
    metadata = new uint8_t[metadata_num*metadata_length];
    memset(metadata, 0, metadata_num*metadata_length);

    //address
    addr_index_start = int_log2(block_size);
    addr_tag_start = 64 - tag_length;
    //cout<<"address: |63--tag--"<<addr_tag_start<<"|"<<(addr_tag_start-1)<<"--index--"<<addr_index_start<<"|"<<(addr_index_start-1)<<"--offset--0|"<<endl;

    //替换策略
    if(ra==Replace_Algorithm::BT){
        //cout<<"replace algorithm: Binary Tree"<<endl;
        method = new BinaryTree(set_num, way_num);
    }
    else if(ra==Replace_Algorithm::LRU){
        //cout<<"replace algorithm: Least Recently Used"<<endl;
        method = new LeastRecentlyUsed(set_num, way_num);
    }
    else{
        method = new ProtectedLRU(set_num, way_num);
    }
}

Monitor::~Monitor(){
    delete[] metadata;
    delete method;
}

//Function
bool Monitor::read(uint64_t address){
    int index = get_index(address);
    int way = find_in_cache(address, false);
    /*
    if(index==592){
        cout<<"address: "<<address<<endl;
        cout<<"way: "<<way<<endl;
        cout<<"hit: "<<hit<<endl;
        cout<<"has empty: "<<has_empty_line<<endl;
        cout<<"tag: "<<get_tag(address)<<endl;
        cout<<"tag_start: "<<metadata_tag_start_pos<<endl;
        for(int w=0; w<way_num; w++){
            cout<<"metadata: ";
            for(int i=0; i<metadata_length; i++){
                uint8_t number = metadata[(index*way_num+w)*metadata_length+i];
                cout<<bitset<sizeof(number)*8>(number)<<" ";
            }
            cout<<endl;
        }
    }
    */

    if(hit){
        method->update(index, way);
        return true;
    }
    else if(has_empty_line){
        replace_in_cache(address, way, false);
        method->update(index, way);
        return false;
    }
    else{
        way = method->find_victim(index);
        replace_in_cache(address, way, false);
        method->update(index, way);
        return false;
    }
}

bool Monitor::write(uint64_t address){
    int index = get_index(address);
    int way = find_in_cache(address, true);
    /*
    if(index==592){
        cout<<"address: "<<address<<endl;
        cout<<"way: "<<way<<endl;
        cout<<"hit: "<<hit<<endl;
        cout<<"has empty: "<<has_empty_line<<endl;
        cout<<"index: "<<index;
        cout<<"tag: "<<get_tag(address)<<endl;
        cout<<"tag_start: "<<metadata_tag_start_pos<<endl;
        
        for(int w=0; w<way_num; w++){
            cout<<"metadata: ";
            for(int i=0; i<metadata_length; i++){
                uint8_t number = metadata[(index*way_num+w)*metadata_length+i];
                cout<<bitset<sizeof(number)*8>(number)<<" ";
            }
            cout<<endl;
        }   
    }
    */

    if(hit){
        //dirty位在find_in_cache里解决
        method->update(index, way);
        return true;
    }
    else if(write_miss_algorithm==Write_Miss_Algorithm::No_Write_Allocate){
        return false;
    }
    else if(has_empty_line){
        //dirty位在replace_in_cache中解决
        replace_in_cache(address, way, true);
        method->update(index, way);
        return false;
    }
    else{
        //dirty位在replace_in_cache中解决
        way = method->find_victim(index);
        replace_in_cache(address, way, true);
        method->update(index, way);
        return false;
    }
}


//Operate in metadata
int Monitor::find_in_cache(uint64_t address, bool write){
    uint64_t index = get_index(address);
    //cout<<"address: ";print_2(address);
    //cout<<"tag: ";print_2(tag);//cout<<"index: ";print_2(index);
    hit = false;
    has_empty_line = false;
    int first_invalid_way = -1;
    for(int way=0; way<way_num; way++){       //比较每一路
        uint64_t tag = get_tag(address);
        bool hit_here = true;
        for(int k=metadata_tag_start_pos; k<metadata_tag_start_pos+tag_length;k++){ //比较tag的每一位
            bool bit = (tag % 2 == 1);
            //if(index==1462){
            //    cout<<way<<" "<<bit<<" "<<test(k, index, way)<<endl;
            //}
            
            if(test(k, index, way)!=bit){
                hit_here = false;
                break;
            }
            tag = tag >> 1;
        }
        if(hit_here){
            hit = test(valid_pos, index, way);
            if(write && (write_hit_algorithm==Write_Hit_Algorithm::Write_Back)){
                set(dirty_pos, index, way);
            }
            has_empty_line = true;  //如果没有hit的话那个invalid的位必可以填入新的
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

void Monitor::replace_in_cache(uint64_t address, int way, bool write){
    uint64_t tag = get_tag(address);
    uint64_t index = get_index(address);
    for(int k=metadata_tag_start_pos; k<metadata_tag_start_pos+tag_length;k++){ //比较tag的每一位
        bool bit = (tag % 2 == 1);
        if(bit){
            set(k, index, way);
        }
        else{
            clear(k, index, way);
        }
        tag = tag >> 1;
    }
    set(valid_pos, index, way);
    //No Write Allocate在write里解决
    if(write_hit_algorithm==Write_Hit_Algorithm::Write_Back){
        if(write) set(dirty_pos, index, way);
        else clear(dirty_pos, index, way);
    }
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
