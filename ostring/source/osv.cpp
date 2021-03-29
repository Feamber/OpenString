#include "osv.h"

_NS_OSTR_BEGIN

int string_view::compare(const string_view& rhs) const
{
	return _str.compare(rhs._str);
}

_NS_OSTR_END
