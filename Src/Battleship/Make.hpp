/*
 * Make.hpp
 *
 *  Created on: Feb 23, 2025
 *      Author: Timur
 */

#ifndef INC_BATTLESHIP_MAKE_HPP_
#define INC_BATTLESHIP_MAKE_HPP_

#include "GamesCollectionExport.h"
#include <GamesSupport/BaseGame.hpp>
#include <GamesSupport/GameInfo.hpp>
#include <LetoAPI_V1/Application/LetoApplication_V1.h>


extern "C"
{
#ifdef USE_HAL_DRIVER
	__attribute__((section(".game_meta"), used, aligned(4)))
#endif
	extern GAMES_COLLECTION_EXPORT LetoApplication_V1 LETO_EXECUTABLE_INFO_VAR_NAME;
};

#endif
