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

namespace Network
{

}