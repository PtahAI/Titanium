#pragma once

#include "Framework.h"

namespace Gamemode
{
	bool (*ReadyToStartMatch)(void*);
	bool ReadyToStartMatchHook(AFortGameModeAthena* Game)
	{
		if (!Globals::IsPlaylistInitialized)
		{
			Globals::IsPlaylistInitialized = true;

			auto PLAYLIST = UObject::FindObject<UFortPlaylistAthena>(Globals::Playlist);
			if (PLAYLIST)
			{
				Helpers::GetGameState()->CurrentPlaylistInfo.BasePlaylist = PLAYLIST;
				Helpers::GetGameState()->CurrentPlaylistInfo.OverridePlaylist = PLAYLIST;
				Helpers::GetGameState()->CurrentPlaylistInfo.PlaylistReplicationKey++;
				Helpers::GetGameState()->CurrentPlaylistInfo.MarkArrayDirty();
				Helpers::GetGameState()->OnRep_CurrentPlaylistInfo();

				Helpers::GetGameMode()->WarmupRequiredPlayerCount = 1;
			}
		}

		if (!Helpers::GetGameState()->MapInfo)
			return false;

		if (!Globals::IsServerListening)
		{
			Globals::IsServerListening = true;
			Helpers::GetGameState()->PlayersLeft--;
			Helpers::GetGameState()->OnRep_PlayersLeft();
			Helpers::GetGameState()->OnRep_CurrentPlaylistInfo();
			Server::Listen();
			Game->bWorldIsReady = true;
		}

		bool Ret = false;
		Ret = ReadyToStartMatch(Game);
		return Ret;
	}

	APawn* SpawnDefaultPawnForHook(AGameModeBase* Game, AController* NewPlayer, AActor* StartSpot)
	{
		auto Transform = StartSpot->GetTransform();

		return Game->SpawnDefaultPawnAtTransform(NewPlayer, Transform);
	}

	void InitHooks()
	{

	}
}