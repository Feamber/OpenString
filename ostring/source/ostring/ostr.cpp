
#include <string_view>
#include "ostring/ostr.h"

_NS_OSTR_BEGIN

string::string(const char* src, size_t length)
	:_surrogate_pair_count(0)
{
	std::string_view sv(src, length);

	// ansi is a subset of the first plane
	//_str.insert(_str.cbegin(), src, src + len);
	_str = std::u16string(sv.cbegin(), sv.cend());
}

string string::substring(size_t from, size_t size) const
{
	const size_t uint16_size = std::min(size, length() - from);
	if (_surrogate_pair_count == 0)
	{
		/*ret._str.reserve(uint16_size + 1);
		ret._str.insert(ret._str.cbegin(), _str.cbegin() + from, _str.cbegin() + (from + uint16_size));*/
		return _str.substr(from, size);
	}
	else
	{
		auto from_it = helper::string::codepoint_count_to_iterator(_str.cbegin(), from, _str.cend());

		const size_t substr_surrogate_pair_count = _surrogate_pair_count - ((from_it - _str.cbegin()) - from);
		const size_t real_size = uint16_size + substr_surrogate_pair_count;

		string ret = _str.substr(from_it - _str.cbegin(), real_size);
		ret._surrogate_pair_count = substr_surrogate_pair_count;
		return ret;
	}
}

_NS_OSTR_END
