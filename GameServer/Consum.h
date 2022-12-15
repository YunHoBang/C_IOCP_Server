#pragma once
#include "MetaData.h"

class Consum : public MetaData<Protocol::ConsumMetaData>
{
public:
	Consum(int id);

	virtual void Set(int count, Protocol::ConsumMetaData data);
public:
	int _count = 0;
	//int _maxCount = 10;

};

