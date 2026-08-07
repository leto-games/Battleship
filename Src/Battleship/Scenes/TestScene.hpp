/*
 * TestScene.hpp
 *
 *  Created on: Mar 25, 2026
 *      Author: Timur
 */

#ifndef INC_GAME_BATTLESHIP_TEST_SCENE_HPP_
#define INC_GAME_BATTLESHIP_TEST_SCENE_HPP_

#include <SceneManager/ISceneManager.hpp>
#include <GamesSupport/BaseGame.hpp>

#include <UI/Menu/ParamMenu.hpp>
#include <UI/Menu/DialogMenu.hpp>
#include <Time/Timer.hpp>

#include "../Scenes/SceneID.hpp"
#include "../Logic/BattleshipField.hpp"
#include "../Bitmaps/BitmapsWithEffects.hpp"

#include <Data/StaticList.hpp>
#include <Drawable/DrawableBitmap.hpp>
#include <DrawFunctions/DrawBitmap.hpp>
#include <UI/Animation/MoveAnimation.hpp>
#include <UI/Animation/CartoonAnimation.hpp>
#include <Data/Point2.hpp>
#include <Graphics/BitmapEffects.hpp>

namespace Battleship
{
	// Сцена с приветствием
	class TestScene final : public IScene
	{
    protected:
        Timer timer;
        float angle{};
	public:
		TestScene(ISceneManager* game) : IScene{ game }
        {
            timer.Start(2000);
        }

		// Пользовательский ввод в игру
		virtual bool ProcessInput(const AppEvent& event) override 
        {
            if (IsSystemLeftEvent(event))
                scene_manager->SwitchScene((uint32_t)Battleship_Scene::MAIN);
            else if (IsSystemTurnLeftEvent(event))
            {
                angle -= 2.0f;
                BitmapEffects::RotateBitmap(BM_Popal_Sticker, BM_Popal_Sticker_Rotating, angle);
            }
            else if (IsSystemTurnRightEvent(event))
            {
                angle += 2.0f;
                BitmapEffects::RotateBitmap(BM_Popal_Sticker, BM_Popal_Sticker_Rotating, angle);
            }
            return true;
        }

		// Игровая отрисовка
		virtual void Draw(IScreen& screen) override
        {
            if (timer.GetProgress() < 0.5f)
                DrawFunctions::DrawRectangle(screen, {0, 0}, {screen.Width(), screen.Height()}, WhiteColor);
            else
                DrawFunctions::DrawRectangle(screen, {0, 0}, {screen.Width(), screen.Height()}, BlackColor);
            if (timer.Expired()) timer.Start();
            DrawFunctions::DrawBitmap(screen, {10, 10}, BM_Popal_Sticker_Rotating, WhiteColor, BlackColor);
            DrawFunctions::DrawBitmap(screen, {60, 10}, BM_Ubil_Sticker, WhiteColor, BlackColor);
        }

        SCENE_NO_ARGS_BUILDER(TestScene)
	};
}

#endif
