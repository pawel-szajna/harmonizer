#ifndef GENETIC_HH
#define GENETIC_HH

namespace genetic
{

typedef std::vector<melody> sample; // an individual is music for all the parts
typedef std::vector<sample> population; // population of individuals
typedef std::vector<int> score;

void optimize(song& s, int pop, int epochs, double cross_prob, double mutate_prob, int sel_param, std::ostream& log);

}

#endif
