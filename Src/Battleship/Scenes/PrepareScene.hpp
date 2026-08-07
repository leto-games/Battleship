/*
 * PrepareScene.hpp
 *
 *  Created on: Dec 27, 2025
 *      Author: Timur
 */

#ifndef INC_GAME_BATTLESHIP_PREPARE_SCENE_HPP_
#define INC_GAME_BATTLESHIP_PREPARE_SCENE_HPP_

#include <SceneManager/ISceneManager.hpp>
#include <GamesSupport/BaseGame.hpp>

#include <UI/Menu/ParamMenu.hpp>
#include <UI/Menu/DialogMenu.hpp> 
#include <Time/Timer.hpp>

#include "../Scenes/SceneID.hpp"

#include <Drawable/Circle.hpp>
#include <UI/Animation/Animation.hpp>

#include "../Logic/Ship.hpp"
#include "../Logic/BattleshipCursor.hpp"
#include "../Logic/BattleshipField.hpp"

#include <UI/ButtonHoldHandler.hpp>
#include <UI/ButtonMultiPressHandler.hpp>
#include <UI/InputHelperUI.hpp>

#include <Graphics/IFont.hpp>


namespace Battleship
{
	// Подсцена с установкой кораблей
	class PlaceShipsScene final : public IScene
	{
	protected:
		BattleshipCursor cursor;
		const IFont* font;

		// Состояние установки лодки
		enum class PlaceState 
		{ 
			CHOOSE_SHIP, 
			PLACE_SHIP,
			CHOOSE_EDIT_SHIP,	// Выбрать лодку для редактирования
			EDIT_SHIP,
			ASK_LOAD_MEMORY,
		};
		PlaceState state = PlaceState::CHOOSE_SHIP;

		enum class ShipType
		{
			_1 = 4,
			_2 = 3,
			_3 = 2,
			_4 = 1
		};

		// Расположено кораблей на поле
		int placed_ships[4]{};

		// Признак что все корабли расположены
		bool AllShipsPlaced() const;

		DialogParamMenu<bool, 4> delete_loaded_memory;
		ParamMenu<int, 16> ship_type_menu;
		Menu<4> load_memory_menu;

		// Текущая устанавливаемая лодка
		Ship current_ship{};

		// Редактируемая лодка в памяти
		Ship memory_ship{};

		// Таймер моргания лодки (когда её нельзя установить)
		Timer ship_blink{};
		bool draw_ship = true;

		InputHelperUI button_helper;

		ButtonHoldHandler up_hold_handler, down_hold_handler;
		ButtonHoldHandler left_hold_handler, right_hold_handler;
		ButtonMultiPressHandler cancel_multi_press_handler;

		void StartGame();

		void RefreshMenu();
		void Clear();
		void ClearFields();
		void Save();
		void LoadSave(BattleshipSave& save);

		void RandomPlaceShips();

	public:
		PlaceShipsScene(ISceneManager* game);

		virtual void OnShow() override;

		// Пользовательский ввод в игру
		virtual bool ProcessInput(const AppEvent& event) override;

		// Игровая отрисовка
		virtual void Draw(IScreen& screen) override;

		virtual void Loop() override;

		SCENE_NO_ARGS_BUILDER(PlaceShipsScene)
	};

}
#endif
