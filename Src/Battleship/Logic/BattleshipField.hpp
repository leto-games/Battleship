/*
 * BattleshipField.hpp
 *
 *	Created on: Dec 18, 2025
 *		Author: Timur
 */

#ifndef INC_GAME_BATTLESHIP_FIELD_HPP_
#define INC_GAME_BATTLESHIP_FIELD_HPP_

#include <Data/StaticTable.hpp>
#include <Graphics/IScreen.hpp>

#include <Time/Timer.hpp>
#include "Ship.hpp"
#include <stdint.h>

#include <Data/StaticList.hpp>

#include "../Data/BattleshipSave.hpp"

// Базовое поле игры "Морской бой" 
class BattleshipField
{
protected:

	enum class FieldState : uint8_t
	{
		UNDEFINED,		// Состояние ячейки неопределено
		EMPTY,			// Ячейка гарантированно пустая
		ATTACKED,		// Ячейка содержит атакованную часть корабля
		MY_SHIP,		// Ячейка содержит неатакованную часть своего корабля
	};

public:
	enum class AttackResult { MIMO, POPAL, UBIL };

protected:
	StaticTable<FieldState, BATTLESHIP_FIELD_MAX, BATTLESHIP_FIELD_MAX> field;

	// По размеру можно определить, что поле не проинициализировано
	StaticList<Ship, 10> ships{};

	// Последний результат
	AttackResult last_result{};

	// Координаты последнего попадания
	short result_row{}, result_col{};

	// Анимация для отрисовки результата
	Timer anim_timer;

	// Положение курсора
	int cursor_row{}, cursor_col{};

	bool cursor_active = false;

	// Проверить границы координаты ячейки
	bool CheckRange(int row, int col) const;

	// Обвести корабль EMPTY-зоной
	void OutlineShip(const Ship& ship);

	BattleshipFieldSave GenerateFieldSave();
	void LoadFromFieldSave(BattleshipFieldSave& save);

	// Распарсить сохраненные лодки (по инфе о состояниях ячеек)
	void ParseSavedShips();

	// Определить направление атакованного корабля
	Ship::DIR ParseArrackedShip(int row, int col, int* p_head_row = nullptr, int* p_head_col = nullptr, int* p_size = nullptr) const;

	// Определить направление своего корабля
	Ship::DIR ParseOkShip(int row, int col, int* p_head_row = nullptr, int* p_head_col = nullptr, int* p_size = nullptr) const;

public:
	BattleshipField();
	virtual ~BattleshipField() = default;

	virtual void DrawAnim(IScreen& screen, float progress, Point2_i offset = {}) {};

	// Очистить поле
	void Clear();

	// Количество кораблей на поле
	int ShipsCount() const;

	// В поле наш корабль
	bool IsOKCell(int row, int col) const;

	// Атаковано ли поле (для определения границ убитого корабля)
	bool IsAttackedCell(int row, int col) const;

	// Проверка что поле гарантировано пустое (после "Мимо" или определения границ корабля)
	bool IsEmptyCell(int row, int col) const;

	// Проверка что поле не определено
	bool IsUndefinedCell(int row, int col) const;

	// Проверить возможность расположения корабля на поле
	bool CanPlaceShip(const Ship& ship) const;

	// Расположить корабль на поле (неважно чей, у чужого корабля ship_state будет = 1)
	virtual void PlaceShip(const Ship& ship);

	// Получить корабль для удаления или редактирования
	bool GetShip(int row, int col, Ship& ship);

	// Удалить корабль 
	void RemoveShip(int row, int col);

	void SetCursorActive(bool active);
	void SetCursorPos(int row, int col);

	void DrawCursor(IScreen& screen, Point2_i offset = {});

	// Отрисовка поля
	virtual void Draw(IScreen& screen, Point2_i offset = {});

	// Заполнить структуру с сохранением
	virtual void FillSave(BattleshipSave& save) {};

	// Загрузить поле из сохранения
	virtual void LoadSave(BattleshipSave& save) {};
};

// Поле игры хозяина
class BattleshipMyField final : public BattleshipField
{
public:
	BattleshipMyField();

	// Атаковать поле хозяина со стороны соперника
	AttackResult Attack(int row, int col);

	void DrawAnim(IScreen& screen, float progress, Point2_i offset = {}) override;

	int AliveShipsCount() const;

	void FillSave(BattleshipSave& save) override;
	void LoadSave(BattleshipSave& save) override;
};

// Поле игры соперника
class BattleshipOppField final : public BattleshipField
{
protected:
	// Обработать событие крушения корабля и сформировать корабль
	void ProcessKill(int row, int col);

public:
	BattleshipOppField();

	// Проверить возможность ударить по ячейке (с защитой от уже сделанных ударов)
	bool CanAttack(int row, int col) const;

	// Установить результат атаки хозяина в поле соперника
	void PutAttackResult(int row, int col, AttackResult result);

	void PlaceShip(const Ship& ship) override;

	void Draw(IScreen& screen, Point2_i offset = {}) override;

	void FillSave(BattleshipSave& save) override;
	void LoadSave(BattleshipSave& save) override;
};

#endif
