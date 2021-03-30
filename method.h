#include <iostream>
using namespace std;

//替换算法的父类
class Method{
public:
    Method(){}
    ~Method(){}
    //TODO: 参数？？？
    virtual void update();
    virtual int find_victim();
};