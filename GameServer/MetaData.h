#pragma once
#include "GameData.h"
#include "DataManager.h"

// T is MetaData
template< typename T >
class MetaData : public GameData
{
public:
	MetaData() = delete;
	MetaData(Protocol::DataType type, const T& data) : GameData(type) { SetMetaData(data); }
	virtual ~MetaData() {}

public:
	T GetMetaData() { return _data; }

private:
	void SetMetaData(const T& data) { _data = data; }
	T _data;

};
