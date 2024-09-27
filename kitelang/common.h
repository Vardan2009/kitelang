#pragma once
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace ktypes {
	typedef enum {
		VOID,
		CHAR,
		BYTE,
		BOOL,
		INT16,
		INT32,
		INT64,
		PTR8,
		PTR16,
		PTR32,
		PTR64,
	} ktype_t;

	typedef struct {
		std::string name;
		ktype_t type;
	} kval_t;

	typedef struct {
		std::string name;
		std::vector<ktype_t> argtps;
		ktype_t returns;
	} kfndec_t;

	extern std::map<std::string, ktype_t> nktype_t;
	extern std::map<ktype_t, int> bsktype_t;
	extern ktype_t from_string(std::string);
	extern int size(ktype_t);
}