/*
 * BattleshipGame.hpp
 *
 *  Created on: Dec 27, 2025
 *      Author: Timur
 */

#ifndef INC_GAME_BATTLESHIP_GAME_HPP_
#define INC_GAME_BATTLESHIP_GAME_HPP_

#include <GamesSupport/BaseGame.hpp>

class BattleshipGame final : public BaseGame<32, 512, 12 * 1024>
{
public:
	BattleshipGame(const AppBinHeader& header);

	void Draw(IScreen& screen) override;

protected:
	// Инициализация игры
	bool CustomInit() override;

	// Функция завершения игры
	void CustomClose() override;

	void CustomLoop() override;
};

extern BattleshipGame* GameInstance;

#endif
