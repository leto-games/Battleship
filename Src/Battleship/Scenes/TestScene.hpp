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
//#include <Graphics/BitmapEffects.hpp>

#include <Battleship/Bitmaps/Bitmaps.hpp>

namespace Battleship
{
	// Сцена с приветствием
	class TestScene final : public IScene
	{
    protected:
        LetoBitmap_V1 *BM_Popal_Big, *BM_Ubil_Big;
        LetoBitmap_V1 *BM_Popal_Outlined, *BM_Ubil_Outlined;

        LetoBitmap_V1 *BM_Popal_Big_rotated, *BM_Popal_Outlined_rotated;
        LetoBitmap_V1 *BM_Ubil_Big_rotated, *BM_Ubil_Outlined_rotated;
        
        bool stop{false};
        Timer timer;
        int16_t angle{};
	public:
		TestScene(ISceneManager* game) : IScene{ game }
        {
            timer.Start(2000);

        }

        void OnShow() override { 
            BM_Popal_Big = leto_api_v1->Bitmap->ResizeCopyBitmap(BitmapData::ToHandle(&BM_PopalText), LoadedGameAllocator(), 52, 52);
            BM_Ubil_Big = leto_api_v1->Bitmap->ResizeCopyBitmap(BitmapData::ToHandle(&BM_UbilText), LoadedGameAllocator(), 52, 52);
            
            BM_Popal_Outlined = leto_api_v1->Bitmap->MakeOutlinedBitmap(BM_Popal_Big, LoadedGameAllocator(), 2);
            BM_Ubil_Outlined = leto_api_v1->Bitmap->MakeOutlinedBitmap(BM_Ubil_Big, LoadedGameAllocator(), 2);

            BM_Popal_Big_rotated = leto_api_v1->Bitmap->CopyBitmap(BM_Popal_Big, LoadedGameAllocator());
            BM_Popal_Outlined_rotated = leto_api_v1->Bitmap->CopyBitmap(BM_Popal_Outlined, LoadedGameAllocator());

            BM_Ubil_Big_rotated = leto_api_v1->Bitmap->CopyBitmap(BM_Ubil_Big, LoadedGameAllocator());
            BM_Ubil_Outlined_rotated = leto_api_v1->Bitmap->CopyBitmap(BM_Ubil_Outlined, LoadedGameAllocator());
        }

		// Пользовательский ввод в игру
		virtual bool ProcessInput(const AppEvent& event) override 
        {
            if (IsSystemLeftEvent(event))
                scene_manager->SwitchScene((uint32_t)Battleship_Scene::MAIN);
            else if (IsSystemTurnLeftEvent(event))
            {
                angle -= 8;
                leto_api_v1->Bitmap->RotateBitmap(BM_Popal_Big,     BM_Popal_Big_rotated, angle);
                leto_api_v1->Bitmap->RotateBitmap(BM_Popal_Outlined, BM_Popal_Outlined_rotated, angle);
                leto_api_v1->Bitmap->RotateBitmap(BM_Ubil_Big,      BM_Ubil_Big_rotated, -angle);
                leto_api_v1->Bitmap->RotateBitmap(BM_Ubil_Outlined, BM_Ubil_Outlined_rotated, -angle);
            }
            else if (IsSystemTurnRightEvent(event))
            {
                angle += 8;
                leto_api_v1->Bitmap->RotateBitmap(BM_Popal_Big, BM_Popal_Big_rotated, angle);
                leto_api_v1->Bitmap->RotateBitmap(BM_Popal_Outlined, BM_Popal_Outlined_rotated, angle);
                leto_api_v1->Bitmap->RotateBitmap(BM_Ubil_Big,      BM_Ubil_Big_rotated, -angle);
                leto_api_v1->Bitmap->RotateBitmap(BM_Ubil_Outlined, BM_Ubil_Outlined_rotated, -angle);
            }
            else if (IsSystemAltEvent(event))
            {
                stop = !stop;
            }
            return true;
        }

		// Игровая отрисовка
		virtual void Draw(IScreen& screen) override
        {
            if (!stop)
            {
                if (timer.GetProgress() < 0.5f)
                    DrawFunctions::DrawRectangle(screen, {0, 0}, {screen.Width(), screen.Height()}, WhiteColor);
                else
                    DrawFunctions::DrawRectangle(screen, {0, 0}, {screen.Width(), screen.Height()}, BlackColor);
                if (timer.Expired()) timer.Start();
            }
            DrawFunctions::DrawBitmap(screen, {15, 20}, *BitmapData::FromHandle(BM_Popal_Outlined_rotated), BlackColor, WhiteColor);
            DrawFunctions::DrawBitmap(screen, {15, 20}, *BitmapData::FromHandle(BM_Popal_Big_rotated), WhiteColor, BlackColor);
            DrawFunctions::DrawBitmap(screen, {95, 20}, *BitmapData::FromHandle(BM_Ubil_Outlined_rotated), BlackColor, WhiteColor);
            DrawFunctions::DrawBitmap(screen, {95, 20}, *BitmapData::FromHandle(BM_Ubil_Big_rotated), WhiteColor, BlackColor);
        }

        SCENE_NO_ARGS_BUILDER(TestScene)
	};
}

#endif
