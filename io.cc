#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "song.hh"

#include "io.hh"

namespace io
{

const std::vector<std::string> notenames_sharp{ "c", "cis", "d", "dis", "e", "f", "fis", "g", "gis", "a", "ais", "b" };
const std::vector<std::string> notenames_flat{ "c", "des", "d", "es", "e", "f", "ges", "g", "as", "a", "bes", "b" };

std::string name_from_note(int note, scaletype scale)
{
	std::ostringstream out;
	if (note == 0) {
		out << "r";
	} else {
		out << (scale ? notenames_sharp : notenames_flat)[note % 12];
		int octave = note / 12 - 4;
		while (octave < 0) {
			out << ",";
			++octave;
		}
		while (octave-- > 0) {
			out << "'";
		}
	}
	return out.str();
}

void save_note(std::ostream& out, int note, int beats, scaletype scale)
{
	static const std::unordered_map<int, std::string> lengths{ {8, "1"}, {6, "2."}, {4, "2"}, {3, "4."}, {2, "4"}, {1, "8"} };
	std::string note_name(name_from_note(note, scale));
	auto print_note = [&beats, &out, &note_name](int length, std::string length_name) {
		while (beats >= length) {
			out << note_name << length_name;
			if (beats -= length) out << "~";
		}
	};
	for (auto& p : lengths) print_note(p.first, p.second);
	out << " ";
}

void save_melody(std::ostream& out, melody& notes, scaletype scale)
{
	int last_pitch = notes[0], duration = 0;
	for (int pitch : notes) {
		if (pitch == last_pitch) {
			++duration;
		} else {
			save_note(out, last_pitch, duration, scale);
			duration = 1;
			last_pitch = pitch;
		}	
	}
	save_note(out, last_pitch, duration, scale);
}

void save_voice(std::ostream& out, voice& voice, scaletype scale)
{
	out << voice.name << "Notes = {\n";
	save_melody(out, voice.notes, scale);
	out << "\\bar \"|.\"\n"
		<< "\n}\n\n";
}

void save_voices(std::ostream& out, song& song)
{
	for (voice& voice : song.voices) save_voice(out, voice, song.scale >= 0);
}

void save_global(std::ostream& out, song& song)
{
	out << "global = {\n"
		<< "\\time 4/4\n"
		<< "\\tempo 4 = 120\n"
		<< "}\n\n";
}

void save_scores(std::ostream& out, song& song)
{
	if (song.voices.size() == 4) {
		save_four_part(out, song);
	} else {
		save_generic(out, song);
	}
}

void save_four_part(std::ostream& out, song& song)
{
	out << "\\score { <<\n"
		<< "  \\new PianoStaff <<\n"
		<< "    \\new Staff <<\n"
		<< "      \\set Staff.printPartCombineTexts = ##f\n"
		<< "      \\partcombine\n"
		<< "      << \\global \\" << song.voices[0].name << "Notes >>\n"
		<< "      << \\global \\" << song.voices[1].name << "Notes >>\n"
		<< "    >>\n"
		<< "    \\new Staff <<\n"
		<< "      \\set Staff.printPartCombineTexts = ##f\n"
		<< "      \\clef bass\n"
		<< "      \\partcombine\n"
		<< "      << \\global \\" << song.voices[2].name << "Notes >>\n"
		<< "      << \\global \\" << song.voices[3].name << "Notes >>\n"
		<< "    >>\n"
		<< "  >>\n"
		<< ">>\n"
		<< "\\layout{}\n"
		<< "\\midi{}\n"
		<< "}\n\n";
}

void save_generic(std::ostream& out, song& song)
{
	out << "\\score { <<\n";
	for (auto& voice : song.voices) {
		out << "\\new Staff <<\n";
		if (voice.max_note < 67) out << "\\clef bass\n";
		out << "\\global\n"
			<< "\\" << voice.name << "Notes\n"
			<< ">>\n";
	}
	out << ">>\n"
		<< "\\layout{}\n"
		<< "\\midi{}\n"
        << "}\n\n";
}

void save(std::ostream& out, song& song)
{
	save_global(out, song);
	save_voices(out, song);
	save_scores(out, song);
}

}
