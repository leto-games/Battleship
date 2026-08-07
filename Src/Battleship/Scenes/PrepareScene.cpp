#include "PrepareScene.hpp"
#include "../Data/GameData.hpp"

using namespace Battleship;

// ----------------------------------------------------------------------------------------------------
//  Подсцена с выбором стороны
// ----------------------------------------------------------------------------------------------------

#include "../Bitmaps/Bitmaps.hpp"
#include <GamesSupport/GameButtonHandlerMiddleware.hpp>

PlaceShipsScene::PlaceShipsScene(ISceneManager* game)
	: IScene{ game }, 
	ship_type_menu{ 8, {70, 6} },
	load_memory_menu{ 2, {30, 30} },
	button_helper{ {0, 0}, 1000 },
	cancel_multi_press_handler{ 2, 250 },
	cursor{ 10, 10, 7 }
{
	ship_type_menu.InitBaseCatchers();
	load_memory_menu.InitBaseCatchers();
	delete_loaded_memory.InitBaseCatchers();

	font = IFont::FromHandle(leto_api_v1->Font->GetFont(7, 7, 1));

	ship_type_menu.SetStyle(MenuStyle::STYLE_3, font);
	ship_type_menu.Capture();
	RefreshMenu();

	load_memory_menu.SetStyle(MenuStyle::STYLE_3, font);
	load_memory_menu.AppendMenuItem("ЗАГРУЗИТЬ");
	load_memory_menu.AppendMenuItem("УДАЛИТЬ");
	load_memory_menu.Capture();

	delete_loaded_memory.SetText(0, "УДАЛИТЬ");
	delete_loaded_memory.SetText(1, "СОХРАНЕНИЕ?");
	delete_loaded_memory.AppendMenuItem("УДАЛИТЬ", true);
	delete_loaded_memory.AppendMenuItem("ОТМЕНИТЬ", false);

	button_helper.SetButton(InputHelperUI::HelperButton::LEFT);
	button_helper.SetButton(InputHelperUI::HelperButton::RIGHT);
}

enum SHIP_MENU_EXTRA
{
	SHIP_MENU_START = -1, 
	SHIP_MENU_EDIT = -2, 
	SHIP_MENU_EXIT = -3,
	SHIP_MENU_RANDOM = -4,
	SHIP_MENU_CLEAR = -5
};

bool Battleship::PlaceShipsScene::AllShipsPlaced() const
{
	bool all = true;
	for (int i = 1; i < 5; ++i)
	{
		if (placed_ships[i - 1] < 5 - i)
			all = false;
	}
	return all;
}

void Battleship::PlaceShipsScene::StartGame()
{
	scene_manager->SwitchScene((uint8_t)Battleship_Scene::GAME);
	if (web_mode)
	{
		// Сообщение другому игроку о входе в игру
		// TODO:!!!
	}
}

void PlaceShipsScene::RefreshMenu()
{
	char text[15];
	ship_type_menu.Clear();
	ship_type_menu.ResetCurrentID();

	bool all_placed = true;
	bool placed = false;
	for (int i = 1; i < 5; ++i)
	{
		if (placed_ships[i - 1] < 5 - i)
		{
			leto_api_v1->Text->FormatText(text, sizeof(text), "%dx1:%d", i, 5 - i - placed_ships[i - 1]);
			ship_type_menu.AppendMenuItem(text, i);
			all_placed = false;
		}

		if (placed_ships[i - 1] > 0)
			placed = true;
	}
	if (all_placed)
		ship_type_menu.AppendMenuItem("НАЧАТЬ", SHIP_MENU_START);

	if (placed)
	{
		ship_type_menu.AppendMenuItem("ИЗМЕНИТЬ", SHIP_MENU_EDIT);
		ship_type_menu.AppendMenuItem("ОЧИСТИТЬ", SHIP_MENU_CLEAR);
	}
	else if (leto_api_v1->Globals->GetDebugMode())
	{
		ship_type_menu.AppendMenuItem("РАНДОМ", SHIP_MENU_RANDOM);
	}

	ship_type_menu.AppendMenuItem("ВЫЙТИ", SHIP_MENU_EXIT);
}

void PlaceShipsScene::Clear()
{
	ClearFields();

	// Очистка сохранения
	BattleshipSave save{};
	//GameSave.Set(save);
}

void Battleship::PlaceShipsScene::ClearFields()
{
	my_field->Clear();
	opp_field->Clear();
	memset(&placed_ships, 0, sizeof(placed_ships));
}

void Battleship::PlaceShipsScene::Save()
{
	// Сохранение доступно только для web-режима
	if (!web_mode) return;

	BattleshipSave save{};
	my_field->FillSave(save);
	save.crc16_opp_name = leto_api_v1->Globals->CalcCRC16(opp_name.ConstChar(), opp_name.TextLength());

	// TODO: SAVE!!!!
	//GameSave.Set(save);
}

void Battleship::PlaceShipsScene::LoadSave(BattleshipSave& save)
{
	// Загрузка сохранения доступна только для web-режима
	if (!web_mode) return;

	my_field->LoadSave(save);
	opp_field->LoadSave(save);
	for (Ship& ship : save.my_ships)
	{
		if (ship.GetSize() != 0)
		{
			placed_ships[ship.GetSize() - 1] += 1;
		}
	}
}

void Battleship::PlaceShipsScene::RandomPlaceShips()
{
	// TODO: прописать рандомную генерацию поля
	ClearFields();

	placed_ships[0] = 4;
	placed_ships[1] = 3;
	placed_ships[2] = 2;
	placed_ships[3] = 1;

	struct ShipParam {
		short head_row; 
		short head_col; 
		Ship::DIR dir; 
		short size;
	};

	ShipParam ships_param[] = {
		// 1
		{ 0, 9, Ship::DIR::DOWN, 1 },
		{ 1, 1, Ship::DIR::DOWN, 1 },
		{ 3, 5, Ship::DIR::DOWN, 1 },
		{ 8, 7, Ship::DIR::DOWN, 1 },
		// 2
		{ 4, 7, Ship::DIR::RIGHT, 2 },
		{ 6, 9, Ship::DIR::DOWN, 2 },
		{ 8, 1, Ship::DIR::RIGHT, 2 },
		// 3
		{ 1, 4, Ship::DIR::RIGHT, 3 },
		{ 6, 5, Ship::DIR::DOWN, 3 },
		// 4
		{ 3, 2, Ship::DIR::DOWN, 4 },
	};

	for (ShipParam& p : ships_param)
	{
		Ship ship{};
		ship.SetPos(p.head_row, p.head_col, p.dir, p.size);
		ship.InitAsSelf();
		my_field->PlaceShip(ship);
	}

	Save();
}

void PlaceShipsScene::OnShow()
{
	button_helper.ResetTimer();
	ship_blink.Start(600);
	cursor.SetActive(false);

	ClearFields();
	RefreshMenu();

	load_memory_menu.ResetCurrentID();
	ship_type_menu.ResetCurrentID();
	delete_loaded_memory.ResetCurrentID();
	state = PlaceState::CHOOSE_SHIP;

	BattleshipSave save{};
	//if (web_mode && GameSave.Get(save))
	//{
	//	bool ask_load_memory = false;
	//	if (save.my_ships[0].GetSize() != 0)
	//		ask_load_memory = true;
//
	//	if (ask_load_memory)
	//	{
	//		if (save.crc16_opp_name == calc_crc16(opp_name.ConstChar(), opp_name.TextLength()))
	//			state = PlaceState::ASK_LOAD_MEMORY;
	//		else
	//			Clear();
	//	}
	//}
}

bool PlaceShipsScene::ProcessInput(const AppEvent& event)
{
	GameButtonHandlerMiddleware::HandleEvent(event, IsSystemLeftEvent,		left_hold_handler);
	GameButtonHandlerMiddleware::HandleEvent(event, IsSystemRightEvent, 	right_hold_handler);
	GameButtonHandlerMiddleware::HandleEvent(event, IsSystemUpEvent,		up_hold_handler);
	GameButtonHandlerMiddleware::HandleEvent(event, IsSystemDownEvent,		down_hold_handler);

	GameButtonHandlerMiddleware::HandleEvent(event, IsSystemLeftEvent,		cancel_multi_press_handler, 0);
	GameButtonHandlerMiddleware::HandleEvent(event, IsSystemRightEvent, 	cancel_multi_press_handler, 1);
	
	if (state == PlaceState::CHOOSE_SHIP)
	{			
		if (ship_type_menu.MainProcessInput(event))
			return true;
		else if (IsSystemEnterEvent(event))
		{
			//SHIP_MENU_CLEAR
			int param = ship_type_menu.GetCurrentParam();
			if (param == SHIP_MENU_EDIT)
			{
				state = PlaceState::CHOOSE_EDIT_SHIP;
				cursor.SetActive(true);
				return true;
			}
			else if (param == SHIP_MENU_START)
			{
				StartGame();
				return true;
			}
			else if (param == SHIP_MENU_EXIT)
			{
				ClearFields();
				scene_manager->SwitchScene((uint8_t)Battleship_Scene::MAIN);
				return true;
			}
			else if (param == SHIP_MENU_CLEAR)
			{
				ClearFields();
				RefreshMenu();
				return true;
			}
			else if (param == SHIP_MENU_RANDOM)
			{
				RandomPlaceShips();
				RefreshMenu();
				return true;
			}
			current_ship.SetPos(0, 0, Ship::DIR::DOWN, param);
			state = PlaceState::PLACE_SHIP;
		}

		return true;
	}
	else if (state == PlaceState::ASK_LOAD_MEMORY)
	{
		if (delete_loaded_memory.MainProcessInput(event))
			return true;
		if (load_memory_menu.MainProcessInput(event))
			return true;
		else if (IsSystemEnterEvent(event))
		{
			bool _delete = load_memory_menu.GetCurrentID();
			if (!_delete)
			{
				BattleshipSave save{};
				//if (GameSave.Get(save))
				//{
				//	LoadSave(save);
				//	if (AllShipsPlaced())
				//	{
				//		StartGame();
				//		return;
				//	}
				//}
			}
			else
			{
				delete_loaded_memory.Enable();
			}
		}
		return true;
	}
	else if (state == PlaceState::CHOOSE_EDIT_SHIP)
	{
		if (IsSystemUpEvent(event))
			cursor.Up();
		else if (IsSystemDownEvent(event))
			cursor.Down();
		else if (IsSystemLeftEvent(event))
			cursor.Left();
		else if (IsSystemRightEvent(event))
			cursor.Right();
		else if (IsSystemEnterEvent(event))
		{
			int row, col;
			cursor.GetCell(row, col);
			if (my_field->GetShip(row, col, memory_ship))
			{
				memory_ship.DeInit();
				current_ship = memory_ship;
				my_field->RemoveShip(row, col);

				placed_ships[current_ship.GetSize() - 1] -= 1;

				cursor.SetActive(false);
				cursor.SetCell(0, 0);
				state = PlaceState::EDIT_SHIP;
			}
		}
		return true;
	}
	else
	{
		ship_blink.Start();
		if (IsSystemUpEvent(event))
			current_ship.Up();
		else if (IsSystemDownEvent(event))
			current_ship.Down();
		else if (IsSystemLeftEvent(event))
			current_ship.Left();
		else if (IsSystemRightEvent(event))
			current_ship.Right();
		else if (IsSystemEnterEvent(event))
		{
			// Обновить количество кораблей
			if (my_field->CanPlaceShip(current_ship))
			{
				Ship placed_ship = current_ship;
				placed_ship.InitAsSelf();
				my_field->PlaceShip(placed_ship);
				placed_ships[current_ship.GetSize() - 1] += 1;
				state = PlaceState::CHOOSE_SHIP;
				RefreshMenu();
				Save();
			}
		}
		return true;
	}

	if (IsSystemTurnLeftEvent(event) || IsSystemTurnRightEvent(event) && (state == PlaceState::PLACE_SHIP || state == PlaceState::EDIT_SHIP))
	{
		current_ship.Rotate();
		return true;
	}	

	return false;
}

#include <Data/StaticText.hpp>
#include <DrawFunctions/DrawText.hpp>

void PlaceShipsScene::Draw(IScreen& screen)
{
	using namespace DrawFunctions;

	StaticText32 text;
	my_field->Draw(screen, {1, 1});
	cursor.Draw(screen, {1, 1});

	if (state == PlaceState::PLACE_SHIP || state == PlaceState::EDIT_SHIP)
	{
		bool can_place = my_field->CanPlaceShip(current_ship);
		if (!can_place)
		{
			text = "БЛИЗКО!";
			DrawText(screen, { 96 - TextWidth(text, font) / 2, 26 }, text, WhiteColor, BlackColor, true, font);

			if (!ship_blink.Expired())
			{
				draw_ship = ship_blink.GetProgress() < 0.5f;
			}
			else
			{
				ship_blink.Start();
			}
		}
		else
			draw_ship = true;

		if (draw_ship)
		{
			current_ship.Draw(screen, {1, 1});
		}

		text = state == PlaceState::PLACE_SHIP ? "РАЗМЕСТИ" : "ПЕРЕМЕСТИ";
		DrawText(screen, {96 - TextWidth(text, font) / 2, 4}, text, WhiteColor, BlackColor, false, font);
		text = "КОРАБЛЬ";
		DrawText(screen, {96 - TextWidth(text, font) / 2, 12}, text, WhiteColor, BlackColor, false, font);

		text = "ОТМЕНА";
		DrawText(screen, {68, 40}, text, WhiteColor, BlackColor, false, font);

		button_helper.Draw(screen, {68 + 2 + TextWidth(text, font), 40 - 1});
	}
	else if (state == PlaceState::ASK_LOAD_MEMORY)
	{
		text = "ОБНАРУЖЕНО";
		DrawText(screen, {64 - TextWidth(text, font) / 2, 10}, text, WhiteColor, BlackColor, false, font);
		text = "СОХРАНЕНИЕ";
		DrawText(screen, {64 - TextWidth(text, font) / 2, 18}, text, WhiteColor, BlackColor, false, font);
		load_memory_menu.Draw(screen);
		delete_loaded_memory.Draw(screen, {64, 0});
		return;
	}
	else if (state == PlaceState::CHOOSE_EDIT_SHIP)
	{
		text = "ВЫБЕРИ";
		DrawText(screen, {96 - TextWidth(text, font) / 2, 4}, text, WhiteColor, BlackColor, false, font);
		text = "КОРАБЛЬ";
		DrawText(screen, {96 - TextWidth(text, font) / 2, 12}, text, WhiteColor, BlackColor, false, font);

		text = "ОТМЕНА";
		DrawText(screen, {68, 40}, text, WhiteColor, BlackColor, false, font);

		button_helper.Draw(screen, {68 + 2 + TextWidth(text, font), 40 - 1});
	}
	else
	{
		ship_type_menu.Draw(screen);
	}

}

void PlaceShipsScene::Loop()
{
	my_field->SetCursorActive(state == PlaceState::CHOOSE_EDIT_SHIP);

	cursor.SetActive(state == PlaceState::CHOOSE_EDIT_SHIP);
	if (!cursor.IsActive())
		cursor.SetCell(0, 0);
	
	bool _delete;
	if (delete_loaded_memory.IsResultReady(_delete))
	{
		if (_delete)
		{
			Clear();
			state = PlaceState::CHOOSE_SHIP;
			RefreshMenu();
		}
		delete_loaded_memory.Disable();
	}

	if (cancel_multi_press_handler.MultiPressed(false))
	{
		if (left_hold_handler.Holded(50, ButtonHoldHandler::OnHoldPolicy::KEEP) &&
			right_hold_handler.Holded(50, ButtonHoldHandler::OnHoldPolicy::KEEP))
		{
			if (state == PlaceState::PLACE_SHIP ||
				state == PlaceState::CHOOSE_EDIT_SHIP)
				state = PlaceState::CHOOSE_SHIP;
			else if (state == PlaceState::EDIT_SHIP)
			{
				Ship placed_ship = memory_ship;
				placed_ship.InitAsSelf();
				my_field->PlaceShip(placed_ship);
				placed_ships[current_ship.GetSize() - 1] += 1;
				state = PlaceState::CHOOSE_SHIP;

				RefreshMenu();
				Save();
			}

			left_hold_handler.Release();
			right_hold_handler.Release();
		}
	}
	else
	{
		if (state != PlaceState::PLACE_SHIP &&
			state != PlaceState::EDIT_SHIP &&
			state != PlaceState::CHOOSE_EDIT_SHIP)
			return;

		if (left_hold_handler.WaitHolded(300, 100))
		{
			if (state == PlaceState::CHOOSE_EDIT_SHIP)
				cursor.Left();
			else
				current_ship.Left();
		}
		else if (right_hold_handler.WaitHolded(300, 100))
		{
			if (state == PlaceState::CHOOSE_EDIT_SHIP)
				cursor.Right();
			else
				current_ship.Right();
		}

		if (up_hold_handler.WaitHolded(300, 100))
		{
			if (state == PlaceState::CHOOSE_EDIT_SHIP)
				cursor.Up();
			else
				current_ship.Up();
		}
		else if (down_hold_handler.WaitHolded(300, 100))
		{
			if (state == PlaceState::CHOOSE_EDIT_SHIP)
				cursor.Down();
			else
				current_ship.Down();
		}
	}

	int row, col;
	cursor.GetCell(row, col);
	my_field->SetCursorPos(row, col);
}

//void PlaceShipsScene::WebIn(const WebMessage& msg_in)
//{
//	if (!web_mode)
//		return;
//}
//
//void PlaceShipsScene::WebAckIn(const WebMessage& msg_in)
//{
//	if (!web_mode)
//		return;
//}
