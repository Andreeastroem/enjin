#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../../../Domain/Repositories/IGridRepository.h"
#include "../../../Domain/Entities/Pattern.h"

namespace cwf::infrastructure
{
    class FileGridRepository : public domain::IGridRepository
    {
    public:
        FileGridRepository(const std::string &basePath, const domain::Pattern &pattern);
        FileGridRepository(const std::string &basePath);

        void setPattern(const domain::Pattern &pattern);

        std::unique_ptr<domain::Grid> loadGrid(const std::string &identifier) override;
        void saveGrid(const domain::Grid &grid, const std::string &identifier) override;
        std::vector<std::string> listSavedGrids() override;
        bool gridExists(const std::string &identifier) override;
        void deleteGrid(const std::string &identifier) override;

    private:
        std::string getFullPath(const std::string &identifier) const;

        std::string base_path_;
        domain::Pattern pattern_;
    };
} // namespace cwf::infrastructure