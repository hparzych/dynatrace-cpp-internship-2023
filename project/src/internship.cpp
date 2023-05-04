#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <optional>
#include <string>
#include <vector>

#include <date/date.h>
#include <nlohmann/json.hpp>

#include "internship.h"

using json = nlohmann::json;
using namespace date;

namespace internship {

    void solution(const std::string& jsonFileName, int elementsCount) {
        const auto allOSEntries = readAllOSEntries(jsonFileName);
        const auto solutionEntries = getSolutionEntries(std::move(allOSEntries), elementsCount);

        // print the solution
        for (const auto& entry : solutionEntries) {
            std::cout << entry.name << " " << entry.cycle << " " << entry.supportPeriod << '\n';
        }

        std::cout << std::flush;
    }

    // calculateSupportPeriod calculates the support period from the release
    // date and eol date contained in the version json data
    std::optional<int> calculateSupportPeriod(const json &versionJson) {
        if (!versionJson.contains("releaseDate") || !versionJson.at("releaseDate").is_string()) {
            return std::nullopt;
        }

        std::string startDateStr = versionJson.at("releaseDate");
        std::string endDateStr;

        if (versionJson.contains("eol")) {
            auto eolDate = versionJson.at("eol");
            if (eolDate.is_string()) {
                endDateStr = eolDate.get<std::string>();
            }
        }

        // XXX if "eol" is not present, we try using "support" instead
        if (endDateStr.empty() && versionJson.contains("support")) {
            auto supportDate = versionJson.at("support");
            if (supportDate.is_string() && !supportDate.empty()) {
                endDateStr = supportDate.get<std::string>();
            }
        }

        if (endDateStr.empty()) {
            return std::nullopt;
        }

        auto startSysDays = util::toSysDays(startDateStr);
        auto endSysDays = util::toSysDays(endDateStr);

        if (!startSysDays.has_value() || !endSysDays.has_value()) {
            return std::nullopt;
        }

        if (startSysDays.value() > endSysDays.value()) {
            // release date is later than eol date
            return std::nullopt;
        }

        auto daysDifference = endSysDays.value() - startSysDays.value();

        // We add 1 because we want to include the last day in the support period
        return daysDifference.count() + 1;
    }

    // parseOSEntry parses the JSON of a version and returns a valid OSEntry
    // or null if the JSON is invalid
    std::optional<OSEntry> parseOSEntry(const std::string name, const json& versionJson) {
        if (name.empty() || !versionJson.contains("cycle") || !versionJson.at("cycle").is_string()) {
            return std::nullopt;
        }

        std::string cycle = versionJson.at("cycle");
        if (cycle.empty()) {
            return std::nullopt;
        }

        auto supportPeriod = calculateSupportPeriod(versionJson);

        if (!supportPeriod.has_value()) {
            return std::nullopt;
        }

        return OSEntry{name, cycle, supportPeriod.value()};
    }

    // parseOSEntries parses the JSON of products and returns a vector of valid OSEntry structs
    std::vector<OSEntry> parseOSEntries(const json& productsJson) {
        std::vector<OSEntry> osEntries;

        for (const auto& productJson : productsJson) {
            // skip if the product does not contain required json data or is not
            // an operating system
            if (!productJson.contains("versions") ||
                !productJson.contains("name") ||
                !productJson.contains("os") ||
                !productJson.at("versions").is_array() ||
                !productJson.at("name").is_string() ||
                !productJson.at("os").is_boolean() ||
                !productJson.at("os")
            ) {
                continue;
            }
            
            const std::string name = productJson.at("name");
            if (name.empty()) {
                continue;
            }
            
            // iterate over product versions and parse each one into OSEntry struct
            for (const auto& versionJson : productJson.at("versions")) {
                if (auto ose = parseOSEntry(name, versionJson); ose.has_value()) {
                    osEntries.push_back(ose.value());
                }
            }
        }
        return osEntries;
    }

    std::vector<OSEntry> getSolutionEntries(std::vector<OSEntry> allOSEntries, int elementsCount) {
        if (elementsCount > allOSEntries.size()) {
            elementsCount = allOSEntries.size();
        }
        
        if (elementsCount <= 0) {
            return {};
        }

        // sort elementsCount of elements by support period
        std::partial_sort(allOSEntries.begin(), allOSEntries.begin() + elementsCount,
                          allOSEntries.end(),
                          [](const OSEntry& a, const OSEntry& b) {
            return a.supportPeriod > b.supportPeriod;
        });

        return std::vector<OSEntry>(allOSEntries.begin(), allOSEntries.begin() + elementsCount);
    }

    // readAllOSEntries reads the JSON file and returns parsed osEntries
    std::vector<OSEntry> readAllOSEntries(const std::string& jsonFileName) {
        std::vector<OSEntry> osEntries;
        std::ifstream f(jsonFileName);
        json data = json::parse(f);

        return parseOSEntries(data);
    }

    namespace util {
        std::optional<sys_days> toSysDays(const std::string& date) {
            std::istringstream ss(date);
            sys_days result;
            ss >> date::parse("%F", result);
            if (result == sys_days{}) {
                // the date is not valid
                return std::nullopt;
            }
            return result;
        }
    }

}