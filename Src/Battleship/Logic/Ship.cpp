/*
 * Ship.cpp
 *
 *	Created on: Dec 19, 2025
 *		Author: Timur
 */

#include "Ship.hpp"

void Ship::SetShipCell(short cell, STATE state)
{
	if (cell < 0 || cell >= MAX_SHIP_CELLS)
		return;
	ship_state &= ~(0xF << (cell * 4));
	ship_state |= (state << (cell * 4));
}

Ship::STATE Ship::GetShipState(short cell) const
{
	return (Ship::STATE)((ship_state >> (cell * 4)) & 0xF);
}

Ship::Ship() :
	head_row{}, 
	head_col{}, 
	dir{}, 
	inited{ false },
	size{},
	width{},
	height{}
{
}

Ship::Ship(short head_row, short head_col, DIR dir, short size)
{
	SetPos(head_row, head_col, dir, size);
}

void Ship::Up()
{
	if (inited)
		return;

	if (head_row > 0)
		head_row--;
}

void Ship::Down()
{
	if (inited)
		return;

	if (head_row + height < 10)
		head_row++;
}

void Ship::Left()
{
	if (inited)
		return;

	if (head_col > 0)
		head_col--;
}

void Ship::Right()
{
	if (inited)
		return;

	if (head_col + width < 10)
		head_col++;
}

void Ship::Rotate()
{
	if (inited)
		return;

	if (dir == DIR::DOWN)
	{
		if (head_col + size <= 10)
			SetPos(head_row, head_col, DIR::RIGHT, size);
	}
	else if (dir == DIR::RIGHT)
	{
		if (head_row + size <= 10)
			SetPos(head_row, head_col, DIR::DOWN, size);
	}
}

void Ship::DeInit()
{
	inited = false;
}

void Ship::InitAsSelf()
{
	inited = true;
	for (int i = 0; i < size; ++i)
		SetShipCell(i, STATE::OK);
}

void Ship::InitAsDestroyedOpp()
{
	inited = true;
	for (int i = 0; i < size; ++i)
		SetShipCell(i, STATE::FIRED_OPP);
}

void Ship::FireSelf(short cell)
{
	SetShipCell(cell, STATE::FIRED_ME);
}

void Ship::SetPos(short head_row, short head_col, Ship::DIR dir, short size)
{
	// TODO: изменить head_row и head_col на head_row или что-то похожее
	this->head_row = head_row;
	this->head_col = head_col;
	this->dir = dir;
	this->size = size;

	if (dir == DIR::DOWN)
	{
		width = 1;
		height = size;
	}
	else if (dir == DIR::RIGHT)
	{
		width = size;
		height = 1;
	}
}

short Ship::GetSize() const
{
	return size;
}

void Ship::GetPos(short& head_row, short& head_col, DIR& dir, short& size) const
{
	head_row = this->head_row;
	head_col = this->head_col;
	dir = this->dir;
	size = this->size;
}

bool Ship::WasDestroyed() const
{
	for (int i = 0; i < size; ++i)
	{
		// Если хотя бы одна точка еще не определена, корабль не уничтожен
		// Аналогично с состоянием своего корабля, если OK - точно не уничтожен
		if (GetShipState(i) == STATE::N_A || GetShipState(i) == STATE::OK)
			return false;
	}

	// Если все 4 (или больше) ячеек FIRED_ME/FIRED_OPP/NONE, значит корабль уничтожен
	return true;
}

// Получить индекс ячейки корабля от 0 (если не в границах, возвращается -1)
short Ship::GetShipCell(int check_row, int check_col) const
{
	if (!IsInShip(check_row, check_col))
		return -1;

	if (dir == DIR::DOWN)
	{
		return check_row - head_row;
	}
	else if (dir == DIR::RIGHT)
	{
		return check_col - head_col;
	}

	return -1;
}

// Получить координаты конкретной ячейки
bool Ship::GetShipCellCoord(short cell, short& row, short& col) const
{
	if (cell >= MAX_SHIP_CELLS)
		return false;

	if (dir == DIR::DOWN)
	{
		row = head_row + cell;
		col = head_col;
	}
	else if (dir == DIR::RIGHT)
	{
		row = head_row;
		col = head_col + cell;
	}

	return true;
}

// Проверка находится ли точка внутри корабля
bool Ship::IsInShip(int check_row, int check_col) const
{
	return check_col >= head_col && check_col < head_col + width &&
		check_row >= head_row && check_row < head_row + height;
}

// Корабль находится слишком близко (при расположении)
bool Ship::IsNearError(const Ship& ship) const
{
	short up_row = head_row - 1, left_col = head_col - 1;
	short down_row = head_row + height, right_col = head_col + width;

	if (ship.head_row > down_row)
		return false;
	else if (ship.head_col > right_col)
		return false;
	else if (ship.head_col < left_col && ship.head_row < up_row)
		return false;
	else if (ship.head_col < left_col)
		return ship.head_col + ship.width - 1 >= left_col;
	else if (ship.head_row < up_row)
		return ship.head_row + ship.height - 1 >= up_row;
	return true;
}

#include "../Bitmaps/Bitmaps.hpp"
#include <DrawFunctions/DrawRectangle.hpp>
#include <DrawFunctions/DrawBitmap.hpp>

void Ship::Draw(IScreen& screen, Point2_i offset)
{
	using namespace DrawFunctions;
	// Отрисовка кубиков
	// Отличается для своих и не своих кораблей
	constexpr int scale = 6;

	bool white = true;

	Point2_i point_A = offset + Point2_i{head_col * scale, head_row * scale};
	Point2_i point_B = point_A + Point2_i{width * scale, height * scale};

	for (int idx = 0; idx < 3; ++idx, white = !white)
	{
		DrawRectangle(screen, point_A, point_B, white ? WhiteColor : BlackColor);
		point_A += {1, 1};
		point_B -= {1, 1};
	}

	if (GetShipState(0) == STATE::FIRED_OPP)
	{
		DrawRectangle(screen, point_A, point_B, white ? WhiteColor : BlackColor);
		return;
	}

	for (int i = 0; i < size; ++i)
	{
		short row, col;
		GetShipCellCoord(i, row, col);

		if (GetShipState(i) == STATE::FIRED_ME)
			DrawBitmap(screen, offset + Point2_i{col * scale, row * scale}, BM_DeadCell, WhiteColor, BlackColor);
	}
}

// ====================================================================================================
