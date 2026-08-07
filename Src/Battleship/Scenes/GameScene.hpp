/*
 * GameScene.hpp
 *
 *  Created on: Dec 13, 2025
 *      Author: Timur
 */

#ifndef INC_GAME_TIC_TAC_TOE_GAME_SCENE_HPP_
#define INC_GAME_TIC_TAC_TOE_GAME_SCENE_HPP_

#include <GamesSupport/BaseGame.hpp>
#include <SceneManager/IScene.hpp>

#include "../Logic/BattleshipCursor.hpp"
#include "../Data/GameData.hpp"
#include <Time/Timer.hpp>

#include <UI/Animation/Animation.hpp>
#include <UI/Menu/ParamMenu.hpp>

#include <UI/ButtonHoldHandler.hpp>
#include <UI/ButtonMultiPressHandler.hpp>

namespace Battleship
{
	// Игровая сцена игры TicTacToe
	class GameScene final : public IScene
	{
	protected:
		BattleshipCursor cursor;

		// Пауза игры
		bool pause{};

		// Для режима Web 
		bool online{};

		// Таймер проверки, что соединение установлено
		Timer check_ready_timer;

		// Режим готовности
		bool ready{};

		// 1 - я, 2 - другой
		int winner{};

		// Флаг, что выход из сцены, связан с реваншем
		bool switch_revanche = false;

		// Меню обработки запроса на реванш
		ParamMenu<bool, 4> revanche_answer;

		void Restart();

		void RefreshPauseMenu();

		// Сохранить игру
		void Save();
		// Очистить сохранение
		void ClearSave();

		enum PAUSE_STATE : unsigned char { CONTINUE, REVANCHE, EXIT };
		ParamMenu<PAUSE_STATE, 4> pause_menu;

		ButtonHoldHandler up_hold_handler, down_hold_handler;
		ButtonHoldHandler left_hold_handler, right_hold_handler;
		ButtonMultiPressHandler pause_multi_press_handler;

		Timer show_res_timer;
		short show_res_part = 0;
		BattleshipField::AttackResult show_res{};
		void ShowResult(BattleshipField::AttackResult res);

	public:
		GameScene(ISceneManager* game);

		virtual void OnShow() override;

		virtual void OnHide() override;

		// Пользовательский ввод в игру
		virtual bool ProcessInput(const AppEvent& event) override;

		// Игровая отрисовка
		virtual void Draw(IScreen& screen) override;

		// Фоновая обработка данных
		virtual void Loop() override;

		SCENE_NO_ARGS_BUILDER(GameScene)
	};
}

#endif
