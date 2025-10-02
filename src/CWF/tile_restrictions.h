#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

class TileRestrictions
{
public:
    json getRestrictions() const
    {
        return restrictions;
    }
    void readRestrictions(std::string fileName)
    {
        std::ifstream f(fileName);
        restrictions = json::parse(f);
    }

private:
    json restrictions;
};