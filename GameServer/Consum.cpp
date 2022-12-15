#include "pch.h"
#include "Consum.h"

Consum::Consum(int id) : MetaData(Protocol::DataType::DATA_TYPE_CONSUM, DataManager::ConsumMap.find(id)->second)
{

}


void Consum::Set(int count, Protocol::ConsumMetaData data)
{

	_count = count;
	//_data = data;
}
