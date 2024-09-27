# CMakeList.txt : CMake project for kitelang, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (kitelang
	"kitelang.cpp"
	"kitelang.h"
	"lexer/token.h"
	"lexer/lexer.h"
	"lexer/lexer.cpp"
	"parser/node.h"
	"parser/parser.h"
	"parser/parser.cpp"
	"compiler/compiler.h"
	"compiler/compiler.cpp"
	"common.h"
	
 "common.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET kitelang PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
