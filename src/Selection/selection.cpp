#include "selection.h"

Selection::Selection(Vector2 startPosition)
{
    position = startPosition;
    size = {0, 0};
    color = WHITE;
}

void Selection::update(Vector2 currentPosition)
{
    size.x = currentPosition.x - position.x;
    size.y = currentPosition.y - position.y;
}

void Selection::render()
{
    DrawRectangleRec({position.x, position.y, size.x, size.y}, color);
}
