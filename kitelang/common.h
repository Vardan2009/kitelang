#pragma once
#include <map>
#include <stdexcept>
#include <string>

namespace ktypes {
	typedef enum {
		VOID,
		CHAR,
		BYTE,
		BOOL,
		INT32,
		INT64,
		PTR
	} ktype_t;

	typedef struct {
		std::string name;
		ktype_t type;
	} kval_t;

	extern std::map<std::string, ktype_t> nktype_t;
	extern std::map<ktype_t, int> bsktype_t;
	extern ktype_t from_string(std::string);
	extern int size(ktype_t);
}