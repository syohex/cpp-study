#pragma once

#include <vector>
#include <string>
#include <cstdint>

struct Rank {
    int rank;
    std::string id;
    std::int64_t average;
};

enum class RankingError {
    kOk,
    kLogFileError,
};

RankingError CalculateRanking(const std::string& log_file, int count, std::vector<Rank>& ranking);