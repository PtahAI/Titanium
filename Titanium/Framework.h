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
}

namespace Server
{
	static void (*ServerReplicateActors)(void*);

	void Listen()
	{
		// WIP

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