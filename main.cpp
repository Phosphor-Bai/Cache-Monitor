#include "monitor.h"
#include <string>
#include <fstream>
using namespace std;

uint64_t string_to_int64(char* s){
    uint64_t value = 0;
    for(int i=0; i<strlen(s); i++){
        if(s[i]=='1'){  //一开始的0b会被记成0，没事
            value += 1;
        }
        value *= 2;
    }
    value /= 2;
    return value;
}


int main(){
    int block_size = 8;
    int way_num = 8;
    string ra_s = "BT";
    string wha_s = "WriteBack";
    string wma_s = "WriteAllocate";
    //TODO:new algorithm
    Replace_Algorithm ra = (ra_s == "BT") ? Replace_Algorithm::BT : Replace_Algorithm::LRU;
    Write_Hit_Algorithm wha = (wha_s == "WriteBack") ? Write_Hit_Algorithm::Write_Back : Write_Hit_Algorithm::Write_Through;
    Write_Miss_Algorithm wma = (wma_s == "WriteAllocate") ? Write_Miss_Algorithm::Write_Allocate : Write_Miss_Algorithm::No_Write_Allocate;

    Monitor monitor = Monitor(block_size, way_num, ra, wma, wha);

    string log_name = "./result/" + to_string(block_size) + "B_" + to_string(way_num) + "way_" + ra_s + "_" + wha_s + "_" + wma_s + "_trace1_log";
    ofstream outLog(log_name);
    ifstream inTrace("./test_trace/1.trace");
    char buffer[1024];
    bool result;
    int count = 0;
    int hit = 0;
    int miss = 0;
    while(inTrace>>buffer){
        if(buffer[0]=='\n' || buffer[0]==EOF){
            //cout<<"end of file!";
        }
        ////cout<<buffer<<endl; //地址
        uint64_t address = string_to_int64(buffer);
        ////cout<<bitset<sizeof(address)*8>(address)<<endl;
        inTrace>>buffer;
        ////cout<<buffer<<endl; //w/r
        count ++;
        if(count % 10000 == 0){
            cout<<count<<endl;
        }
        if(buffer[0]=='r'){
            result = monitor.read(address);
            //cout<<"result: "<<result<<endl;
            if(result){
                hit ++;
                outLog<<"Hit\n";
            }
            else{
                miss++;
                outLog<<"Miss\n";
            }
        }
        else if(buffer[0]=='w'){
            result = monitor.write(address);
            //cout<<"result: "<<result<<endl;
            if(result){
                hit ++;
                outLog<<"Hit\n";
            }
            else{
                miss++;
                outLog<<"Miss\n";
            }
        }
    }
    cout<<"visit: "<<count<<endl;
    cout<<"hit: "<<hit<<endl;
    cout<<"miss: "<<miss<<endl;
    inTrace.close();
    outLog.close();
    return 0;
}