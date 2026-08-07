#include "GameScene.hpp"

#include "SceneID.hpp"

#include "../Data/GameData.hpp"

using namespace Battleship;

GameScene::GameScene(ISceneManager* game)
	: IScene{ game },
	cursor{10, 10, 7},
	pause_multi_press_handler{ 2, 250 },
	pause_menu{ 2, {64, 25} },
	revanche_answer{ 2, {64, 25} }
	//move{ BattleshipField::PLAYER::PLAYER_X },
	//cursor_animation{ 300 }
{
	pause_menu.InitBaseCatchers();
	revanche_answer.InitBaseCatchers();

	pause_menu.SetStyle(MenuStyle::STYLE_3, IFont::FromHandle(leto_api_v1->Font->GetFont(7, 7, 1)));
	pause_menu.SetHorizonAlignment(MenuHorizonAlignment::CENTER);
	pause_menu.Capture();

	RefreshPauseMenu();

	revanche_answer.SetStyle(MenuStyle::STYLE_3, IFont::FromHandle(leto_api_v1->Font->GetFont(7, 7, 1)));
	revanche_answer.SetHorizonAlignment(MenuHorizonAlignment::CENTER);
	revanche_answer.Capture();

	revanche_answer.AppendMenuItem("ДА", true);
	revanche_answer.AppendMenuItem("НЕТ", false);

	/*

	cursor_animation{ 300 }, cursor{ 2 }
{
	cursor_animation.SetResetToStart(true);
	cursor_animation.Init(&cursor,
		[](Drawable::Circle& cursor, float progress)
		{
			cursor.SetRadius(2 + progress * 5);
		}
	);

	*/

}

void GameScene::OnShow()
{
	Restart();
}

void GameScene::OnHide()
{
	// Отправить сообщение о разрыве связи
	if (!switch_revanche)
	{
		web_mode = false;

		//WebMessage msg_out = MakeGameMessage();
		//end_request.MakeRequest(msg_out);
		/// TODO:
		//WebAckOut(msg_out);
	}
	else
	{
		my_field->Clear();
		opp_field->Clear();
	}
}

//#include "ProjectSettings/ProjectSettings.hpp"

void GameScene::Restart()
{
	cursor.SetCell(0, 0);
	// Если не подгрузил инфу из памяти, очищаем все
	if (my_field->ShipsCount() == 0)
		opp_field->Clear();
	winner = 0;
	pause = false;
	pause_menu.ResetCurrentID();

	fire_request.Cancel();
	revanche_request.Cancel();
	revanche_answer.ResetCurrentID();

	switch_revanche = false;
	online = true;
	ready = false;

	if (web_mode)
	{
		check_ready_timer.Start(300);

		//WebMessage msg_out = MakeGameMessage();
//
		//ready_request.MakeRequest(msg_out);

		/// TODO:
		//WebAckOut(msg_out);
	}

	RefreshPauseMenu();
}

//WebMessage Battleship::GameScene::MakeGameMessage() const
//{
//	WebMessage msg_out;
//
//	// Канал для подключения
//	msg_out.channel = opp_channel;
//
//	// Фаза игры
//	msg_out.data[4] = 0x03;
//
//	return msg_out;
//}

void GameScene::RefreshPauseMenu()
{
	pause_menu.Clear();
	pause_menu.ResetCurrentID();

	if (winner != 0)
		pause_menu.AppendMenuItem("РЕВАНШ", PAUSE_STATE::REVANCHE);
	else
		pause_menu.AppendMenuItem("ПРОДОЛЖИТЬ", PAUSE_STATE::CONTINUE);

	pause_menu.AppendMenuItem("ВЫЙТИ", PAUSE_STATE::EXIT);
}

void GameScene::Save()
{
	// Сохранение доступно только для web-режима
	if (!web_mode) return;

	BattleshipSave save{};
	my_field->FillSave(save);
	opp_field->FillSave(save);
	save.crc16_opp_name = leto_api_v1->Globals->CalcCRC16(opp_name.ConstChar(), opp_name.TextLength());

	//GameSave.Set(save);
}

void GameScene::ClearSave()
{
	// Очистка сохранения доступна только для web-режима
	if (!web_mode) return;

	BattleshipSave save{};
	//GameSave.Set(save);
}

void Battleship::GameScene::ShowResult(BattleshipField::AttackResult res)
{
	show_res_timer.Start(1500);
	show_res = res;
}

#include <GamesSupport/GameButtonHandlerMiddleware.hpp>
#include <Input/SystemInputID.hpp>

bool GameScene::ProcessInput(const AppEvent& event)
{
	//if (cursor_animation.IsPlaying())
//	return;
	if (revanche_request.IsProcessingRequest())
	{
		if (revanche_answer.ProcessInput(event))
			return true;
		else if (IsSystemEnterEvent(event))
		{
			bool answer = revanche_answer.GetCurrentParam();

			//WebMessage msg_out = MakeGameMessage();
//
			//// Сообщение ДА или НЕТ 
			//// (на самом деле сообщение НЕТ отработано не будет, поскольку
			//// при смене сцены будет вызван OnHide в котором будет отправлено синхро-сообщение
			//// с гарантией доставки - а оно может обрабатываться только одно - 
			//// нужно будет поправить в будущем)
			//// TODO: добавить обработку нескольких гарантируемых сообщений
			//revanche_request.MakeResponse(msg_out, answer ? YES_RESPONSE : NO_RESPONSE);

			/// TODO:
			//WebAckOut(msg_out);

			// ДА
			if (answer)
			{
				switch_revanche = true;
				scene_manager->SwitchScene((uint32_t)Battleship_Scene::PLACE_SHIPS);
			}
			// НЕТ
			else
			{
				scene_manager->SwitchScene((uint8_t)Battleship_Scene::MAIN);
			}
		}
		return true;
	}

	if (pause)
	{
		if (pause_menu.ProcessInput(event))
			return true;
		else if (IsSystemEnterEvent(event))
		{
			pause = false;
			PAUSE_STATE state = pause_menu.GetCurrentParam();
			if (state == PAUSE_STATE::EXIT)
			{
				scene_manager->SwitchScene((uint8_t)Battleship_Scene::MAIN);
			}
			else if (state == PAUSE_STATE::REVANCHE)
			{
				//WebMessage msg_out = MakeGameMessage();
//
				//revanche_request.MakeRequest(msg_out);

				/// TODO:
				//WebAckOut(msg_out);
			}
		}
		return true;
	}

	GameButtonHandlerMiddleware::HandleEvent(event, IsSystemLeftEvent, 		pause_multi_press_handler, 0);
	GameButtonHandlerMiddleware::HandleEvent(event, IsSystemRightEvent, 	pause_multi_press_handler, 1);

	GameButtonHandlerMiddleware::HandleEvent(event, IsSystemLeftEvent,		left_hold_handler);
	GameButtonHandlerMiddleware::HandleEvent(event, IsSystemRightEvent, 	right_hold_handler);
	GameButtonHandlerMiddleware::HandleEvent(event, IsSystemUpEvent,		up_hold_handler);
	GameButtonHandlerMiddleware::HandleEvent(event, IsSystemDownEvent,		down_hold_handler);

	if (web_mode && (!online || !ready))
		return true;

	if (!cursor.IsActive())
		return true;

	// Добавить обработку в cursor
	if (IsSystemUpEvent(event))
		cursor.Up();
	else if (IsSystemDownEvent(event))
		cursor.Down();
	else if (IsSystemLeftEvent(event))
		cursor.Left();
	else if (IsSystemRightEvent(event))
		cursor.Right();
	else if (IsSystemTurnLeftEvent(event))
		cursor.ScrollLeft();
	else if (IsSystemTurnRightEvent(event))
		cursor.ScrollRight();
	else if (IsSystemEnterEvent(event))
	{
		int row, col;
		cursor.GetCell(row, col);

		if (!opp_field->IsUndefinedCell(row, col))
			return true;

		if (!web_mode)
		{
			BattleshipField::AttackResult res = my_field->Attack(row, col);
			opp_field->PutAttackResult(row, col, res);
		}
		else
		{
			// ходы в Web
			if (my_move && !fire_request.IsWaitingResponse())
			{
				//WebMessage msg_out = MakeGameMessage();
//
				//// Координаты
				//msg_out.data[6] = row + 1;
				//msg_out.data[7] = col + 1;
//
				//fire_request.MakeRequest(msg_out);

				/// TODO:
				//WebAckOut(msg_out);
			}
		}
	}

	int cur_row, cur_col;
	cursor.GetCell(cur_row, cur_col);
	opp_field->SetCursorPos(cur_row, cur_col);
	return false;
}

#include "../Bitmaps/BitmapsWithEffects.hpp"
#include <Data/StaticText.hpp>
#include <DrawFunctions/DrawLine.hpp>
#include <DrawFunctions/DrawRectangle.hpp>
#include <DrawFunctions/DrawBitmap.hpp>
#include <DrawFunctions/DrawText.hpp>

static void DrawLines(IScreen& screen, Point2_i lu, Point2_i rd, int step)
{
	using namespace DrawFunctions;

	for (int i = lu.y; i < rd.y; i += step)
	{
		DrawLine(screen, { lu.x, i }, { rd.x, i }, BlackColor);
	}
}

static void DrawWaitLines(IScreen& screen)
{
	DrawLines(screen, {0, 2}, {128, 64}, 2);
}

static void DrawTextCenter(IScreen& screen, int y, StaticTextView text)
{
	using namespace DrawFunctions;

	DrawText(screen, {64 - TextWidth(text) / 2, y}, text);
}

void GameScene::Draw(IScreen& screen)
{
	using namespace DrawFunctions;

	static StaticText32 text;

	my_field->Draw(screen, {1, 1});
	opp_field->Draw(screen, {65, 1});

	if (revanche_request.IsProcessingRequest())
	{
		DrawWaitLines(screen);

		text = "РЕВАНШ?";
		DrawTextCenter(screen, 8, text);
		//DrawText(screen, {64 - TextWidth(text) / 2, 8}, text);
		revanche_answer.Draw(screen);
		return;
	}
	else if (revanche_request.IsWaitingResponse())
	{
		DrawWaitLines(screen);

		text = "ЖДЕМ";
		DrawTextCenter(screen, 24, text);
		//DrawText(screen, {64 - TextWidth(text) / 2, 24}, text);
		text = "ОТВЕТА";
		DrawTextCenter(screen, 32, text);
		//DrawText(screen, {64 - TextWidth(text) / 2, 32}, text);
		return;
	}

	if (pause)
	{
		DrawWaitLines(screen);

		DrawRectangle(screen, { 2, 2 }, { 60, 60 }, BlackColor);

		pause_menu.Draw(screen);

		if (winner != 0)
		{
			text = (winner == 1) ? "ВЫ ВЫИГРАЛИ" : "ВЫ ПРОИГРАЛИ";
			DrawText(screen, {64 - TextWidth(text) / 2, 4}, text, WhiteColor, BlackColor, true);
		}
		return;
	}
	 
	cursor.Draw(screen, {65, 1});

	if (my_move && !show_res_timer.Expired(false))
	{
		float progress = show_res_timer.GetProgress();

		DrawLines(screen, {1, 2}, {10 * 6 + 1, 10 * 6 + 1}, progress < 0.25f ? 4 : 2);

		int sin_motion = static_cast<int>(2 * leto_api_v1->Math->sinf(50 * progress));

		if (show_res == BattleshipField::AttackResult::POPAL)
			DrawBitmap(screen, {6, progress < 0.25f ? 20 + sin_motion : 20}, BM_Popal_Sticker, WhiteColor, BlackColor);
		else if (show_res == BattleshipField::AttackResult::UBIL)
			DrawBitmap(screen, {6, 20 + sin_motion}, BM_Ubil_Sticker, WhiteColor, BlackColor);
	}

	const IFont* font = IFont::FromHandle(leto_api_v1->Font->GetFont(7, 7, 1));

	if (web_mode)
	{
		if (!online)
		{
			DrawWaitLines(screen);

			text = "СВЯЗЬ С";
			DrawText(screen, {64 - TextWidth(text) / 2, 20}, text, WhiteColor, BlackColor, false, font);
			text = opp_name;
			DrawText(screen, {64 - TextWidth(text) / 2, 28}, text, WhiteColor, BlackColor, true, font);
			text = "ПОТЕРЯНА";
			DrawText(screen, {64 - TextWidth(text) / 2, 36}, text, WhiteColor, BlackColor, false, font);
			return;

		}
		else if (!ready)
		{
			DrawWaitLines(screen);

			text = "ЖДЕМ";
			DrawText(screen, {64 - TextWidth(text) / 2, 20}, text, WhiteColor, BlackColor, false, font);
			text = "ИГРУ С";
			DrawText(screen, {64 - TextWidth(text) / 2, 28}, text, WhiteColor, BlackColor, false, font);
			text = opp_name;
			DrawText(screen, {64 - TextWidth(text) / 2, 36}, text, WhiteColor, BlackColor, true, font);
			return;
		}
	}

	// Анимация поверх всего
	//cursor_animation.Tick();
}

void GameScene::Loop()
{
	if (pause_multi_press_handler.MultiPressed(false))
	{
		if (left_hold_handler.Holded(150, ButtonHoldHandler::OnHoldPolicy::KEEP) && 
			right_hold_handler.Holded(150, ButtonHoldHandler::OnHoldPolicy::KEEP))
		{
			pause = true;

			left_hold_handler.Release();
			right_hold_handler.Release();
		}
	}
	else if (!web_mode || ready && online)
	{
		if (left_hold_handler.WaitHolded(300, 100))
		{
			if (cursor.IsActive())
				cursor.Left();
		}
		else if (right_hold_handler.WaitHolded(300, 100))
		{
			if (cursor.IsActive())
				cursor.Right();
		}

		if (up_hold_handler.WaitHolded(300, 100))
		{
			if (cursor.IsActive())
				cursor.Up();
		}
		else if (down_hold_handler.WaitHolded(300, 100))
		{
			if (cursor.IsActive())
				cursor.Down();
		}
	}

	if (ready_request.IsWaitingResponse() && check_ready_timer.Expired())
	{
		//WebMessage msg_out = MakeGameMessage();
//
		//ready_request.MakeRequest(msg_out);

		/// TODO:
		//WebAckOut(msg_out);

		check_ready_timer.Start();
	}

	int cur_row, cur_col;
	cursor.GetCell(cur_row, cur_col);
	opp_field->SetCursorPos(cur_row, cur_col);

	int prev_winner = winner;
	if (opp_field->ShipsCount() == 10)
	{
		winner = 1;
	}
	else if (my_field->AliveShipsCount() == 0)
	{
		winner = 2;
	}

	// Срабатывает один раз при определении победителя
	if (prev_winner == 0 && winner != 0)
	{
		pause = true;
		RefreshPauseMenu();
		ClearSave();
	} 

	cursor.SetActive(!winner);

	if (web_mode)
	{
		if (!my_move)
			cursor.SetActive(false);

		online = false;

		WebDeviceInfo_V1 info[8];
		uint32_t count = leto_api_v1->Web->GetDevicesNear(info, sizeof(info) / sizeof(WebDeviceInfo_V1));

		for (uint32_t i = 0; i < count; ++i)
		{
			if (info[i].channel == opp_channel)
			{
				online = true;
				break;
			}
		}
	}

	opp_field->SetCursorActive(cursor.IsActive());
}

//void GameScene::WebIn(const WebMessage& msg_in)
//{
//	if (!web_mode)
//		return;
//
//	// Если не относится к фазе игры
//	if (msg_in.data[4] != 0x03)
//		return;
//
//	// Реванш
//	if (revanche_request.ProcessRequest(msg_in))
//		return;
//
//	uint8_t resp;
//	if (revanche_request.ProcessResponse(msg_in, resp))
//	{
//		// Подтверждение
//		if (resp == YES_RESPONSE)
//		{
//			switch_revanche = true;
//			scene_manager->SwitchScene((uint32_t)Battleship_Scene::PLACE_SHIPS);
//		}
//		// Отказ
//		else if (resp == NO_RESPONSE)
//		{
//			scene_manager->SwitchScene((uint32_t)Battleship_Scene::MAIN);
//		}
//		return;
//	}
//
//	if (ready_request.ProcessResponse(msg_in))
//	{
//		ready = true;
//	}
//	else if (ready_request.ProcessRequest(msg_in))
//	{
//		WebMessage msg_out = msg_in;
//		ready_request.MakeResponse(msg_out, 1);
//		/// TODO:
//		//WebAckOut(msg_out);
//	}
//
//	if (end_request.ProcessRequest(msg_in))
//	{
//		scene_manager->SwitchScene((uint32_t)Battleship_Scene::MAIN);
//	}
//
//	// Если определен победитель, не учитываем атаки и результаты атак (чтобы лишний раз не сохраняться)
//	if (winner)
//		return;
//
//	int row = -1, col = -1;
//
//	// Координаты
//	if (msg_in.data[6])
//		row = msg_in.data[6] - 1;
//	if (msg_in.data[7])
//		col = msg_in.data[7] - 1;
//
//	WebMessage msg_out = msg_in;
//	// Атака
//	if (fire_request.ProcessRequest(msg_in))
//	{
//		BattleshipField::AttackResult res;
//		res = my_field->Attack(row, col);
//		Save();
//
//		fire_request.MakeResponse(msg_out, (uint8_t) res + 1);
//		/// TODO:
//		//WebAckOut(msg_out);
//	}
//	// Результат атаки
//	else if (fire_request.ProcessResponse(msg_in, resp))
//	{
//		BattleshipField::AttackResult res = (BattleshipField::AttackResult) (resp - 1);
//		opp_field->PutAttackResult(row, col, res);
//		Save();
//		if (res == BattleshipField::AttackResult::MIMO)
//			my_move = false;
//		else
//			ShowResult(res);
//	}
//}
//
//void GameScene::WebAckIn(const WebMessage& msg_in)
//{
//	if (!web_mode)
//		return;
//
//	// Если не относится к фазе игры
//	if (msg_in.data[4] != 0x03)
//		return;
//
//	// Получено эхо атаки соперника по полю хозяина
//	uint8_t resp{};
//	if (fire_request.IsResponse(msg_in, resp))
//	{
//		BattleshipField::AttackResult res = (BattleshipField::AttackResult)(resp - 1);
//		if (res == BattleshipField::AttackResult::MIMO)
//			my_move = true;
//	}
//
//	// Получено эхо ответа 
//	if (ready_request.IsResponse(msg_in))
//	{
//		ready = true;
//	}
//}
