#include <raylib.h>

class Selection
{
public:
    Selection(Vector2 startPosition);

    void update(Vector2 currentPosition);
    void render();

private:
    Vector2 position;
    Vector2 size;
    Color color;
};