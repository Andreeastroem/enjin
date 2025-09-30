#pragma once

namespace cwf::domain
{

    class TileId
    {
    public:
        // Default constructor required for containers
        TileId() : value_(0) {}

        // Explicit constructor from int
        explicit TileId(int value) : value_(value) {}

        // Copy constructor
        TileId(const TileId &other) = default;

        // Move constructor
        TileId(TileId &&other) noexcept = default;

        // Copy assignment
        TileId &operator=(const TileId &other) = default;

        // Move assignment
        TileId &operator=(TileId &&other) noexcept = default;

        // Destructor
        ~TileId() = default;

        int value() const { return value_; }

        bool operator==(const TileId &other) const { return value_ == other.value_; }
        bool operator!=(const TileId &other) const { return !(*this == other); }
        bool operator<(const TileId &other) const { return value_ < other.value_; }

    private:
        int value_;
    };

} // namespace cwf::domain

// Add hash support for TileId
namespace std
{
    template <>
    struct hash<cwf::domain::TileId>
    {
        size_t operator()(const cwf::domain::TileId &id) const noexcept
        {
            return std::hash<int>{}(id.value());
        }
    };
}