#include <iostream>
#include <stdint.h>
#include <bitset>
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