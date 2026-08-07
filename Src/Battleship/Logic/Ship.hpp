/*
 * Ship.hpp
 *
 *	Created on: Dec 18, 2025
 *		Author: Timur
 */

#ifndef INC_GAME_BATTLESHIP_SHIP_HPP_
#define INC_GAME_BATTLESHIP_SHIP_HPP_

#ifdef LIB_DUMMY
#include "Dummy.hpp"
#else
#include <Graphics/IScreen.hpp>
#endif

#include <stdint.h>

class BattleshipField;

class Ship
{
public:
	enum class DIR : unsigned char { NONE, DOWN, RIGHT };

protected:
	friend BattleshipField;

	enum { MAX_SHIP_CELLS = 4 };
	enum STATE : unsigned char { NONE, N_A, OK, FIRED_ME, FIRED_OPP };

	// левый верхний угол корабля
	unsigned char head_row : 4;
	unsigned char head_col : 4;
	// Направление корабля: 1 - вниз, 2 - вправо
	DIR dir : 3;
	bool inited : 1;

	// Размер корабля, 1-4
	unsigned char size : 4;
	unsigned char width : 4;
	unsigned char height : 4;

	uint16_t ship_state{}; 	// Состояние ячеек корабля 1 - неопределено (для вражеских), 2 - целый (для своих), 3 - атакован (вражеский), 4 - атакован (свой)

	void SetShipCell(short cell, STATE state);
	STATE GetShipState(short cell) const;

public:
	Ship();
	Ship(short head_row, short head_col, DIR dir, short size);

	void Up();
	void Down();
	void Left();
	void Right();
	void Rotate();

	void DeInit();

	// Инициализировать как свой корабль
	void InitAsSelf();

	// Инициализировать как разрушенный вражеский корабль
	void InitAsDestroyedOpp();

	// Поджечь свой корабль
	void FireSelf(short cell);

	// Установить координату, позицию и размер
	void SetPos(short head_row, short head_col, DIR dir, short size);

	short GetSize() const;

	// Получить координату, позицию и размер
	void GetPos(short& head_row, short& head_col, DIR& dir, short& size) const;

	// Определение, разрушен ли корабль
	bool WasDestroyed() const;

	// Получить индекс ячейки корабля от 0 (если не в границах, возвращается -1)
	short GetShipCell(int check_row, int check_col) const;

	// Получить координаты конкретной ячейки
	bool GetShipCellCoord(short cell, short& row, short& col) const;

	// Проверка находится ли точка внутри корабля
	bool IsInShip(int check_row, int check_col) const;

	// Корабль находится слишком близко (при расположении)
	bool IsNearError(const Ship& ship) const;

	void Draw(IScreen& screen, Point2_i offset);

};

#endif