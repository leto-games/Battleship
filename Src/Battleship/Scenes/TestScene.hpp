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
        uint16_t angle{};

        static uint16_t AngleNormalize(int16_t angle)
        {
            while (angle < 0) angle += 360;
            return (angle %= 360);
        }
	public:
		TestScene(ISceneManager* game) : IScene{ game }
        {
            timer.Start(2000);
        }

        void OnShow() override { 
            BM_Popal_Big = leto_api_v1->Bitmap->ResizeCopyBitmap(BitmapData::ToHandle(&BM_PopalText), LoadedGameAllocator(), 52, 52);
            BM_Ubil_Big = leto_api_v1->Bitmap->ResizeCopyBitmap(BitmapData::ToHandle(&BM_UbilText), LoadedGameAllocator(), 52, 52);
            
            //BM_Popal_Outlined = leto_api_v1->Bitmap->MakeOutlinedBitmap(BM_Popal_Big, LoadedGameAllocator(), 2);
            //BM_Ubil_Outlined = leto_api_v1->Bitmap->MakeOutlinedBitmap(BM_Ubil_Big, LoadedGameAllocator(), 2);

            BM_Popal_Outlined = leto_api_v1->Bitmap->MakeStickerBitmap(BM_Popal_Big, LoadedGameAllocator(), 2);
            BM_Ubil_Outlined = leto_api_v1->Bitmap->MakeStickerBitmap(BM_Ubil_Big, LoadedGameAllocator(), 2);

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
                angle = AngleNormalize(angle - 8);
                leto_api_v1->Bitmap->RotateBitmap(BM_Popal_Big,     BM_Popal_Big_rotated, angle);
                leto_api_v1->Bitmap->RotateBitmap(BM_Popal_Outlined, BM_Popal_Outlined_rotated, angle);
                leto_api_v1->Bitmap->RotateBitmap(BM_Ubil_Big,      BM_Ubil_Big_rotated, -angle);
                leto_api_v1->Bitmap->RotateBitmap(BM_Ubil_Outlined, BM_Ubil_Outlined_rotated, -angle);
            }
            else if (IsSystemTurnRightEvent(event))
            {
                angle = AngleNormalize(angle + 8);
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
            RGBColor back = BlackColor, front = WhiteColor;
            if (!stop)
            {
                if (timer.GetProgress() < 0.5f)
                {
                    back = WhiteColor;
                    front = BlackColor;
                    screen.FillScreen(back);
                }
            }
            if (timer.Expired()) timer.Start();
            
            int32_t triangle_add = 35;
            if (timer.GetProgress() < 0.25f)
                triangle_add *= (0.25f - 2 * timer.GetProgress());
            else if (timer.GetProgress() < 0.5f)
                triangle_add *= (-0.25f);
            else
                triangle_add *= -(0.75f - timer.GetProgress());

            leto_api_v1->Graphics->DrawTriangle(IScreen::ToHandle(&screen), 
                10 - triangle_add, 8 - triangle_add, 
                140 + triangle_add, 31 - triangle_add, 
                75, 142 + triangle_add, 
                0, YellowColor);
            leto_api_v1->Graphics->DrawTriangle(IScreen::ToHandle(&screen), 
                10, 64, 
                140, 10, 
                45, 117, 
                2, PurpleColor);

            leto_api_v1->Graphics->DrawLine(IScreen::ToHandle(&screen), 
                30 + 30 * leto_api_v1->Math->cosf(angle * 3.141592f / 180), 0, 
                130 + 30 * leto_api_v1->Math->sinf(angle * 3.141592f / 180), 128, 
                2, DeepOrangeColor);
            leto_api_v1->Graphics->DrawLine(
                IScreen::ToHandle(&screen), 
                160, 0,
                0, 98 + 30 * leto_api_v1->Math->sinf(angle * 3.141592f / 180), 
                2, DeepOrangeColor);

            leto_api_v1->Graphics->DrawRoundRect(IScreen::ToHandle(&screen), 5, 5, 150, 118, 10, 1, front);

            for (int i = 1; i < 4; ++i)
                leto_api_v1->Graphics->DrawRect(IScreen::ToHandle(&screen), 10 + i * 10, 10 + i * 10, 140 - i * 20, 108 - i * 20, 1, CyanColor);

            int32_t w = 80;
            if (timer.GetProgress() < 0.5f)
                w += 30 * (0.25f - timer.GetProgress());
            else
                w -= 30 * (0.75f - timer.GetProgress());
            leto_api_v1->Graphics->DrawEllipse(IScreen::ToHandle(&screen), 80, 64, 10, w-35, 0, BlackColor);
            leto_api_v1->Graphics->DrawEllipse(IScreen::ToHandle(&screen), 80, 64, 10, w-35, 1, BlueColor);
            leto_api_v1->Graphics->DrawEllipse(IScreen::ToHandle(&screen), 80, 64, 20, w-25, 1, BlueColor);
            leto_api_v1->Graphics->DrawEllipse(IScreen::ToHandle(&screen), 80, 64, 30, w-15, 1, IndigoColor);
            leto_api_v1->Graphics->DrawEllipse(IScreen::ToHandle(&screen), 80, 64, 40, w-5, 1, IndigoColor);
            leto_api_v1->Graphics->DrawEllipse(IScreen::ToHandle(&screen), 80, 64, 20 + w, 20 + w, 3, DarkRedColor);
            leto_api_v1->Graphics->DrawEllipse(IScreen::ToHandle(&screen), 80, 64, 10 + w, 10 + w, 1, DeepOrangeColor);
            leto_api_v1->Graphics->DrawEllipse(IScreen::ToHandle(&screen), 80, 64, 0 + w, 0 + w, 1, GreenColor);

            leto_api_v1->Graphics->DrawBitmap(IScreen::ToHandle(&screen), 15, 35, BM_Popal_Outlined_rotated, BlackColor);
            leto_api_v1->Graphics->DrawBitmap(IScreen::ToHandle(&screen), 15, 35, BM_Popal_Big_rotated,      WhiteColor);
            leto_api_v1->Graphics->DrawBitmap(IScreen::ToHandle(&screen), 95, 35, BM_Ubil_Outlined_rotated,  BlackColor);
            leto_api_v1->Graphics->DrawBitmap(IScreen::ToHandle(&screen), 95, 35, BM_Ubil_Big_rotated,       WhiteColor);

            //DrawFunctions::DrawBitmap(screen, {15, 20}, *BitmapData::FromHandle(BM_Popal_Outlined_rotated), BlackColor, WhiteColor);
            //DrawFunctions::DrawBitmap(screen, {15, 20}, *BitmapData::FromHandle(BM_Popal_Big_rotated), WhiteColor, BlackColor);
            //DrawFunctions::DrawBitmap(screen, {95, 20}, *BitmapData::FromHandle(BM_Ubil_Outlined_rotated), BlackColor, WhiteColor);
            //DrawFunctions::DrawBitmap(screen, {95, 20}, *BitmapData::FromHandle(BM_Ubil_Big_rotated), WhiteColor, BlackColor);
        }

        SCENE_NO_ARGS_BUILDER(TestScene)
	};
}

#endif
