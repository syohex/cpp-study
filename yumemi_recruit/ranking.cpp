#include "ranking.h"

#include <fstream>
#include <sstream>
#include <map>
#include <queue>
#include <cmath>

namespace {

struct ScoreData {
    std::string id;
    std::int64_t score;
};

struct CompareScore {
  public:
    bool operator()(const ScoreData &a, const ScoreData &b) const {
        return a.score < b.score;
    }
};

using RankingQueue = std::priority_queue<ScoreData, std::vector<ScoreData>, CompareScore>;

RankingQueue ParseLogFile(std::ifstream &file) {
    std::string line;
    std::string date_field, id_field, score_field;

    (void)std::getline(file, line); // read header

    struct InnerData {
        int count = 0;
        std::int64_t total = 0;
    };

    std::map<std::string, InnerData> m;
    while (std::getline(file, line)) {
        std::stringstream ss(line);

        std::getline(ss, date_field, ',');
        std::getline(ss, id_field, ',');
        std::getline(ss, score_field, ',');

        m[id_field].count += 1;
        m[id_field].total += std::stoll(score_field);
    }

    RankingQueue ret;
    for (const auto &it : m) {
        const auto &id = it.first;
        const auto &score = it.second;
        double average = static_cast<double>(score.total) / score.count;
        ret.push(ScoreData{id, static_cast<std::int64_t>(std::round(average))});
    }

    return ret;
}

} // namespace

RankingError CalculateRanking(const std::string &log_file, int count, std::vector<Rank> &ranking) {
    std::ifstream file(log_file);
    if (!file) {
        return RankingError::kLogFileError;
    }

    auto ranking_queue = ParseLogFile(file);

    int rank = 0;
    int tied_ranks = 1;
    std::int64_t prev_score = -1;
    while (!ranking_queue.empty()) {
        auto r = ranking_queue.top();
        ranking_queue.pop();

        if (r.score == prev_score) {
            ranking.push_back(Rank{rank, r.id, r.score});
            ++tied_ranks;
        } else {
            rank += tied_ranks;
            if (rank > count) {
                break;
            }

            tied_ranks = 1;
            ranking.push_back(Rank{rank, r.id, r.score});
        }

        prev_score = r.score;
    }

    return RankingError::kOk;
}