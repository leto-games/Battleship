/*
 * SceneID.hpp
 *
 *  Created on: Dec 27, 2025
 *      Author: Timur
 */

#ifndef INC_GAME_BATTLESHIP_SCENE_ID_HPP_
#define INC_GAME_BATTLESHIP_SCENE_ID_HPP_

#include <stdint.h>

enum class Battleship_Scene : uint8_t
{
	EXIT,			// Выход из игры
	WELCOME,		// Сцена с приветствием
	MAIN,			// Главная сцена с "Играть", "Настройки" и т.д.
	CHOOSE_GM,		// Подсцена с выбором режима игры
	PLAY_WEB,		// Подсцена с настройкой режима игры вдвоем по сети
	PLACE_SHIPS,	// Подсцена с расстановкой кораблей
	GAME,			// Игровая сцена с процессом
	SETTINGS,		// Подсцена с настройками
	TEST,			// Тестовая подсцена
};

#endif