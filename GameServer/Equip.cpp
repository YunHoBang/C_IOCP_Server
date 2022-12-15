#include "pch.h"
#include "Equip.h"

Equip::Equip(int id) : MetaData(Protocol::DataType::DATA_TYPE_EQUIP, DataManager::EquipMap.find(id)->second)
{

}
