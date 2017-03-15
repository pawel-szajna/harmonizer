#ifndef IO_HH
#define IO_HH

namespace io
{

typedef bool scaletype;

void save(std::ostream& out, song& song);

void save_melody(std::ostream& out, melody& notes, scaletype scale);
void save_note(std::ostream& out, int note, int beats, scaletype scale);
void save_global(std::ostream& out, song& song);
void save_scores(std::ostream& out, song& song);
void save_four_part(std::ostream& out, song& song);
void save_generic(std::ostream& out, song& song);

std::string name_from_note(int note, scaletype scale);

}
 
#endif
