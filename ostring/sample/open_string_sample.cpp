// ostring.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <iostream>
#include <olog/olog.h>
#include "ostring/helpers.h"
#include "ostring/osv.h"
#include "ostring/osf.h"
#include <spdlog/spdlog.h>

int main()
{
	using namespace ostr::literal;
	using namespace std;

	olog::init_log_system();

	olog::verbose("this is a verbose message.");
	olog::debug(OSTR("this is a debug message."));
	olog::info(OSTR("this is an {}.").format(OSTR("info")));
	olog::warn("this is a warning.");
	LOG_FFL olog::error(OSTR("this is an error message."));
	olog::fatal("this is an {} message.", "fatal");

	olog::fatal(u"♂"o);

	return EXIT_SUCCESS;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code filesu, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
