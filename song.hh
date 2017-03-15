#ifndef SONG_HH
#define SONG_HH

typedef std::vector<int> melody;
typedef std::vector<voice> harmony;

struct voice
{
	melody notes;
	std::string name;
	int min_note, max_note;
};

struct song
{
	harmony voices;
	int cf, scale;
};

#endif
