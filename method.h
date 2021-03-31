#include <iostream>
using namespace std;

//替换算法的父类
class Method{
public:
    int set_num;
    int way_num;
    int statedata_length;
    uint8_t* statedata;
    Method(int _set_num, int _way_num){
        set_num = _set_num;
        way_num = _way_num;
    }
    ~Method(){}
    bool test(int k, int way);
    void set(int k, int way);
    void clear(int k, int way);
    //TODO: 参数？？？
    virtual void update();
    virtual int find_victim();
};

class BinaryTree: public Method{
public:
    BinaryTree(int _set_num, int _way_num);
    ~BinaryTree();
    void update();      //TODO:
    int find_victim();  //TODO:
};