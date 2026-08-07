/*
 * BattleshipCursor.hpp
 *
 *  Created on: Dec 28, 2025
 *      Author: Timur
 */

#ifndef INC_GAME_TIC_TAC_TOE_CURSOR_HPP_
#define INC_GAME_TIC_TAC_TOE_CURSOR_HPP_

#include <stdint.h>
#include <Drawable/Circle.hpp>
#include <UI/ICursor.hpp>

namespace Battleship
{

	class BattleshipCursor final : public ICursor
	{
	protected:
		const int width, scale;

		void DrawCursor(IScreen& screen, Point2_i offset) override;

	public:
		BattleshipCursor(
			int rows_count, int cols_count,		// Размер игрового поля
			int width);							// Ширина курсора
	};

}

#endif
