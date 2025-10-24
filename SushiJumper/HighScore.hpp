#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

struct Score {
	std::string name;
	float score;
};

void SaveHighScores(std::vector<Score>& scores, const std::string& filename) {
	
	std::ofstream file(filename);

	if (!file.is_open()) {
		return; // failed to open file
	}

    std::sort(scores.begin(), scores.end(),
        [](const Score& a, const Score& b) {
            return a.score > b.score;
        });

    if (scores.size() > 3) {
        scores.resize(3);
    }

	for (const Score& s : scores) {
		file << s.name << " " << s.score << "\n";
	}

	file.close();
}

std::vector<Score> LoadHighscores(const std::string& filename) {
    std::vector<Score> scores;
    scores.reserve(10);
    std::ifstream file(filename);

    if (!file.is_open()) {
        return scores;
    }

    Score s;
    while (file >> s.name >> s.score) {
        scores.push_back(s);
    }

    file.close();
    return scores;
}
