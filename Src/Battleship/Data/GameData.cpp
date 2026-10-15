#include "GameData.hpp"

namespace Battleship
{
	bool web_mode = false;
	bool my_move;

	BattleshipMyField* my_field = nullptr;
	BattleshipOppField* opp_field = nullptr;

	StaticText32 opp_name;
	uint32_t opp_avatar;
	uint32_t opp_id;
	uint8_t opp_channel;

	BattleshipCell<uint8_t> GameStage;
	BattleshipCell<uint8_t> Request;
	BattleshipCell<uint8_t> Response;
	BattleshipCell<uint8_t> CursorX;
	BattleshipCell<uint8_t> CursorY;

	LobbyDataProcessor data_processor;

	CommonInfo LobbyData_Common{0x01};
}
