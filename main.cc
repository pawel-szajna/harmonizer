#include <fstream>
#include <iostream>
#include <vector>

#include "song.hh"
#include "genetic.hh"
#include "io.hh"

std::vector<int> transpose(std::vector<int> v, int d)
{
	for (auto& i : v) i += d;
	return v;
}

int main()
{
	song pto{
		{
			voice{
		transpose(
	//{ 67,67,67,67,64,64,65,65,67,67,67,67,67,67,67,67,69,69,69,69,71,71,69,69,67,67,67,67,64,64,64,64,62,62,62,62,64,64,66,66,67,67,67,67,71,71,71,71,69,69,71,71,72,72,66,66,67,67,67,67,67,67,67,67 },
	{
		71, 71, 71, 71, 72, 72, 74, 74,
		74, 74, 72, 72, 71, 71, 69, 69,
		67, 67, 67, 67, 69, 69, 71, 71,
		71, 71, 71, 69, 69, 69, 69, 69,
		71, 71, 71, 71, 72, 72, 74, 74,
		74, 74, 72, 72, 71, 71, 69, 69,
		67, 67, 67, 67, 69, 69, 71, 71,
		69, 69, 69, 67, 67, 67, 67, 67
}
		,-2),
		"soprano", 59, 79
	},
			//voice{ melody(64,0), "ss", 59, 79 },
			//voice{melody(64,0), "ms", 55, 72 },
			voice{melody(64,0), "alto", 55, 72},
			voice{melody(64,0), "tenor", 50, 66},
			voice{melody(64,0), "bass", 41, 60}
							
			
		},
		0, //cf
		-1  //scale
	};
	
	std::ofstream logfile;
	logfile.open("log.csv", std::ofstream::out);
	genetic::optimize(pto, 200, 200, 0.1, 0.015, 7, logfile);
	logfile.close();
	io::save(std::cout, pto);
}
