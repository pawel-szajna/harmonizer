#include <iostream>
#include <vector>

#include "song.hh"
#include "io.hh"

int main()
{
	melody m{48, 48, 50, 50, 52, 52, 48, 48, 48, 48, 50, 50, 52, 52, 48, 48};
	io::save_melody(std::cout, m, true);
	std::getchar();
}
