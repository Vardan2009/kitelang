#include "semantics.h"

ktypes::ktype_t semantics::would_return(std::shared_ptr<parser::Node> node, std::map <std::string, ktypes::ktype_t> vartypes, std::map <std::string, ktypes::kfndec_t> fns) {
	switch (node->type) {
	case parser::CALL:   return call_would_return(std::static_pointer_cast<parser::CallNode>(node), fns);
	case parser::ADDROF: return addrof_would_return(std::static_pointer_cast<parser::AddrOfNode>(node), vartypes);
	case parser::DEREF:  return ktypes::ANY;
	case parser::BINOP: return ktypes::ANY;
	case parser::CHAR_LIT: return ktypes::CHAR;
	case parser::STRING_LIT: return ktypes::PTR8;
	case parser::IDX: return idx_would_return(std::static_pointer_cast<parser::IndexNode>(node), vartypes);
	case parser::INT_LIT: return ktypes::INT64;
	case parser::REG: return ktypes::ANY;
	case parser::VAR: return var_would_return(std::static_pointer_cast<parser::VarNode>(node), vartypes);
	}
}

ktypes::ktype_t semantics::addrof_would_return(std::shared_ptr<parser::AddrOfNode> node, std::map <std::string, ktypes::ktype_t> vartypes) {
	switch (vartypes[node->name])
	{
	case ktypes::CHAR:
	case ktypes::BYTE:
	case ktypes::BOOL:
		return ktypes::PTR8;
	case ktypes::INT16:
		return ktypes::PTR16;
	case ktypes::INT32:
		return ktypes::PTR32;
	case ktypes::INT64:
	case ktypes::PTR8:
	case ktypes::PTR16:
	case ktypes::PTR32:
	case ktypes::PTR64:
	default:
		return ktypes::PTR64;
		break;
	}
	return ktypes::PTR64;
}

ktypes::ktype_t semantics::call_would_return(std::shared_ptr<parser::CallNode> node, std::map <std::string, ktypes::kfndec_t> fns) {
	return fns[node->routine].returns;
}

ktypes::ktype_t semantics::idx_would_return(std::shared_ptr<parser::IndexNode> node, std::map <std::string, ktypes::ktype_t> vartypes) {
	// return vartypes[node->name];
	return ktypes::ANY;
}

ktypes::ktype_t semantics::var_would_return(std::shared_ptr<parser::VarNode> node, std::map <std::string, ktypes::ktype_t> vartypes) {
	return vartypes[node->name];
}

bool semantics::compatible(ktypes::ktype_t a, ktypes::ktype_t b) {
	if ((a == ktypes::INT16 || a == ktypes::INT32 || a == ktypes::INT64 || a == ktypes::CHAR || a == ktypes::BYTE || a == ktypes::BOOL) && (b == ktypes::INT16 || b == ktypes::INT32 || b == ktypes::INT64 || b == ktypes::CHAR || b == ktypes::BYTE || b == ktypes::BOOL))
		return true;
	if (a == ktypes::ANY || b == ktypes::ANY) return true;
	return a == b;
}