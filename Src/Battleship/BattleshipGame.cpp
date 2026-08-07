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

//#include <GamesSupport/PrepareWebGameScene.hpp>
#include "Data/GameData.hpp"
#include "Bitmaps/Bitmaps.hpp"
#include "Bitmaps/BitmapsWithEffects.hpp"

#include <DrawFunctions/DrawText.hpp>

#include <GamesSupport/Lobby/LobbyScene.hpp>

using namespace Battleship;

void battleship_callback(uint32_t id, const void* data, uint32_t size)
{

}

bool BattleshipGame::CustomInit()
{
	DrawFunctions::SetDefaultFont(IFont::FromHandle(leto_api_v1->Font->GetFont(8, 8, 0)));

	//InitStickers(GetCommonAllocator());
	
	static BattleshipMyField my;
	static BattleshipOppField opp;
	my_field = &my;
	opp_field = &opp;

	AddSceneBuilder<WelcomeScene>(Battleship_Scene::WELCOME);
	AddSceneBuilder<MainScene>(Battleship_Scene::MAIN);
	AddSceneBuilder<ChooseGameModeScene>(Battleship_Scene::CHOOSE_GM);
	AddSceneBuilder<PlaceShipsScene>(Battleship_Scene::PLACE_SHIPS);
	AddSceneBuilder<GameScene>(Battleship_Scene::GAME);
	AddSceneBuilder<TestScene>(Battleship_Scene::TEST);

	AddSceneBuilder<LobbyScene>(Battleship_Scene::PLAY_WEB, 2, &battleship_callback, &battleship_callback);

	//PrepareWebGameScene* prepare_web = AddGameScene<PrepareWebGameScene>(
	//	Battleship_Scene::PLAY_WEB,	this, 
	//	&Battleship::opp_name, &Battleship::opp_avatar, &Battleship::opp_channel, (uint8_t)Battleship_Scene::PLACE_SHIPS
	//);
	//prepare_web->SetParams(&Battleship::web_mode, &Battleship::my_move);

	//CurrentSpace = leto_api_v1->User->GetUserSpace();
	//if (!CurrentSpace)
	//{
	//	/// TODO: Сделать обработку, если не получено пространство
	//}

	SwitchScene((uint8_t) Battleship_Scene::WELCOME);
	return true;
}

void BattleshipGame::CustomClose()
{
	my_field = nullptr;
	opp_field = nullptr;
}

BattleshipGame* GameInstance = nullptr;

//void BattleshipGame::CommonWebIn(const WebMessage& msg_in)
//{
//	// Запрос начала игры
//	if (start_request.ProcessRequest(msg_in))
//	{
//		SwitchScene((uint8_t) Battleship_Scene::PLAY_WEB);
//	}
//}
