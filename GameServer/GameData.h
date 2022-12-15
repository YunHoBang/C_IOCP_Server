#pragma once

#include "Enum.pb.h"
#include "Protocol.pb.h"
#include "Struct.pb.h"

class GameData
{
public:
	GameData() {}
	GameData(Protocol::DataType type) : _type(type) {}
	virtual ~GameData() {}

public:
	Protocol::DataType GetDataType() { return _type; }

private:
	Protocol::DataType _type = Protocol::DATA_TYPE_NONE;
};

