#include "BattleshipCursor.hpp"

#include <Data/Point2.hpp>
#include <DrawFunctions/DrawLine.hpp>

using namespace Battleship;

BattleshipCursor::BattleshipCursor(int rows_count, int cols_count, int width)
	: ICursor{ 0, rows_count, 0, cols_count }, width{ width }, scale{ width - 1 }
{
}

static void _DrawCursor(IScreen& screen, Point2_i* points[4])
{
	static const Point2_i x1_y0 = Point2_i{1, 0};
	static const Point2_i x0_y1 = Point2_i{0, 1};

	using namespace DrawFunctions;

	for (size_t i = 0; i < 4; ++i)
		DrawLine(screen, *points[i % 4], *points[(i + 1) % 4], BlackColor);

	for (size_t i = 0; i < 4; ++i)
	{
		screen.PixelSet(*points[i % 4], WhiteColor);
		screen.PixelSet(*points[i % 4] + ((i == 1 || i == 2) 	? -x1_y0 : x1_y0), WhiteColor);
		screen.PixelSet(*points[i % 4] + ((i > 1) 				? -x0_y1 : x0_y1), WhiteColor);
	}

}

void BattleshipCursor::DrawCursor(IScreen& screen, Point2_i offset)
{
	using namespace DrawFunctions;

	Point2_i top_left{}, top_right{}, bottom_left{}, bottom_right{};

	top_left = offset + Point2_i{ cur_col * scale, cur_row * scale };
	top_right = bottom_left = top_left;

	top_right.x += scale;
	bottom_left.y += scale;

	bottom_right = bottom_left;
	bottom_right.x += scale;

	Point2_i* points[4] { &top_left, &top_right, &bottom_right, &bottom_left };

	_DrawCursor(screen, points);
}
