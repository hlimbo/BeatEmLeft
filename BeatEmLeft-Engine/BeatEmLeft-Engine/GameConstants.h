#ifndef GAME_CONSTANTS_H
#define GAME_CONSTANTS_H

namespace Contants
{
	//analog stick input is read from the physical controller as a 16 bit int which ranges from -32767 to 32767
	//25% analog stick outer deadzone
	static const int INNER_DEADZONE = 8191;
	//90% analog stick outer deadzone
	static const int OUTER_DEADZONE = 29490;
};

#endif
