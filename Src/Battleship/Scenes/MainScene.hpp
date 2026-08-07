/*
 * MainScene.hpp
 *
 *  Created on: Dec 13, 2025
 *      Author: Timur
 */

#ifndef INC_GAME_BATTLESHIP_MAIN_SCENE_HPP_
#define INC_GAME_BATTLESHIP_MAIN_SCENE_HPP_

#include <SceneManager/ISceneManager.hpp>
#include <GamesSupport/BaseGame.hpp>

#include <UI/Menu/ParamMenu.hpp>
#include <UI/Menu/DialogMenu.hpp>
#include <Time/Timer.hpp>

#include "../Scenes/SceneID.hpp"
#include "../Logic/BattleshipField.hpp"

#include <Data/StaticList.hpp>
#include <Drawable/DrawableBitmap.hpp>
#include <UI/Animation/MoveAnimation.hpp>
#include <UI/Animation/CartoonAnimation.hpp>
#include <Data/Point2.hpp>

namespace Battleship
{
	// Сцена с приветствием
	class WelcomeScene final : public IScene
	{
	protected:
		Timer welcome_timer;
		uint8_t welcome_part{};

	public:
		WelcomeScene(ISceneManager* game);

		// Пользовательский ввод в игру
		virtual bool ProcessInput(const AppEvent& event) override { return true; }

		// Игровая отрисовка
		virtual void Draw(IScreen& screen) override;

		SCENE_NO_ARGS_BUILDER(WelcomeScene)
	};

	// Главная сцена с "Играть", "Настройки" и т.д.
	class MainScene : public IScene
	{
	protected:
		Timer reset_timer{};

		Drawable::DrawableBitmap ship_bitmap;
		MoveAnimation ship_animation;

		CartoonAnimation bomb_animation;
		int cur_pos = 0;
		StaticList<Point2_i, 4> bomb_positions;

		ParamMenu<Battleship_Scene, 16> menu;
		DialogParamMenu<bool, 4> exit_question;

	public:
		MainScene(ISceneManager* game);

		virtual void OnShow() override;

		// Пользовательский ввод в игру
		virtual bool ProcessInput(const AppEvent& event) override;

		// Игровая отрисовка
		virtual void Draw(IScreen& screen) override;

		void Loop() override;

		SCENE_NO_ARGS_BUILDER(MainScene)
	};

	// Подсцена с выбором режима игры
	class ChooseGameModeScene final : public MainScene
	{
	protected:
		BattleshipMyField 	gm_my_field;
		BattleshipOppField 	gm_opp_field;
		int order{};

	public:
		ChooseGameModeScene(ISceneManager* game);
		
		virtual void OnShow() override;

		// Пользовательский ввод в игру
		virtual bool ProcessInput(const AppEvent& event) override;

		// Игровая отрисовка
		virtual void Draw(IScreen& screen) override;

		SCENE_NO_ARGS_BUILDER(ChooseGameModeScene)
	};

}

#endif
