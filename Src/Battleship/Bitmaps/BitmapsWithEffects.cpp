#include "BitmapsWithEffects.hpp"

#include <Graphics/BitmapEffects.hpp>

#include "BM_PopalText.h"
#include "BM_UbilText.h"

BitmapData BM_Popal_Sticker;
BitmapData BM_Popal_Sticker_Rotating;
BitmapData BM_Ubil_Sticker;

void InitStickers(const LetoAllocator_V1* allocator)
{
    BM_Popal_Sticker = BitmapEffects::ResizeCopy(BM_PopalText, allocator, {52, 20});
    BM_Ubil_Sticker = BitmapEffects::ResizeCopy(BM_UbilText, allocator, {52, 20});

	BitmapEffects::MakeSticker(BM_Popal_Sticker, allocator, 2);
    BitmapEffects::MakeSticker(BM_Ubil_Sticker, allocator, 2);

    BM_Popal_Sticker_Rotating.CopyFrom(allocator, BM_Popal_Sticker);
}
