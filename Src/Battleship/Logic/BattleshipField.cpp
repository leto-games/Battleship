/*
 * BattleshipField.cpp
 *
 *	Created on: Dec 19, 2025
 *		Author: Timur
 */

#include "BattleshipField.hpp"

BattleshipField::BattleshipField()
{
}

bool BattleshipField::CheckRange(int row, int col) const
{
	// Сначала определить, можем ли мы установить корабль в этом месте по границам
	if (row < 0 || col < 0 || 
		row >= BATTLESHIP_FIELD_MAX ||
		col >= BATTLESHIP_FIELD_MAX)
		return false;
	return true;
}

bool BattleshipField::IsOKCell(int row, int col) const
{
	BattleshipField::FieldState state;
	return field.GetCell(row, col, state) && state == BattleshipField::FieldState::MY_SHIP;
}

bool BattleshipField::IsAttackedCell(int row, int col) const
{
	BattleshipField::FieldState state;
	return field.GetCell(row, col, state) && 
		(state == BattleshipField::FieldState::ATTACKED);
}

bool BattleshipField::IsEmptyCell(int row, int col) const
{
	BattleshipField::FieldState state;
	return field.GetCell(row, col, state) && state == BattleshipField::FieldState::EMPTY;
}

bool BattleshipField::IsUndefinedCell(int row, int col) const
{
	BattleshipField::FieldState state;
	return field.GetCell(row, col, state) && state == BattleshipField::FieldState::UNDEFINED;
}

void BattleshipField::OutlineShip(const Ship& ship)
{
	for (int row = ship.head_row - 1; row <= ship.head_row + ship.height; ++row)
	{
		for (int col = ship.head_col - 1; col <= ship.head_col + ship.width; ++col)
		{
			if (!ship.IsInShip(row, col))
				field.SetCell(row, col, BattleshipField::FieldState::EMPTY);
		}
	}
}

BattleshipFieldSave BattleshipField::GenerateFieldSave()
{
	BattleshipFieldSave save{};

	for (int row = 0; row < 10; ++row)
	{
		for (int col = 0; col < 10; ++col)
		{
			int idx = row * BATTLESHIP_FIELD_MAX + col;

			FieldState state;
			if (!field.GetCell(row, col, state))
				continue;

			save.field[idx / 4] |= ( ( ( (uint8_t) state) & 0x3 ) << ((idx % 4) * 2) );
		}
	}

	return save;
}

void BattleshipField::LoadFromFieldSave(BattleshipFieldSave & save)
{
	Clear();
	for (int row = 0; row < 10; ++row)
	{
		for (int col = 0; col < 10; ++col)
		{
			int idx = row * BATTLESHIP_FIELD_MAX + col;

			uint8_t u_state = (save.field[idx / 4] >> ((idx % 4) * 2)) & 0x3;

			field.SetCell(row, col, (FieldState) u_state);
		}
	}
}

void BattleshipField::ParseSavedShips()
{
	for (int row = 0; row < 10; ++row)
	{
		for (int col = 0; col < 10; ++col)
		{
			if (!IsAttackedCell(row, col))
				continue;

			bool parsed = false;
			for (Ship& ship : ships)
			{
				if (ship.IsInShip(row, col))
				{
					parsed = true;
					break;
				}
			}

			if (parsed)
				continue;

			// Сканирование плоскости
			int size{};
			Ship::DIR dir = ParseArrackedShip(row, col, nullptr, nullptr, &size);

			if (dir == Ship::DIR::RIGHT)
			{
				int left_col = col, right_col = left_col + size - 1;

				// Еще неопределенный корабль
				if (left_col > 0 && !IsEmptyCell(row, left_col - 1))
				{
					continue;
				}
				else if (right_col < BATTLESHIP_FIELD_MAX - 1 && !IsEmptyCell(row, right_col + 1))
				{
					continue;
				}
								
			}
			else if (dir == Ship::DIR::DOWN)
			{
				int up_row = row, down_row = up_row + size - 1;

				// Еще неопределенный корабль
				if (up_row > 0 && !IsEmptyCell(up_row - 1, col))
				{
					continue;
				}
				else if (down_row < BATTLESHIP_FIELD_MAX - 1 && !IsEmptyCell(down_row + 1, col))
				{
					continue;
				}

			}
			else
			{
				// Еще неопределенный корабль
				if (col > 0 && !IsEmptyCell(row, col - 1))
				{
					continue;
				}
				else if (col < BATTLESHIP_FIELD_MAX - 1 && !IsEmptyCell(row, col + 1))
				{
					continue;
				}
				else if (row > 0 && !IsEmptyCell(row - 1, col))
				{
					continue;
				}
				else if (row < BATTLESHIP_FIELD_MAX - 1 && !IsEmptyCell(row + 1, col))
				{
					continue;
				}
				else
				{
					dir = Ship::DIR::DOWN;
					size = 1;
				}
			}

			Ship ship{};
			ship.SetPos(row, col, dir, size);
			ship.InitAsDestroyedOpp();
			ships.Push(ship);
		}
	}
}

Ship::DIR BattleshipField::ParseArrackedShip(int row, int col, int* p_head_row, int* p_head_col, int* p_size) const
{
	int head_row = row;
	int head_col = col;
	short size = 0;
	Ship::DIR dir = Ship::DIR::NONE;

	if (IsAttackedCell(row - 1, col) || IsAttackedCell(row + 1, col))
	{
		dir = Ship::DIR::DOWN;
		while (IsAttackedCell(head_row - 1, head_col))
			head_row--;
		while (IsAttackedCell(head_row + size, head_col))
			size++;
	}
	else if (IsAttackedCell(row, col - 1) || IsAttackedCell(row, col + 1))
	{
		dir = Ship::DIR::RIGHT;
		while (IsAttackedCell(head_row, head_col - 1))
			head_col--;
		while (IsAttackedCell(head_row, head_col + size))
			size++;
	}

	if (p_head_row) *p_head_row = head_row;
	if (p_head_col) *p_head_col = head_col;
	if (p_size) *p_size = size;

	return dir;
}

Ship::DIR BattleshipField::ParseOkShip(int row, int col, int* p_head_row, int* p_head_col, int* p_size) const
{
	int head_row = row;
	int head_col = col;
	short size = 0;
	Ship::DIR dir = Ship::DIR::NONE;

	if (IsOKCell(row - 1, col) || IsOKCell(row + 1, col))
	{
		dir = Ship::DIR::DOWN;
		while (IsOKCell(head_row - 1, head_col))
			head_row--;
		while (IsOKCell(head_row + size, head_col))
			size++;
	}
	else if (IsOKCell(row, col - 1) || IsOKCell(row, col + 1))
	{
		dir = Ship::DIR::RIGHT;
		while (IsOKCell(head_row, head_col - 1))
			head_col--;
		while (IsOKCell(head_row, head_col + size))
			size++;
	}

	if (p_head_row) *p_head_row = head_row;
	if (p_head_col) *p_head_col = head_col;
	if (p_size) *p_size = size;

	return dir;
}

void BattleshipField::Clear()
{
	field.Clear();
	ships.Clear();
}

int BattleshipField::ShipsCount() const
{
	return ships.Count();
}

// Проверить возможность расположения корабля на поле
bool BattleshipField::CanPlaceShip(const Ship& ship) const
{
	if (!CheckRange(ship.head_row, ship.head_col) ||
		!CheckRange(ship.head_row + ship.height - 1, ship.head_col + ship.width - 1))
		return false;

	// Затем по конфликту с другими кораблями
	for (const Ship& _ship : ships)
		if (_ship.IsNearError(ship))
			return false;

	return true;
}

// Расположить корабль на поле (неважно чей, у чужого корабля ship_state будет = 1)
void BattleshipField::PlaceShip(const Ship& ship)
{
	if (!CanPlaceShip(ship))
		return;
	ships.Push(ship);
}

// Получить корабль для удаления или редактирования (или для обновления информции о чужом корабле)
bool BattleshipField::GetShip(int row, int col, Ship& ship)
{
	for (Ship& _ship : ships)
	{
		// Алгоритм для определения нужного корабля
		if (_ship.IsInShip(row, col))
		{
			ship = _ship;
			return true;
		}
	}
	return false;
}

void BattleshipField::RemoveShip(int row, int col)
{
	for (int i = 0; i < ships.Count(); ++i)
	{
		if (ships[i].IsInShip(row, col))
		{
			ships.Erase(ships.begin() + i);
			return;
		}
	}
}

void BattleshipField::SetCursorActive(bool active)
{
	cursor_active = active;
}

void BattleshipField::SetCursorPos(int row, int col)
{
	if (!CheckRange(row, col))
		return;
	cursor_row = row;
	cursor_col = col;
}

#include "../Bitmaps/Bitmaps.hpp"
#include <DrawFunctions/DrawLine.hpp>
#include <DrawFunctions/DrawCircle.hpp>
#include <DrawFunctions/DrawRectangle.hpp>
#include <DrawFunctions/DrawBitmap.hpp>

// Отрисовка курсора
void BattleshipField::DrawCursor(IScreen& screen, Point2_i offset)
{
	const int scale = 6;

	if (!cursor_active)
		return;

	Point2_i cursor_top = offset + Point2_i{cursor_col * scale + scale / 2, 0};
	Point2_i cursor_bottom = cursor_top + Point2_i{0, 10 * scale};
	Point2_i cursor_left = offset + Point2_i{0, cursor_row * scale + scale / 2};
	Point2_i cursor_right = cursor_left + Point2_i{10 * scale, 0};

	// Col
	screen.PixelSet(cursor_top, BlackColor);
	screen.PixelSet(cursor_top + Point2_i{-1, -1}, WhiteColor);
	screen.PixelSet(cursor_top + Point2_i{+1, -1}, WhiteColor);

	screen.PixelSet(cursor_bottom, BlackColor);
	screen.PixelSet(cursor_bottom + Point2_i{-1, +1}, WhiteColor);
	screen.PixelSet(cursor_bottom + Point2_i{+1, +1}, WhiteColor);

	// Row
	screen.PixelSet(cursor_left, BlackColor);
	screen.PixelSet(cursor_left + Point2_i{-1, -1}, WhiteColor);
	screen.PixelSet(cursor_left + Point2_i{-1, +1}, WhiteColor);

	screen.PixelSet(cursor_right, BlackColor);
	screen.PixelSet(cursor_right + Point2_i{+1, -1}, WhiteColor);
	screen.PixelSet(cursor_right + Point2_i{+1, +1}, WhiteColor);

	bool is_in_ship = false;
	for (Ship& _ship : ships)
	{
		if (_ship.IsInShip(cursor_row, cursor_col))
		{
			is_in_ship = true;
			break;
		}
	}

	if (!is_in_ship)
	{
		if (IsUndefinedCell(cursor_row, cursor_col))
			return;
		
		if (IsAttackedCell(cursor_row, cursor_col))
		{
			offset += (scale / 2);
			offset += {cursor_col * scale, cursor_row * scale};

			screen.PixelSet(offset, BlackColor );
			screen.PixelSet(offset + Point2_i{-1, +0}, BlackColor);
			screen.PixelSet(offset + Point2_i{+1, +0}, BlackColor);
			screen.PixelSet(offset + Point2_i{+0, -1}, BlackColor);
			screen.PixelSet(offset + Point2_i{+0, +1}, BlackColor);
			return;
		}
	}

	for (int x = cursor_col * scale + 1 + offset.x; x < (cursor_col + 1) * scale + offset.x; ++x)
	{
		for (int y = cursor_row * scale + 1 + offset.y; y < (cursor_row + 1) * scale + offset.y; ++y)
		{
			RGBColor pixel = screen.PixelGet({x, y});
			screen.PixelSet({x, y}, pixel == WhiteColor ? BlackColor : WhiteColor);
		}
	}
}

// Отрисовка поля
void BattleshipField::Draw(IScreen& screen, Point2_i offset)
{
	using namespace DrawFunctions;

	const int scale = 6;

	// Отрисовка поля (на нем же хранится)
	for (int row = 0; row < 10; ++row)
	{
		for (int col = 0; col < 10; ++col)
		{
			FieldState state;
			if (!field.GetCell(row, col, state))
				continue;
			
			Point2_i point = offset + Point2_i{col * scale, row * scale};

			screen.PixelSet(point, WhiteColor);

			if (IsEmptyCell(row, col))
			{
				DrawBitmap(screen, point + Point2_i{1, 1}, BM_EmptyCell, WhiteColor);
			}
			else if (IsAttackedCell(row, col))
			{
				point += (scale / 2);

				DrawCircle(screen, point, 2, WhiteColor);

				Point2_i point_A = point - Point2_i{1, 1};
				Point2_i point_B{ point_A };

				// Если атакована ячейка ниже, рисуем переход
				if (IsAttackedCell(row + 1, col))
				{
					point_B += Point2_i{2, 2 + scale};
					DrawRectangle(screen, point_A, point_B, WhiteColor);
				}
				// Если атакована ячейка правее, рисуем переход
				else if (IsAttackedCell(row, col + 1))
				{
					point_B += Point2_i{2 + scale, 2};
					DrawRectangle(screen, point_A, point_B, WhiteColor);
				}
			}
		}
	}

	const Point2_i lu_bound = offset;
	const Point2_i ru_bound = lu_bound + Point2_i{ 10 * scale	, 0 };
	const Point2_i ld_bound = lu_bound + Point2_i{ 0			, 10 * scale };
	const Point2_i rd_bound = ld_bound + Point2_i{ 10 * scale, 0 };

	// Рамки поля
	DrawLine(screen, lu_bound, ru_bound, WhiteColor);
	DrawLine(screen, lu_bound, ld_bound, WhiteColor);
	DrawLine(screen, ld_bound, rd_bound, WhiteColor);
	DrawLine(screen, ru_bound, rd_bound, WhiteColor);

	// Корабельная отрисовка
	for (Ship& _ship : ships)
	{
		_ship.Draw(screen, offset);
	}

	DrawCursor(screen, offset);

	if (!anim_timer.Expired())
	{
		DrawAnim(screen, anim_timer.GetProgress(), offset);
	}
}

// ====================================================================================================

BattleshipMyField::BattleshipMyField()
{
}

void BattleshipMyField::DrawAnim(IScreen& screen, float progress, Point2_i offset)
{
	using namespace DrawFunctions;
	const int scale = 6;

	//if (last_result != AttackResult::POPAL &&
	//	last_result != AttackResult::UBIL)
	//	return;

	if ((int)(progress * 100) % 50 > 25)
	{
		Point2_i coords{ result_col * scale + 1 + offset.x, result_row * scale + 1 + offset.y };
		Point2_i coords2{ coords.x + 4, coords.y + 4 };
		DrawRectangle(screen, coords, coords2, last_result == AttackResult::MIMO ? BlackColor : WhiteColor);
	}
	else
	{

	}

}

BattleshipField::AttackResult BattleshipMyField::Attack(int row, int col)
{
	anim_timer.Start(900);
	result_row = row;
	result_col = col;

	last_result = AttackResult::MIMO;

	for (Ship& _ship : ships)
	{
		short cell = _ship.GetShipCell(row, col);
		// Алгоритм для определения нужного корабля
		if (cell >= 0)
		{
			field.SetCell(row, col, FieldState::ATTACKED);
			_ship.FireSelf(cell);
			last_result = AttackResult::POPAL;
			if (_ship.WasDestroyed())
			{
				OutlineShip(_ship);
				last_result = AttackResult::UBIL;
			}
			return last_result;
		}
	}
	field.SetCell(row, col, FieldState::EMPTY);
	return last_result;
}

int BattleshipMyField::AliveShipsCount() const
{
	int count = 0;
	for (const Ship& _ship : ships)
	{
		if (!_ship.WasDestroyed())
			count++;
	}
	return count;
}

void BattleshipMyField::FillSave(BattleshipSave & save)
{
	save.my_field_save = GenerateFieldSave();
	for (int i = 0; i < ships.Count() && i < 10; ++i)
		save.my_ships[i] = ships[i];
}

void BattleshipMyField::LoadSave(BattleshipSave & save)
{
	// Загрузка своих лодок
	LoadFromFieldSave(save.my_field_save);
	ships.Clear();
	for (Ship& ship : save.my_ships)
	{
		if (ship.GetSize() != 0)
			ships.Push(ship);
	}
}

// ====================================================================================================

void BattleshipOppField::ProcessKill(int row, int col)
{
	int head_row = row;
	int head_col = col;
	int size = 0;

	Ship::DIR dir = ParseArrackedShip(row, col, &head_row, &head_col, &size);

	if (dir == Ship::DIR::NONE)
	{
		dir = Ship::DIR::DOWN;
		size = 1;
	}

	if (size == 0)
		return;

	Ship ship(head_row, head_col, dir, size);
	ship.InitAsDestroyedOpp();
	PlaceShip(ship);
}

BattleshipOppField::BattleshipOppField()
{
}

bool BattleshipOppField::CanAttack(int row, int col) const
{
	// Или IsUndefinedCell, но большой разницы нет, все равно две функции будут
	return !IsAttackedCell(row, col) && !IsEmptyCell(row, col);
}

void BattleshipOppField::PutAttackResult(int row, int col, BattleshipField::AttackResult result)
{
	if (result == BattleshipField::AttackResult::MIMO)
		field.SetCell(row, col, FieldState::EMPTY);
	else
		field.SetCell(row, col, FieldState::ATTACKED);

	if (result == BattleshipField::AttackResult::UBIL)
	{
		ProcessKill(row, col);
	}
}

void BattleshipOppField::PlaceShip(const Ship& ship)
{
	BattleshipField::PlaceShip(ship);
	OutlineShip(ship);
}

void BattleshipOppField::Draw(IScreen& screen, Point2_i offset)
{
	BattleshipField::Draw(screen, offset);
}

void BattleshipOppField::FillSave(BattleshipSave & save)
{
	save.opp_field_save = GenerateFieldSave();
}

void BattleshipOppField::LoadSave(BattleshipSave & save)
{
	LoadFromFieldSave(save.opp_field_save);
	ParseSavedShips();
}

