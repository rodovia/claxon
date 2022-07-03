#include "Startup.h"
#include <InfoParser.h>

void engine::ParseGameInfo()
{
	gamepaths::CInfoParser::FromFile("gameinfo.txt");
}
