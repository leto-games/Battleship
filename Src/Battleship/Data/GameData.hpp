/*
 * GameData.hpp
 *
 *  Created on: Dec 13, 2025
 *      Author: Timur
 */

#ifndef INC_GAME_BATTLESHIP_GAME_DATA_HPP_
#define INC_GAME_BATTLESHIP_GAME_DATA_HPP_

#include <stdint.h>

#include "BattleshipSave.hpp"
#include "../Logic/BattleshipField.hpp"
#include <Web/WebRequest.hpp>
#include <Storage/NamedDataCell.hpp>
//#include <Storage/NamedSpace.hpp>
#include <Data/StaticText.hpp>

namespace Battleship
{
	extern bool web_mode;
	extern bool my_move;

	extern BattleshipMyField* my_field;
	extern BattleshipOppField* opp_field;

	extern StaticText32 opp_name;
	extern uint32_t opp_avatar;
	extern uint8_t opp_channel;

	//extern NamedSpace* CurrentSpace;
	//extern NamedDataCell<BattleshipSave> GameSave;

	enum RequestType
	{
		// Запрос на начало игры
		START_GAME = 1,
		// Уведомление о готовности начать
		ASK_READY,
		// Запрос на завершение игры
		END_GAME,
		// Атака
		FIRE,
		// Реванш
		REVANCHE_GAME,
	};

	enum YesNoResponseType
	{
		// Подтвеждение
		YES_RESPONSE = 1,
		// Отказ
		NO_RESPONSE,
	};

	enum FireResponseType
	{
		MIMO_RESPONSE = 1,
		POPAL_RESPONSE,
		UBIL_RESPONSE,
	};

	// Запрос на начало игры
	extern WebRequest start_request;

	// Запрос на готовность игры
	extern WebRequest ready_request;

	// Запрос на прерывание игры
	extern WebRequest end_request;

	// Запрос на огонь (для сетевой версии)
	extern WebRequest fire_request;

	// Запрос на реванш
	extern WebRequest revanche_request;

};

#endif
