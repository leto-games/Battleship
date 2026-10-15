#include "BitmapsWithEffects.hpp"

//#include <Graphics/BitmapEffects.hpp>

#include <LetoAPI_V1/LetoAPI_V1.h>

#include "BM_PopalText.h"
#include "BM_UbilText.h"

LetoBitmap_V1* BM_Popal_Top{};
LetoBitmap_V1* BM_Ubil_Top{};

LetoBitmap_V1* BM_Popal_Sticker{};
LetoBitmap_V1* BM_Ubil_Sticker{};

//BitmapData BM_Popal_Sticker_Rotating;

void InitStickers(const LetoAllocator_V1* allocator)
{
    BM_Popal_Top = leto_api_v1->Bitmap->ResizeCopyBitmap(BitmapData::ToHandle(&BM_PopalText), allocator, 52, 20);
    BM_Ubil_Top = leto_api_v1->Bitmap->ResizeCopyBitmap(BitmapData::ToHandle(&BM_UbilText), allocator, 52, 20);
    //BM_Popal_Sticker = BitmapEffects::ResizeCopy(BM_PopalText, allocator, {52, 20});
    //BM_Ubil_Sticker = BitmapEffects::ResizeCopy(BM_UbilText, allocator, {52, 20});
    
    //BM_Popal_Outlined = leto_api_v1->Bitmap->MakeOutlinedBitmap(BM_Popal_Big, LoadedGameAllocator(), 2);
    //BM_Ubil_Outlined = leto_api_v1->Bitmap->MakeOutlinedBitmap(BM_Ubil_Big, LoadedGameAllocator(), 2);

    BM_Popal_Sticker = leto_api_v1->Bitmap->MakeStickerBitmap(BM_Popal_Top, allocator, 2);
    BM_Ubil_Sticker = leto_api_v1->Bitmap->MakeStickerBitmap(BM_Ubil_Top, allocator, 2);

//
	//BitmapEffects::MakeSticker(BM_Popal_Sticker, allocator, 2);
    //BitmapEffects::MakeSticker(BM_Ubil_Sticker, allocator, 2);
    //BM_Popal_Sticker_Rotating = *BitmapData::FromHandle(leto_api_v1->Bitmap->CopyBitmap(BitmapData::ToHandle(&BM_Popal_Sticker), allocator));
    //BM_Popal_Sticker_Rotating.CopyFrom(allocator, BM_Popal_Sticker);
}