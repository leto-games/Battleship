#include "GameData.hpp"

namespace Battleship
{
	bool web_mode = false;
	bool my_move;

	BattleshipMyField* my_field = nullptr;
	BattleshipOppField* opp_field = nullptr;

	StaticText32 opp_name;
	uint32_t opp_avatar;
	uint8_t opp_channel;

	//NamedSpace* CurrentSpace{};
	//NamedDataCell<BattleshipSave> GameSave("BTSP_SV", &CurrentSpace);

	WebRequest start_request(START_GAME);
	WebRequest ready_request(ASK_READY);
	WebRequest end_request(END_GAME);
	WebRequest fire_request(FIRE);
	WebRequest revanche_request(REVANCHE_GAME);

}
