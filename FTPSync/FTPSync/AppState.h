#pragma once

#include "FTPSync.h"

enum class AppState : uint8_t
{
	Connecting,
	GatheringLocalFilelist,
	GatheringRemoteFilelist,
	Diff,
	NothingToDo,
	SyncPush,
	SyncPull
};