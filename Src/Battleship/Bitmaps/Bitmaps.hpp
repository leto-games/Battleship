/*
 * Bitmaps.hpp
 *
 *  Created on: Dec 13, 2025
 *      Author: Timur
 */

#ifndef INC_GAME_BATTLESHIP_BITMAPS_HPP_
#define INC_GAME_BATTLESHIP_BITMAPS_HPP_

#include <stdint.h>
#include <Data/StaticList.hpp>

#include "BitmapsWithEffects.hpp"

#include "BM_BattleshipLogo.h"
#include "BM_Ship.h"
#include "BM_DeadCell.h"
#include "BM_EmptyCell.h"

// Анимация взрыва
#include "BM_Bomb_0.h"
#include "BM_Bomb_1.h"
#include "BM_Bomb_2.h"
#include "BM_Bomb_3.h"
#include "BM_Bomb_4.h"
#include "BM_Bomb_5.h"

#include "BM_PopalText.h"
#include "BM_UbilText.h"

extern StaticList<BitmapData*, 8> BM_Bombs;

#endif
