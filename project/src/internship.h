#pragma once
#include <optional>
#include <string>

#include <nlohmann/json.hpp>
#include <date/date.h>

using json = nlohmann::json;
using sys_days = date::sys_days;

namespace internship {
    // do not remove this function
    void solution(const std::string& jsonFileName, int elementsCount);

    struct OSEntry;

    std::optional<int> calculateSupportPeriod(const json &versionJson);
    std::optional<OSEntry> parseOSEntry(const std::string name, const json& versionJson);
    std::vector<OSEntry> parseOSEntries(const json& productsJson);
    std::vector<OSEntry> readAllOSEntries(const std::string& jsonFileName);
    std::vector<OSEntry> getSolutionEntries(std::vector<OSEntry> allOSEntries, int elementsCount);

    // OSEntry struct represents an item in the list of entries that will be
    // displayed in the result
    struct OSEntry {
        std::string name;
        std::string cycle;
        int supportPeriod;
    };

    namespace util {
        std::optional<sys_days> toSysDays(const std::string& date);
    }
}