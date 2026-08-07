/*
 * BattleshipSave.hpp
 *
 *  Created on: Dec 13, 2025
 *      Author: Timur
 */

#ifndef INC_BATTLESHIP_SAVE_HPP_
#define INC_BATTLESHIP_SAVE_HPP_

#include <stdint.h>
#include "../Logic/Ship.hpp"

enum { BATTLESHIP_FIELD_MAX = 10 };

// Сохранение поля (временное) - можно сохранять это поле в eeprom память
// для возможности возобновить игру при потере связи 
// (подтверждение целостности можно производить сравнением отправленного поля соперника со своим)
// удары должны совпадать. Если нет - подогнать свое поле под представление соперника
// Итого (пока что): 54 байта на одно сохранение
struct BattleshipFieldSave
{
	// Поле хозяина или сопреника, компактно упакованное по 2 бита на состояние (00,01,10,11)
	uint8_t field[BATTLESHIP_FIELD_MAX * BATTLESHIP_FIELD_MAX / 4]{};
};

// Сохранение игры (временное) - можно сохранять состояние игры в eeprom память
struct BattleshipSave
{
	// CRC16 имени соперника
	uint16_t crc16_opp_name{};

	// Объект с информацией о заполненных полях

	// Информация о своих кораблях
	Ship				my_ships[10]{};
	BattleshipFieldSave my_field_save{};
	BattleshipFieldSave opp_field_save{};

	// Сравнить и скорректировать разночтения
	bool CompareAndCorrect(BattleshipFieldSave& my_field_by_opp);
};

#endif