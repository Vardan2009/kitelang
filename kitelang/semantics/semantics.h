#pragma once
#include <map>
#include "../common.h"
#include "../parser/node.h"

namespace semantics {
	bool compatible(ktypes::ktype_t, ktypes::ktype_t);
	ktypes::ktype_t would_return(std::shared_ptr<parser::Node>, std::map <std::string, ktypes::ktype_t>, std::map <std::string, ktypes::kfndec_t>);
	ktypes::ktype_t addrof_would_return(std::shared_ptr<parser::AddrOfNode>, std::map <std::string, ktypes::ktype_t>);
	ktypes::ktype_t call_would_return(std::shared_ptr<parser::CallNode>, std::map <std::string, ktypes::kfndec_t>);
	ktypes::ktype_t idx_would_return(std::shared_ptr<parser::IndexNode>, std::map <std::string, ktypes::ktype_t>);
	ktypes::ktype_t var_would_return(std::shared_ptr<parser::VarNode>, std::map <std::string, ktypes::ktype_t>);
}