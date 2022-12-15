#pragma once
#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "BufferWriter.h"
#include "ClientPacketHandler.h"
#include <tchar.h>
#include "Protocol.pb.h"
#include "Job.h"
#include "Room.h"
#include "Managers.h"
#include "Player.h"
#include "DBConnectionPool.h"
#include "DBBind.h"
#include "XmlParser.h"
#include "DBSynchronizer.h"
#include "GenProcedures.h"

#include <cstdlib>
#include <ctime> 
enum
{
	WORKER_TICK = 64
};

void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		// 네트워크 입출력 처리 -> 인게임 로직까지 (패킷 핸들러에 의해)
		service->GetIocpCore()->Dispatch(10);

		// 예약된 일감 처리
		ThreadManager::DistributeReservedJobs();

		// 글로벌 큐
		ThreadManager::DoGlobalQueueWork();
	}
}

int main()
{
	cout << endl;
	cout << "Server Start" << endl;
	srand((unsigned int)time(NULL));

	//ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={SQL Server Native Client 11.0};Server=(localdb)\\MSSQLLocalDB;Database=ServerDb;Trusted_Connection=Yes;"));
	ASSERT_CRASH(GDBConnectionPool->Connect(10, L"Driver={SQL Server Native Client 11.0};Server=DESKTOP-SA0K1AE\\SQLEXPRESS01;Database=ServerDb;Trusted_Connection=Yes;"));

	DBConnection* dbConn = GDBConnectionPool->Pop();
	DBSynchronizer dbSync(*dbConn);
	dbSync.Synchronize(L"GameDB.xml");

	// Init MapManagers
	GMapManager->Init();
	DataManager::LoadData();

	ClientPacketHandler::Init();

	// 임시 // 나중에 수정해야함
	GRoom->Init();

	ServerServiceRef service = MakeShared<ServerService>(
	//	NetAddress(L"127.0.0.1", 7777),
		NetAddress(L"0.0.0.0", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, // TODO : SessionManager 등
		100);

	ASSERT_CRASH(service->Start());

	for (s_int32 i = 0; i < 10; i++)
	{
		GThreadManager->Launch([&service]()
			{
				srand((unsigned int)time(NULL));
				DoWorkerJob(service);
			});
	}

	// Main Thread
	DoWorkerJob(service);

	GThreadManager->Join();

}