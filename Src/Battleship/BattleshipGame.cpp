#include "BattleshipGame.hpp"

// ----------------------------------------------------------------------------------------------------

BattleshipGame::BattleshipGame(const AppBinHeader& header) : BaseGame{ header } 
{
}

#include "Scenes/MainScene.hpp"
#include "Scenes/PrepareScene.hpp"
#include "Scenes/GameScene.hpp"
#include "Scenes/SceneID.hpp"
#include "Scenes/TestScene.hpp"

#include "Data/GameData.hpp"
#include "Bitmaps/Bitmaps.hpp"
#include "Bitmaps/BitmapsWithEffects.hpp"

#include <DrawFunctions/DrawText.hpp>

#include <GamesSupport/Lobby/LobbyScene.hpp>

using namespace Battleship;

void battleship_callback(uint32_t id, const void* data, uint32_t size)
{
	data_processor.ReceiveData(id, data, size);
}

void battleship_ready()
{
	LobbyConnection_V1 lobby;
	if (!leto_api_v1->Lobby->GetActiveLobby(&lobby))
		return;
		
	bool host = lobby.owner == leto::globals::GetDeviceID();

	Battleship::web_mode = true;
	Battleship::my_move = host;
	
	uint8_t opp_member = host ? 1 : 0;
	
	WebDeviceInfo_V1 info;
	if (!leto_api_v1->Web->FindDeviceNear(lobby.members[opp_member], &info))
		return;

	Battleship::opp_name = info.web_name;
	Battleship::opp_channel = info.channel;
	Battleship::opp_avatar = info.avatar_id;
	Battleship::opp_id = info.id;

	data_processor.Enable(true);
	GameInstance->SwitchScene(Battleship_Scene::PLACE_SHIPS);
}

bool BattleshipGame::CustomInit()
{
	DrawFunctions::SetDefaultFont(IFont::FromHandle(leto_api_v1->Font->GetFont(8, 8, 0)));

	//InitStickers(LoadedGameAllocator());
	
	my_field = leto_new BattleshipMyField;
	opp_field = leto_new BattleshipOppField;

	AddSceneBuilder<WelcomeScene>(Battleship_Scene::WELCOME);
	AddSceneBuilder<MainScene>(Battleship_Scene::MAIN);
	AddSceneBuilder<ChooseGameModeScene>(Battleship_Scene::CHOOSE_GM);
	AddSceneBuilder<PlaceShipsScene>(Battleship_Scene::PLACE_SHIPS);
	AddSceneBuilder<GameScene>(Battleship_Scene::GAME);
	//AddSceneBuilder<TestScene>(Battleship_Scene::TEST);

	AddSceneBuilder<LobbyScene>(Battleship_Scene::PLAY_WEB, 2, &battleship_callback, &battleship_callback, &battleship_ready);

	data_processor.RegLobbyData(&Battleship::LobbyData_Common);

	SwitchScene((uint8_t) Battleship_Scene::WELCOME);
	return true;
}

void BattleshipGame::CustomClose()
{
	my_field = nullptr;
	opp_field = nullptr;
}

void BattleshipGame::CustomLoop()
{
	data_processor.Loop();
}

void BattleshipGame::Draw(IScreen &screen)
{
	BaseGame::Draw(screen);

	DrawFunctions::DrawText(screen, {0,0}, to_text(Battleship::GameStage.GetOrDefault()));
	DrawFunctions::DrawText(screen, {0,10}, to_text(Battleship::GameStage.GetOrDefault(Battleship::opp_id)));
}

BattleshipGame* GameInstance = nullptr;
