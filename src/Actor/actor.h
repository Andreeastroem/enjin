class Actor
{
public:
    Actor();
    ~Actor();

    void update();
    void render();

private:
    int health;
    int damage;
};