#include "common.h"

std::map<std::string, ktypes::ktype_t> ktypes::nktype_t {
	{"void", VOID},
	{"char", CHAR},
	{"byte", BYTE},
	{"bool", BOOL},
	{"int32", INT32},
	{"int64", INT64},
	{"ptr", PTR},
};

std::map<ktypes::ktype_t, int> ktypes::bsktype_t{
	{VOID, 0},
	{CHAR, 1},
	{BYTE, 1},
	{BOOL, 1},
	{INT32, 4},
	{INT64, 8},
	{PTR, 8},
};

ktypes::ktype_t ktypes::from_string(std::string nm) {
	if (nktype_t.contains(nm)) return nktype_t[nm];
	else throw std::runtime_error("Unknown type " + nm);
}

int ktypes::size(ktypes::ktype_t t) {
	return bsktype_t[t];
}