#include <cassert>
#include <vector>

#include "ranking.h"

int main() {
    {
        std::vector<Rank> ranking;
        auto ret = CalculateRanking("./data/all_first.csv", 1, ranking);
        assert(ret == RankingError::kOk);
        assert(ranking.size() == 13);
    }
    {
        std::vector<Rank> ranking;
        auto ret = CalculateRanking("./data/input01.csv", 1, ranking);
        assert(ret == RankingError::kOk);
        assert(ranking.size() == 2);
    }
    {
        std::vector<Rank> ranking;
        auto ret = CalculateRanking("./data/input02.csv", 5, ranking);
        assert(ret == RankingError::kOk);
        assert(ranking.size() == 5);
        assert(ranking[0].id == "player0002");
        assert(ranking[1].id == "player0001");
        assert(ranking[1].average == 30);
        assert(ranking[2].id == "player0003");
        assert(ranking[3].id == "player0004");
        assert(ranking[4].id == "player0005");
        assert(ranking[4].average == 4);
    }
    return 0;
}