/**
 * @file 
 * @date Mar 25, 2026
 * @author Rakhimov T.
 */

#ifndef INC_GAME_BATTLESHIP_BITMAPS_WITH_EFFECTS_HPP_
#define INC_GAME_BATTLESHIP_BITMAPS_WITH_EFFECTS_HPP_

#include <Graphics/BitmapData.hpp>
#include <Data/IAllocator.hpp>
#include <LetoAPI_V1/Globals/LetoAllocator_V1.h>

/// @brief Стикер "Попал"
extern LetoBitmap_V1* BM_Popal_Top;

/// @brief Стикер "Убил"
extern LetoBitmap_V1* BM_Ubil_Top;

/// @brief Стикер "Попал"
extern LetoBitmap_V1* BM_Popal_Sticker;

/// @brief Стикер "Убил"
extern LetoBitmap_V1* BM_Ubil_Sticker;

/**
 * @brief Инициализация битмапов "с эффектами"
 */
extern void InitStickers(const LetoAllocator_V1* allocator);


// /// @brief Стикет "Попал" с вращением
// extern BitmapData BM_Popal_Sticker_Rotating;

#endif
