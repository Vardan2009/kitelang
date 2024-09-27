#include "common.h"

std::map<std::string, ktypes::ktype_t> ktypes::nktype_t {
	{"void", VOID},
	{"char", CHAR},
	{"byte", BYTE},
	{"bool", BOOL},
	{"int16", INT16},
	{"int32", INT32},
	{"int64", INT64},
	{"ptr8", PTR8},
	{"ptr16", PTR16},
	{"ptr32", PTR32},
	{"ptr64", PTR64},
};

std::map<ktypes::ktype_t, int> ktypes::bsktype_t{
	{VOID, 0},
	{CHAR, 1},
	{BYTE, 1},
	{BOOL, 1},
	{INT16, 2},
	{INT32, 4},
	{INT64, 8},
	{PTR8, 8},
	{PTR16, 8},
	{PTR32, 8},
	{PTR64, 8},
};

ktypes::ktype_t ktypes::from_string(std::string nm) {
	if (nktype_t.contains(nm)) return nktype_t[nm];
	else throw std::runtime_error("Unknown type " + nm);
}

int ktypes::size(ktypes::ktype_t t) {
	return bsktype_t[t];
}