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

Monitor::Monitor(int _block_size=8, int _way_num=8, Replace_Algorithm ra=Replace_Algorithm::BT, Write_Miss_Algorithm wma=Write_Miss_Algorithm::Write_Allocate, Write_Hit_Algorithm wha=Write_Hit_Algorithm::Write_Back){
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
    set_num = cache_size/(way_num*block_size);
    metadata_num = set_num*way_num;
    metadata_length = 1;        //Valid
    if(wha==Write_Hit_Algorithm::Write_Back){
        metadata_length++;      //Dirty
    }
    metadata_length += (64-int_log2(set_num*block_size));   //Tag = Whole-Index-Offset
    cout<<"metadata length in bits: "<<metadata_length<<endl;
    

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
