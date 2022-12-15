#ifndef _MLOOPQ_H_
#define _MLOOPQ_H_

#define MAX_LQ 8

class LoopQueue
{
private:
    int _data[MAX_LQ];
    int _pos;
    int _num;
public:
    ~LoopQueue();
    LoopQueue();
    int pos();
    int num();
    void clear();
    int push(int item);
    int pop();
    int peek();
    bool isInData(int item);
};

#endif  /* _MLOOPQ_H_ */

