#pragma once
#include "MapManager.h"
#include "DataManager.h"

extern class MapManager* GMapManager;

class Managers
{
public:
	Managers()
	{
		GMapManager = new MapManager();
	}

	~Managers()
	{
		delete GMapManager;
	}
};
extern Managers GManagers;