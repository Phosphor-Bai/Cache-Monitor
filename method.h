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
    ~Method(){;}
    bool test(int k, int way);
    void set(int k, int way);
    void clear(int k, int way);
    //解决undefined reference to `vtable for Method'的方法：改为纯虚函数
    virtual int find_victim(int index) = 0;
    virtual void update(int index, int way) = 0;
};

class BinaryTree: public Method{
    int step;
public:
    BinaryTree(int _set_num, int _way_num);
    ~BinaryTree();
    void update();
    int find_victim(int index);
    void update(int index, int way);
};