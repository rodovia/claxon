#include <cstdio>
#include "ColorTransform.h"
#include <tier0lib/CommandLine.h>

int main(int argc, char* argv[])
{
	std::vector<std::string> ag(argv + 1, argv + argc);

	tier0::CCmdInputParser pars(ag);
	std::optional<std::string> outpf = pars.GetOptionValue("O");
	if (outpf == std::nullopt)
	{
		std::printf("Missing required argument: /O (output file name)\n");
		return 1;
	}

	std::optional<std::string> inpf = pars.GetOptionValue("I");
	if (inpf == std::nullopt)
	{
		std::printf("Missing required argument: /I (input file name)\n");
		return 1;
	}

	RotateAxis180(*inpf, *outpf);

	return 0;
}