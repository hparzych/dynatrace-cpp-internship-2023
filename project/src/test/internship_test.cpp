#include <gtest/gtest.h>
#include "../internship.h"

TEST(InternshipTest, UtilToSysDaysValidDate) {
  std::string dateStr = "2023-11-04";
  auto sysDays = internship::util::toSysDays(dateStr);
  EXPECT_TRUE(sysDays.has_value());
  EXPECT_EQ(sysDays.value(), date::sys_days{date::year{2023}/date::month{11}/date::day{4}});
}

TEST(InternshipTest, UtilToSysDaysInvalidDate) {
  std::string dateStr = "201$%03-15";
  auto sysDays = internship::util::toSysDays(dateStr);
  EXPECT_FALSE(sysDays.has_value());
}

TEST(InternshipTest, UtilToSysDaysInvalidDateEmpty) {
  std::string dateStr = "";
  auto sysDays = internship::util::toSysDays(dateStr);
  EXPECT_FALSE(sysDays.has_value());
}

TEST(InternshipTest, UtilToSysDaysInvalidDateNoYear) {
  std::string dateStr = "11-22";
  auto sysDays = internship::util::toSysDays(dateStr);
  EXPECT_FALSE(sysDays.has_value());
}


TEST(InternshipTest, CalculateSupportPeriodValidDate) {
  std::string releaseDateStr = "2023-01-01";
  std::string eolDateStr = "2024-01-01";
  nlohmann::json versionJson = {
    {"releaseDate", releaseDateStr},
    {"eol", eolDateStr}
  };
  auto supportPeriod = internship::calculateSupportPeriod(versionJson);
  EXPECT_TRUE(supportPeriod.has_value());
  EXPECT_EQ(supportPeriod.value(), 366); // 366 days including last day
}

TEST(InternshipTest, CalculateSupportPeriodInvalidDate) {
  std::string releaseDateStr = "2024-01-01";
  std::string eolDateStr = "2023-01-01"; // invalid order
  nlohmann::json versionJson = {
    {"releaseDate", releaseDateStr},
    {"eol", eolDateStr}
  };
  auto supportPeriod = internship::calculateSupportPeriod(versionJson);
  EXPECT_FALSE(supportPeriod.has_value());
}

TEST(InternshipTest, ParseOSEntryValidJsonDebian11) {
  std::string osName = "debian";
  std::string cycle = "11";
  std::string releaseDateStr = "2021-08-14";
  std::string eolDateStr = "2024-07-01";
  nlohmann::json versionJson = {
    {"cycle", cycle},
    {"releaseDate", releaseDateStr},
    {"eol", eolDateStr}
  };
  auto osEntry = internship::parseOSEntry(osName, versionJson);
  EXPECT_TRUE(osEntry.has_value());
  EXPECT_EQ(osEntry.value().name, osName);
  EXPECT_EQ(osEntry.value().cycle, cycle);
  EXPECT_EQ(osEntry.value().supportPeriod, 1053);
}

TEST(InternshipTest, ParseOSEntryValidJsonOpenSuse15_4) {
  std::string osName = "opensuse";
  std::string cycle = "15.4";
  std::string releaseDateStr = "2022-06-09";
  std::string eolDateStr = "2023-12-01";
  nlohmann::json versionJson = {
    {"cycle", cycle},
    {"releaseDate", releaseDateStr},
    {"eol", eolDateStr}
  };
  auto osEntry = internship::parseOSEntry(osName, versionJson);
  EXPECT_TRUE(osEntry.has_value());
  EXPECT_EQ(osEntry.value().name, osName);
  EXPECT_EQ(osEntry.value().cycle, cycle);
  EXPECT_EQ(osEntry.value().supportPeriod, 541);
}

TEST(InternshipTest, ParseOSEntryValidJsonUbuntu22_10) {
  std::string osName = "ubuntu";
  std::string cycle = "22.10";
  std::string releaseDateStr = "2022-10-20";
  std::string eolDateStr = "2023-07-20";
  nlohmann::json versionJson = {
    {"cycle", cycle},
    {"releaseDate", releaseDateStr},
    {"eol", eolDateStr}
  };
  auto osEntry = internship::parseOSEntry(osName, versionJson);
  EXPECT_TRUE(osEntry.has_value());
  EXPECT_EQ(osEntry.value().name, osName);
  EXPECT_EQ(osEntry.value().cycle, cycle);
  EXPECT_EQ(osEntry.value().supportPeriod, 274);
}

TEST(InternshipTest, ParseOSEntryInvalidJsonNoDates) {
  std::string osName = "ubuntu";
  nlohmann::json versionJson = {
    {"cycle", "22.10"}
  };
  auto osEntry = internship::parseOSEntry(osName, versionJson);
  EXPECT_FALSE(osEntry.has_value());
}
