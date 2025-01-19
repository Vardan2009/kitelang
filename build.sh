#/bin/sh
cmake .
cd kitelang
make
./kitelang ../source.kite
cd ..