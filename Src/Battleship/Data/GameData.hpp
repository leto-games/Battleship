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
#include <Data/SourceCell.hpp>

#include <GamesSupport/Lobby/LobbyDataProcessor.hpp>

namespace Battleship
{
	template <typename T>
	using BattleshipCell = SourceCell<T, uint32_t, 2>;

	extern bool web_mode;
	extern bool my_move;

	extern BattleshipMyField* my_field;
	extern BattleshipOppField* opp_field;

	extern StaticText32 opp_name;
	extern uint32_t opp_avatar;
	extern uint32_t opp_id;
	extern uint8_t opp_channel;

	enum GameStage 
	{
		STAGE_NONE,
		STAGE_PLACE_SHIPS,
		STAGE_GAME_MY_TURN, 	// REQ_ATTACK
		STAGE_GAME_OPP_TURN,
		STAGE_FINISH
	};
	extern BattleshipCell<uint8_t> GameStage;

	enum Request
	{
		REQ_NONE,
		REQ_ATTACK, // Запрос на атаку (активируется только из NONE -> REQ_ATTACK)
		REQ_REVENGE,
		REQ_YOUR_TURN
	};
	extern BattleshipCell<uint8_t> Request;

	enum Response
	{
		RESP_NONE,
		RESP_ATTACK_MIMO,
		RESP_ATTACK_POPAL,
		RESP_ATTACK_UBIL,
		RESP_REVENGE_YES,
		RESP_REVENGE_NO,
	};
	extern BattleshipCell<uint8_t> Response;

	extern BattleshipCell<uint8_t> CursorRow;
	extern BattleshipCell<uint8_t> CursorCol;

	extern LobbyDataProcessor data_processor;

	extern struct CommonInfo : public LobbyData
	{
		CommonInfo(uint8_t data_id) : LobbyData(data_id, 200) { }
		
		bool OnSend(void* data, uint32_t size) override
		{
			if (size < 5) return false; // !!! TODO
			
			uint8_t* u_data = static_cast<uint8_t*>(data);

			u_data[0] = GameStage.GetOrDefault();
			u_data[1] = Request.GetOrDefault();
			u_data[2] = Response.GetOrDefault();
			u_data[3] = CursorRow.GetOrDefault();
			u_data[4] = CursorCol.GetOrDefault();

			return true;
		}
		
		void OnReceive(uint32_t id, const void* data, uint32_t size) override
		{
			if (size < 5) return; // !!! TODO

			const uint8_t* u_data = static_cast<const uint8_t*>(data);
			
			GameStage.Set(u_data[0], id);
			Request.Set(u_data[1], id);
			Response.Set(u_data[2], id);
			CursorRow.Set(u_data[3], id);
			CursorCol.Set(u_data[4], id);
		}

		void OnTimeout() override
		{
			GameStage.Clear(opp_id);
			Request.Clear(opp_id);
			Response.Clear(opp_id);
			CursorRow.Clear(opp_id);
			CursorCol.Clear(opp_id);
		}
	}
	LobbyData_Common;
};

#endif
