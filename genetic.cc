#include <iomanip>
#include <iostream>
#include <set>
#include <random>
#include <vector>

#include "song.hh"
#include "genetic.hh"

namespace genetic
{

int penalty_crossing = 50;
int penalty_duplication = 5;

int penalty_semitone = 2;
int penalty_tritone = 10;
int penalty_seventh = 7;
int penalty_over_octave = 7;
int penalty_wrong_note = 50;

int penalty_dissonance_semitone = 30;
int penalty_dissonance_second = 25;
int penalty_dissonance_tritone = 25;
int penalty_parallel = 50;

int penalty_rhythm = 25;
int penalty_fast = 15;

std::set<int> good_notes;
std::vector<int> cf_length;

int evaluate(song& s, sample& ind, int measure, bool verbose = false)
{
	int voices = s.voices.size();
	int beats = 8; // s.voices[0].notes.size();

	// voice crossing and duplicated notes
	int score_crossing = 0;
	int score_duplication = 0;
	for (int i = 1; i < voices; ++i) {
		for (int j = 0; j < beats; ++j) {
			if (ind[i][j] > ind[i - 1][j]) score_crossing += penalty_crossing;
			if (ind[i][j] == ind[i - 1][j]) score_duplication += penalty_duplication;
		}
	}

	// voice leading
	int score_voice_leading = 0;
	int score_wrong_notes = 0;
	for (int i = 0; i < voices; ++i) {
		if (s.cf != i) {
			int j = 0;
			int note = ind[i][0];
			while (j < beats) {
				if (ind[i][j] != note) {
					if (good_notes.find(note % 12) == good_notes.end()) score_wrong_notes += penalty_wrong_note;
					int distance = std::abs(ind[i][j] - note);
					if (distance == 1) score_voice_leading += penalty_semitone;
					else if (distance == 6) score_voice_leading += penalty_tritone;
					else if (distance == 70 || distance == 71) score_voice_leading += penalty_seventh;
					else if (distance > 12) score_voice_leading += penalty_over_octave;
					note = ind[i][j];
				}
				++j;
			}
			if (good_notes.find(note % 12) == good_notes.end()) score_wrong_notes += penalty_wrong_note; // last note
		}
	}

	// penalize dissonances and parallels
	int score_dissonances = 0;
	int score_parallels = 0;
	for (int i = 1; i < voices - 1; ++i) {
		for (int j = i + 1; j < voices; ++j) {
			for (int k = 0; k < beats; ++k) {
				int penalty = 0;
				int distance = std::abs(ind[i][k] - ind[j][k]) % 12;
				if (distance == 1 || distance == 11) penalty += penalty_dissonance_semitone;
				if (distance == 6) penalty += penalty_dissonance_tritone;
				if (( distance == 2 || distance == 10) && (k % 4 < 2)) penalty += penalty_dissonance_second;
				if (k % 8 < 2) penalty *= 4;
				score_dissonances += penalty;
				if (ind[i][k - 1] != ind[i][k] && ind[j][k - 1] != ind[j][k]) {
					int d1 = std::abs(ind[i][k - 1] - ind[j][k - 1]) % 12;
					int d2 = std::abs(ind[i][k] - ind[j][k]) % 12;
					if (d1 == d2) {
						if (d1 != 0 && d1 != 3 && d1 != 4 && d1 != 5 && d1 != 8 && d1 != 9) {
							score_parallels += penalty_parallel;
						}
					} 
				}
			}
		}
	}

	// note length
	int score_rhythm = 0;
	std::vector<std::vector<int>> lengths(voices, std::vector<int>(8, 0));
	for (int i = 0; i < voices; ++i) {
		if (s.cf != i) {
			int note = s.voices[i].notes[0], note_start = 0;
			for (int j = 1; j < beats; ++j) {
				if (note != s.voices[i].notes[j]) {
					int length = j - note_start;
					for (int k = note_start; k < j; ++k) lengths[i][k] = length;
				}
			}

			for (int j = 0; j < 8; ++j) {
				if (
					lengths[i][j] != cf_length[j + measure] &&
					lengths[i][j] * 2 != cf_length[j + measure] &&
					lengths[i][j] != cf_length[j + measure] * 2
					) {
					score_rhythm += penalty_rhythm;
				}
				if (i == s.voices.size() - 1 && lengths[i][j] != cf_length[j + measure]) {
					score_rhythm += penalty_rhythm;
				}
			}
		}
	}

	for (int i = 0; i < 8; ++i) {
		if (cf_length[i + measure] != 1) {
			int eights = 0;
			for (int j = 0; j < voices; ++j) {
				if (lengths[j][i] == 1) ++eights;
			}
			if (eights > 1) score_rhythm += ( eights - 1 ) * penalty_fast;
		}
	}

	int score = score_crossing + score_duplication + score_voice_leading
		+ score_wrong_notes + score_dissonances + score_rhythm;

	if (verbose) {
		std::cerr << std::setw(8) << score_crossing << std::setw(8) 
			<< score_duplication << std::setw(8) << score_voice_leading 
			<< std::setw(8) << score_wrong_notes << std::setw(8) 
			<< score_dissonances << std::setw(8) << score_parallels 
			<< std::setw(8) << score_rhythm << std::setw(8) << score << std::endl;
	}

	return score;
}

population initialize(song& s, std::mt19937& gen, int pop, int start, int voices)
{
	static const int beats = 8;
	population p(pop, sample(voices, melody(beats, 0)));
	for (auto& individual : p) {
		for (int i = 0; i < voices; ++i) {
			if (i == s.cf) {
				for (int j = 0; j < 8; ++j) {
					individual[i][j] = s.voices[i].notes[j + start];
				}
			} else {
				int range = ( s.voices[i].max_note - s.voices[i].min_note ) / 2;
				std::normal_distribution<> d(range + s.voices[i].min_note, range / 2);
				for (int j = 0; j < beats; ++j) {
					// stabilize bass
					if ((j % 2) && i == s.voices.size() - 1) individual[i][j] = individual[i][j - 1];
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

void evaluation(song& s, population& p, int measure, score& scores, int& best, int& avg, int& worst, int& best_id)
{
	unsigned count = p.size();
	worst = std::numeric_limits<int>::min();
	best = std::numeric_limits<int>::max();
	avg = 0;

	for (unsigned i = 0; i < count; ++i) {
		int score = evaluate(s, p.at(i), measure);
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
		int cut = std::uniform_int_distribution<>(1, 3)( gen );
		cut *= 2;

		for (int i = cut; i < 8; ++i) {
			for (int j = 0; j < voices; ++j) {
				std::swap(( first[j] )[i], ( second[j] )[i]);
			}
		}
	}
}

sample mutation(song& s, sample individual, double probability, std::mt19937 gen, int voices, int beats)
{
	beats = 8;
	std::uniform_real_distribution<> distribution(0, 1);
	for (int i = 0; i < voices; ++i) {
		if (s.cf != i) {
			int range = ( s.voices[i].max_note - s.voices[i].min_note ) / 2;
			std::normal_distribution<> d(range + s.voices[i].min_note, range / 2);
			for (int j = 0; j < beats; ++j) {
				if (distribution(gen) < probability) {
					if ((j % 2) && ( i == s.voices.size() - 1) || (distribution(gen) < 0.5)) {
						individual[i][j] = individual[i][j - 1];
					} else {
						individual[i][j] = d(gen);
					}
				}
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
	int beats = s.voices[s.cf].notes.size();

	int key_shift = 72 + s.scale * 7;
	good_notes.emplace((key_shift + 0) % 12);
	good_notes.emplace((key_shift + 2) % 12);
	good_notes.emplace((key_shift + 4) % 12);
	good_notes.emplace((key_shift + 5) % 12);
	good_notes.emplace((key_shift + 7) % 12);
	good_notes.emplace((key_shift + 9) % 12);
	good_notes.emplace((key_shift + 11) % 12);
	
	std::cerr << "   cross     dup    lead   wrong    diss     par  rhythm   total" << std::endl;

	for (int measure = 0; measure < beats; measure += 8) {

		cf_length = std::vector<int>(8, 0);
		int note = s.voices[s.cf].notes[0 + measure], note_start = 0;
		for (int i = 1; i < 8; ++i) {
			if (note != s.voices[s.cf].notes[i + measure]) {
				int length = i - note_start;
				for (int j = note_start; j < i; ++j) cf_length[j] = length;
			}
		}

		population p = initialize(s, gen, pop, measure, voices);
		score scores(pop, 0);
		int best, avg, worst, best_id;
		evaluation(s, p, measure, scores, best, avg, worst, best_id);

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
			evaluation(s, p, measure, scores, best, avg, worst, best_id);
			log << ( epoch + 1 + ((measure / 8) * epochs) ) << "," << best << "," << avg << "," << worst << std::endl;
			//if (!( epoch % 100 ))std::cerr << epoch << "/" << epochs << std::endl;
		}

		evaluate(s, p[best_id], measure, true);

		for (int i = 0; i < voices; ++i) {
			if (s.cf != i) {
				for (int j = 0; j < 8; ++j) {
					s.voices[i].notes[j + measure] = p[best_id][i][j];
				}
			}
		}
	}
}

}
