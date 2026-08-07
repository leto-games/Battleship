#include "Make.hpp"

#include "BattleshipGame.hpp"

#include <LetoAPI_V1/Application/LetoApplication_V1.h>
#include <LetoAPI_V1/Application/LetoAppSetup_V1.h>

LetoResult_V1 Init(const LetoAPI_V1* api)
{
	// ==================================================
	//       Обязательная часть каждого приложения
	// ==================================================
	LetoResult_V1 setup = LetoAppSetup(api);
	if (setup != LETO_V1_SUCCESS) return setup;
	// ==================================================

	if (GameInstance) return LETO_V1_ALREADY_DONE;

	GameInstance = leto_new BattleshipGame(LETO_EXECUTABLE_INFO_VAR_NAME.Header);
	if (!GameInstance) return LETO_V1_OUT_OF_MEMORY;
	
	if (!GameInstance->Init()) return LETO_V1_ERROR;
	return LETO_V1_SUCCESS;
}

LetoResult_V1 Clean()
{
	if (!GameInstance) return LETO_V1_ALREADY_DONE;
	
	GameInstance = nullptr;

	return LETO_V1_SUCCESS;	
}

LetoAppStatus_V1 GetStatus()
{
	if (!GameInstance) return LETO_V1_NOT_WORK_STATUS;

	if (GameInstance->IsClosed())
		return LETO_V1_CLOSE_STATUS;
	
	return LETO_V1_WORK_STATUS;
}

LetoResult_V1 ProcessEvent(AppEvent event)
{
	if (!GameInstance) return LETO_V1_ERROR;
	//if (!game->ProcessEvent(event)) return LETO_V1_UNSUPPORT;

	GameInstance->ProcessEvent(event);
	return LETO_V1_SUCCESS;
}

LetoResult_V1 Draw(LetoScreen_V1* screen)
{
	if (!GameInstance) return LETO_V1_ERROR;
	GameInstance->Draw(*IScreen::FromHandle(screen));
	return LETO_V1_SUCCESS;
}

LetoResult_V1 Loop()
{
	if (!GameInstance) return LETO_V1_ERROR;
	GameInstance->Loop();
	return LETO_V1_SUCCESS;
}

// ======================================================================

LetoApplication_V1 LETO_EXECUTABLE_INFO_VAR_NAME
{
	{
		LETO_EXECUTABLE_TYPE_GAME,
		1,
		0x0001,
		"Battleship",
		"Морской бой"
	},
	&Init,
	&Clean,
	&GetStatus,
	&ProcessEvent,
	&Draw,
	&Loop
};

extern "C" LetoApplication_V1 MainMake()
{
	return LETO_EXECUTABLE_INFO_VAR_NAME;
}

// ======================================================================
