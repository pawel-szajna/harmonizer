#include <set>
#include <random>
#include <vector>

#include "song.hh"
#include "genetic.hh"

namespace genetic
{

population initialize(song& s, std::mt19937& gen, int pop, int beats, int voices)
{
	population p(pop, sample(voices, melody(beats, 0)));
	for (auto& individual : p) {
		for (int i = 0; i < voices; ++i) {
			if (i == s.cf) {
				individual[i] = s.voices[i].notes;
			} else {
				int range = ( s.voices[i].max_note - s.voices[i].min_note ) / 2;
				std::normal_distribution<> d(range + s.voices[i].min_note, range / 2);
				for (int j = 0; j < beats; ++j) {
					individual[i][j] = d(gen);
				}
			}
		}
	}
	return p;
}

int tournament_selector(score scores, std::mt19937& gen, int ind_count)
{
	int length = scores.size();
	int best = -1, best_score = std::numeric_limits<int>::max();
	std::set<int> individuals;
	std::uniform_int_distribution<> distribution(0, length - 1);

	while (individuals.size() < ind_count) {
		individuals.emplace(distribution(gen));
	}

	for (auto& i : individuals) {
		if (scores.at(i) < best_score) {
			best_score = scores.at(i);
			best = i;
		}
	}

	return best;
}

int evaluate(song& s, sample& ind)
{
	return 0;
}

void evaluation(song& s, population& p, score& scores, int& best, int& avg, int& worst, int& best_id)
{
	unsigned count = p.size();
	worst = std::numeric_limits<int>::min();
	best = std::numeric_limits<int>::max();
	avg = 0;

	for (unsigned i = 0; i < count; ++i) {
		int score = evaluate(s, p.at(i));
		scores[i] = score;
		avg += score;
		if (worst < score) {
			worst = score;
		}
		if (best > score) {
			best = score;
			best_id = i;
		}
	}

	avg /= count;
}

void crossover(sample& first, sample& second, double probability, std::mt19937 gen, int voices, int beats)
{
	if (std::uniform_real_distribution<>(0, 1)( gen ) < probability) {
		int cut = std::uniform_int_distribution<>(1, beats - 2)( gen );

		for (int i = cut; i < beats; ++i) {
			for (int j = 0; j < voices; ++j) {
				std::swap(( first[j] )[i], ( second[j] )[i]);
			}
		}
	}
}

sample mutation(song& s, sample individual, double probability, std::mt19937 gen, int voices, int beats)
{
	std::uniform_real_distribution<> distribution(0, 1);
	for (int i = 0; i < voices; ++i) {
		if (s.cf != i) {
			int range = ( s.voices[i].max_note - s.voices[i].min_note ) / 2;
			std::normal_distribution<> d(range + s.voices[i].min_note, range / 2);
			for (int j = 0; j < beats; ++j) {
				if(distribution(gen) < probability) individual[i][j] = d(gen);
			}
		}
	}
	return individual;
}

void optimize(song& s, int pop, int epochs, double cross_prob, double mutate_prob, int sel_param, std::ostream& log)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	if (pop % 2) ++pop;

	int voices = s.voices.size();
	int beats = s.voices[0].notes.size();

	population p = initialize(s, gen, pop, beats, voices);
	score scores(pop, 0);
	int best, avg, worst, best_id;
	evaluation(s, p, scores, best, avg, worst, best_id);

	for (int epoch = 0; epoch < epochs; ++epoch) {
		population new_population;
		while (new_population.size() < pop) {
			sample child1 = p.at(tournament_selector(scores, gen, sel_param));
			sample child2 = p.at(tournament_selector(scores, gen, sel_param));
			crossover(child1, child2, cross_prob, gen, voices, beats);
			new_population.push_back(mutation(s, child1, mutate_prob, gen, voices, beats));
			new_population.push_back(mutation(s, child2, mutate_prob, gen, voices, beats));
		}
		p = new_population;
		evaluation(s, p, scores, best, avg, worst, best_id);
		log << ( epoch + 1 ) << "," << best << "," << avg << "," << worst << std::endl;
	}

	for (int i = 0; i < voices; ++i) {
		s.voices[i].notes = p[best_id][i];
	}
}

}
