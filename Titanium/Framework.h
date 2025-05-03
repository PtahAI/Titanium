#pragma once

#include "Windows.h"
#include "Globals.h"
#include <iostream>

#include "minhook/MinHook.h"
#pragma comment(lib, "minhook/minhook.lib")

#include "SDK/SDK.hpp"
using namespace SDK;

#define LOG(Message) (std::cout << "[TITANIUM]: " << Message << std::endl)

namespace Helpers
{
	uintptr_t GetOffset(uintptr_t Offset)
	{
		return __int64(GetModuleHandleW(0)) + Offset;
	}

	UWorld* GetWorld()
	{
		return UObject::FindObject<UFortEngine>("FortEngine_")->GameViewport->World;
	}

	AFortGameStateAthena* GetGameState()
	{
		return (AFortGameStateAthena*)Helpers::GetWorld()->GameState;
	}

	AFortGameModeAthena* GetGameMode()
	{
		return (AFortGameModeAthena*)Helpers::GetWorld()->AuthorityGameMode;
	}

	void VirtualHook(void* VFT, int Index, void* Detour, void** OG = nullptr)
	{
		auto vft = *(void***)VFT;
		if (!vft || !vft[Index])
		{
			return;
		}

		if (OG)
			*OG = vft[Index];

		DWORD dwOld;
		VirtualProtect(&vft[Index], 8, PAGE_EXECUTE_READWRITE, &dwOld);

		vft[Index] = Detour;

		DWORD dwTemp;
		VirtualProtect(&vft[Index], 8, dwOld, &dwTemp);
	}

	template<typename T>
	T* GetDefObj()
	{
		return (T*)T::StaticClass()->DefaultObject;
	}
}

namespace Server
{
	static UNetDriver* (*CreateNetDriver)(UEngine*, UWorld*, FName) = decltype(CreateNetDriver)(Helpers::GetOffset(0x2C53530));
	static bool (*InitListen)(void*, void*, FURL&, bool, FString&) = decltype(InitListen)(Helpers::GetOffset(0x458C90));
	static void (*SetWorld)(UNetDriver*, UWorld*) = decltype(SetWorld)(Helpers::GetOffset(0x29CCCC0));
	static void (*ServerReplicateActors)(void*);

	void Listen()
	{
		Helpers::GetWorld()->NetDriver = CreateNetDriver(UObject::FindObject<UFortEngine>("FortEngine_"), Helpers::GetWorld(), FName(282));

		Helpers::GetWorld()->NetDriver->NetDriverName = FName(282);
		Helpers::GetWorld()->NetDriver->World = Helpers::GetWorld();

		FString Error;
		FURL URL = FURL();
		URL.Port = 7777;

		InitListen(Helpers::GetWorld()->NetDriver, Helpers::GetWorld(), URL, false, Error);
		SetWorld(Helpers::GetWorld()->NetDriver, Helpers::GetWorld());

		Helpers::GetWorld()->LevelCollections[0].NetDriver = Helpers::GetWorld()->NetDriver;
		Helpers::GetWorld()->LevelCollections[1].NetDriver = Helpers::GetWorld()->NetDriver;

		void** ReplicationDriverVTable = *(void***)Helpers::GetWorld()->NetDriver->ReplicationDriver;
		ServerReplicateActors = decltype(ServerReplicateActors)(ReplicationDriverVTable[0x56]);

		LOG("Listening On Port " << URL.Port);

		Globals::IsServerInitialized = true;

		if (Globals::IsServerInitialized)
		{
			Sleep(1000);

			LOG("Titanium Initialized!\n");

			Sleep(1000);

			LOG("Players Can Now Join!");
		}
	}
}

namespace Network
{
	void (*TickFlush)(UNetDriver*);
	void TickFlushHook(UNetDriver* Driver)
	{
		if (!Driver)
			return;

		if (Driver->ClientConnections.Num() > 0 && Driver->ReplicationDriver && !Driver->ClientConnections[0]->InternalAck)
			Server::ServerReplicateActors(Driver->ReplicationDriver);

		return TickFlush(Driver);

	}

	char KickPlayer(__int64, __int64, __int64)
	{
		return 1;
	}

	char ValidationFailure(__int64, __int64)
	{
		return 0;
	}

	__int64 __fastcall NoReservation(__int64 a1, __int64 a2, char a3, __int64 a4)
	{
		return 0;
	}
}