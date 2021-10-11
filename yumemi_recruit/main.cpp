#include <cstdio>
#include <cinttypes>

#include "ranking.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: ranking_cli csv_file\n");
        return 1;
    }

    std::vector<Rank> ranking;
    auto err = CalculateRanking(argv[1], 10, ranking);
    if (err != RankingError::kOk) {
        fprintf(stderr, "failed to calculate ranking\n");
        return 1;
    }

    printf("rank,player_id,mean_score\n");
    for (const auto &rank : ranking) {
        printf("%d,%s,%" PRId64 "\n", rank.rank, rank.id.c_str(), rank.average);
    }

    return 0;
}