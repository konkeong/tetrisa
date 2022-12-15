#include "mloopq.h"

LoopQueue::~LoopQueue()
{
}

LoopQueue::LoopQueue()
{
}

int LoopQueue::pos()
{
    return _pos;
}

int LoopQueue::num()
{
    return _num;
}

void LoopQueue::clear()
{
    _pos = 0;
    _num = 0;
}

int LoopQueue::push(int item)
{
	int jj;

	if (_num == 0)
		jj = _pos;
	else
		jj = _pos + _num;

	if (jj >= MAX_LQ)
		jj = jj - MAX_LQ;

	_data[jj] = item;
	_num = _num + 1;

	return item;
}

int LoopQueue::pop()
{
    int item;

	item = _data[_pos];

	if (_pos + 1 >= MAX_LQ)
		_pos = 0;
	else
		_pos = _pos + 1;
	_num = _num - 1;

    return item;
}

int LoopQueue::peek()
{
    int item;

	item = _data[_pos];

    return item;
}

bool LoopQueue::isInData(int item)
{
    int i;
	int jj;

	if (_num == 0)
		jj = _pos;
	else
		jj = _pos + _num;
    for (i=0; i<_num; i++)
    {
    	if (jj >= MAX_LQ)
	    	jj = jj - MAX_LQ;
        if (item == _data[jj])
            return true;
        jj = jj + 1;
    }

	return false;
}
