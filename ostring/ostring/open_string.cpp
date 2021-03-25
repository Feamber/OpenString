// ostring.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include "string.h"
#include "helpers.h"
#include <chrono>
#include <vector>
#include "../fmt/include/fmt/ranges.h"
#include "string_view.h"
#include "static_functions.h"


template<class T>
struct is_c_str : std::integral_constant
	<
	bool,
	std::is_same<char const*, typename std::decay<T>::type>::value ||
	std::is_same<char*, typename std::decay<T>::type>::value
	>
{};

int main()
{
	using namespace ostr;

	// constexpr std::string_view ss = "123321";
	// constexpr ostr::string_view a = "123321";
	// constexpr size_t s = "123321"_sv.length();

	string a = "123{}";
	string b = a.format(456);

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
