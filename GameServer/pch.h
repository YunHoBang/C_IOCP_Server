#pragma once

#define WIN32_LEAN_AND_MEAN // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#ifdef _DEBUG
#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#else
#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#endif

#include "CorePch.h"
#include "Enum.pb.h"

using GameSessionRef = shared_ptr<class GameSession>;
using PlayerRef = shared_ptr<class Player>;
using EnemyRef = shared_ptr<class Enemy>;
using NPCRef = shared_ptr<class NPC>;
using ItemRef = shared_ptr<class Item>;
//using MonsterRef = shared_ptr<class Monster>;
using GameObjectRef = shared_ptr<class GameObject>;
using ArrowRef = shared_ptr<class Arrow>;
using ProjectileRef = shared_ptr<class Projectile>;
using GameDataRef = shared_ptr<class GameData>;
using ItemSlotRef = shared_ptr<class ItemSlot>;
using YHSkillRef = shared_ptr<class YHSkill>;
using SummonsRef = shared_ptr<class Summons>;
using RoomRef = shared_ptr<class Room>;

#include "Vector3.h"
#include "Vector2.h"
#include "YHUtils.h"

using namespace ServerCore;

#define PI 3.14159265
#define GRAVITY 9.8