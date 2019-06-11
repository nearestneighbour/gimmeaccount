# Because fuck eosio-cpp
eosio-cpp -I=. -contract=gimmeaccount -o=gimme.wasm gimmeaccount.cpp
mv gimme.wasm gimmeaccount.wasm
mv gimme.abi gimmeaccount.abi
