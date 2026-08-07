#include "MainScene.hpp"

using namespace Battleship;

// ----------------------------------------------------------------------------------------------------
//  Сцена с приветствием
// ----------------------------------------------------------------------------------------------------

#include "../Bitmaps/Bitmaps.hpp"
//#include "Game_TicTacToe/GameData.hpp"

// ----------------------------------------------------------------------------------------------------
//  Сцена с приветственным логотипом
// ----------------------------------------------------------------------------------------------------

#include <DrawFunctions/DrawBitmap.hpp>

WelcomeScene::WelcomeScene(ISceneManager* game) :
	IScene{ game }
{
	welcome_timer.Start(600);
}

void WelcomeScene::Draw(IScreen& screen)
{
	if (welcome_part >= 3)
	{
		scene_manager->SwitchScene((uint8_t) Battleship_Scene::MAIN);
		return;
	}

	//if (welcome_part == 0)
		DrawFunctions::DrawBitmap(screen, {0, 0}, BM_BattleshipLogo, WhiteColor);
	//if (welcome_part == 1)
	//	screen.Bitmap(0, 0, BM_TicTac);
	//if (welcome_part == 2)
	//	screen.Bitmap(0, 0, BM_TicTacToe);

	if (welcome_timer.Expired(false))
	{
		welcome_part++;
		welcome_timer.Start();
	}
}

// ----------------------------------------------------------------------------------------------------
//  Главная сцена с "Играть", "Настройки" и т.д.
// ----------------------------------------------------------------------------------------------------

MainScene::MainScene(ISceneManager* game) :
	IScene{ game }, menu{ 4, {35, 30} }, 
	ship_bitmap{ BM_Ship }, ship_animation{ ship_bitmap, true, 6 },
	bomb_animation{ BM_Bombs, 50 }
{
	menu.InitBaseCatchers();
	exit_question.InitBaseCatchers();

	menu.SetStyle(MenuStyle::STYLE_3, 	IFont::FromHandle(leto_api_v1->Font->GetFont(7, 7, 1)));
	menu.AppendMenuItem("ИГРАТЬ",		Battleship_Scene::CHOOSE_GM);
	menu.AppendMenuItem("НАСТРОЙКИ",	Battleship_Scene::SETTINGS);

	if (leto_api_v1->Globals->GetDebugMode())
		menu.AppendMenuItem("ТЕСТ",		Battleship_Scene::TEST);
	
	menu.AppendMenuItem("ВЫХОД",		Battleship_Scene::EXIT);
	menu.Capture();

	exit_question.SetText(0, "ВЫЙТИ");
	exit_question.SetText(1, " ИЗ ИГРЫ? ");
	exit_question.AppendMenuItem("ДА", true);
	exit_question.AppendMenuItem("НЕТ", false);
	exit_question.SetPosition({64, 0});
	exit_question.Disable();
	
	ship_animation.SetStartPos({-BM_Ship.width, 0});
	bomb_animation.SetResetToStart(true);

	bomb_positions.Push({ 0, 0 });
	bomb_positions.Push({ 0, 36 });
	bomb_positions.Push({ 100, 36 });
	bomb_positions.Push({ 100, 0 });
}

void MainScene::OnShow()
{
	menu.OnShow();
	exit_question.OnShow();

	reset_timer.Start(2000);

	ship_animation.SetFinishPos({180, 0});
	ship_animation.Play();

	bomb_animation.Play();
}

bool MainScene::ProcessInput(const AppEvent& event)
{
	if (exit_question.MainProcessInput(event))
		return true;
	else if (menu.MainProcessInput(event))
		return true;
	else if (IsSystemEnterEvent(event))
	{
		if (menu.GetCurrentParam() != Battleship_Scene::EXIT)
			scene_manager->SwitchScene((uint8_t) menu.GetCurrentParam());
		else 
			exit_question.Enable();
		return true;
	}
	return false;
}

#include "Battleship/BattleshipGame.hpp"
// TODO: REFACTOR!!! BAD PRACTICY

void MainScene::Loop()
{
	bool exit;
	if (exit_question.IsResultReady(exit))
	{
		if (exit && GameInstance) GameInstance->Close();
		exit_question.Disable();
	}
}

void MainScene::Draw(IScreen& screen)
{
	// Перезагрузка анимации
	if (!ship_animation.IsPlaying())
	{
		ship_animation.SetStartPos({-BM_Ship.width, 0});
		ship_animation.SetFinishPos({180, 0});
		ship_animation.Play();
	}

	if (reset_timer.Expired())
	{
		cur_pos = (cur_pos + 1) % bomb_positions.Count();
		bomb_animation.Play();
		reset_timer.Start();
	}

	ship_animation.Tick();
	bomb_animation.Tick();
	ship_bitmap.Draw(screen);

	const BitmapData* bomb = bomb_animation.GetCurrent();
	if (bomb)
	{
		Point2_i position = bomb_positions[cur_pos];
		DrawFunctions::DrawBitmap(screen, position, *bomb, WhiteColor);
	}

	menu.MainDraw(screen);
	exit_question.MainDraw(screen);
}

// ----------------------------------------------------------------------------------------------------
//  Подсцена с выбором режима игры.
// ----------------------------------------------------------------------------------------------------

#include "../Logic/Ship.hpp"
#include "../Data/GameData.hpp"

ChooseGameModeScene::ChooseGameModeScene(ISceneManager* game) :
	MainScene{ game }
{	
	menu.InitBaseCatchers();

	menu.Clear();
	menu.SetPosition({70, 20});

	menu.AppendMenuItem("ОДИН",			Battleship_Scene::PLACE_SHIPS);
	menu.AppendMenuItem("ПО СЕТИ",		Battleship_Scene::PLAY_WEB);
}

void ChooseGameModeScene::OnShow()
{
	Battleship::web_mode = false;
	reset_timer.Start(300);

	order = 0;
	gm_my_field.Clear();
	gm_opp_field.Clear();
	gm_opp_field.SetCursorActive(true);

	{
		Ship ship{};
		ship.SetPos(0, 0, Ship::DIR::DOWN, 4);
		ship.InitAsSelf();
		gm_my_field.PlaceShip(ship);
	}

	{
		Ship ship{};
		ship.SetPos(0, 2, Ship::DIR::RIGHT, 3);
		ship.InitAsSelf();
		gm_my_field.PlaceShip(ship);
	}

	{
		Ship ship{};
		ship.SetPos(5, 2, Ship::DIR::RIGHT, 2);
		ship.InitAsSelf();
		gm_my_field.PlaceShip(ship);
	}

	{
		Ship ship{};
		ship.SetPos(1, 6, Ship::DIR::RIGHT, 1);
		ship.InitAsSelf();
		gm_my_field.PlaceShip(ship);
	}

	{
		Ship ship{};
		ship.SetPos(6, 9, Ship::DIR::DOWN, 4);
		ship.InitAsSelf();
		gm_my_field.PlaceShip(ship);
	}

	menu.ResetCurrentID();
}

bool ChooseGameModeScene::ProcessInput(const AppEvent& event)
{
	if (IsSystemLeftEvent(event))
	{
		scene_manager->SwitchScene((uint8_t) Battleship_Scene::MAIN);
		return true;
	}
	return MainScene::ProcessInput(event);
}

void ChooseGameModeScene::Draw(IScreen& screen)
{
	menu.Draw(screen);

	if (reset_timer.Expired() && order >= 0 && order <= 100)
	{
		int row = order / 10, col = order % 10;
		BattleshipField::AttackResult res = gm_my_field.Attack(row, col);
		gm_opp_field.SetCursorPos(row, col);
		gm_opp_field.PutAttackResult(row, col, res);
		order++;
		reset_timer.Start();
	}
	 
	gm_opp_field.Draw(screen, {1, 1});
}
