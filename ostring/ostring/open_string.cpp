// ostring.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include "string.h"
#include "helpers.h"
#include <chrono>
#include "converter.h"

#define ENUM_COMBINATION STR()

int main()
{
	using namespace ostr;

	const wchar_t* str1 = L"我😘😘ni";
	const wchar_t* str2 = L"😘N";

	size_t a = ostr::helper::string::string_search_hash(str1, 7, str2, 3);

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
