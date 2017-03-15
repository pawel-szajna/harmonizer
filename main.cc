#include <fstream>
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
				{ 67,67,67,67,64,64,65,65,67,67,67,67,67,67,67,67,69,69,69,69,71,71,69,69,67,67,67,67,64,64,64,64,62,62,62,62,64,64,66,66,67,67,67,67,71,71,71,71,69,69,71,71,72,72,66,66,67,67,67,67,67,67,67,67},
				"soprano", 59, 79
			},
			voice{melody(48,0), "alto", 55, 72},
			voice{melody(48,0), "tenor", 50, 66},
			voice{melody(48,0), "bass", 41, 60}
		},
		0, 0
	};
	
	std::ofstream logfile;
	logfile.open("log.csv", std::ofstream::out);
	genetic::optimize(pto, 100, 100, 0.1, 0.01, 5, logfile);
	logfile.close();
	io::save(std::cout, pto);
}
