#include <iostream>
#include <vector>

#include "song.hh"
#include "genetic.hh"
#include "io.hh"

int main()
{
	song pto{
		{
			voice{
				{ 67,67,67,67,64,64,65,65,67,67,67,67,67,67,67,67,69,69,69,69,71,71,69,69,67,67,67,67,64,64,64,64,62,62,62,62,64,64,66,66,67,67,67,67,71,71,71,71},
				"soprano", 59, 79
			},
			voice{melody(48,0), "alto", 55, 72},
			voice{melody(48,0), "tenor", 50, 66},
			voice{melody(48,0), "bass", 41, 60}
		},
		0, 0
	};
	melody m{48, 48, 50, 50, 52, 52, 48, 48, 48, 48, 50, 50, 52, 52, 48, 48};
	io::save(std::cout, pto);
	std::getchar();
}
