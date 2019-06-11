#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <array>
#include "base58.hpp"

using namespace eosio;

class [[eosio::contract("gimmeaccount")]] gimmeaccount : public contract {
    public:
        gimmeaccount(name receiver, name code, datastream<const char*> ds):
            contract(receiver, code, ds) {}

        [[eosio::action]]
        void testdecoding(std::string pkstr, public_key pkobj);

        [[eosio::on_notify("eosio.token::transfer")]]
        void on_transfer(name from, name to, asset quantity, std::string memo);

    private:
        //Inspired by http://knowledge.cryptokylin.io/topics/115
        public_key decode_pubkey(std::string pk_str) {
            //Remove EOS prefix and check pubkey length
            pk_str = pk_str.substr(3);
            check(pk_str.length() == 50, "Public key str wrong length");

            //Decode pubkey
            const std::array<unsigned char, 37> decoded = DecodeBase58(pk_str);
            std::array<char, 33> pk_data;
            std::copy_n(decoded.begin(), 33, pk_data.begin());

            //Evaluate checksum
            checksum160 pk_cs = ripemd160(reinterpret_cast<char*>(pk_data.data()), 33);
            std::array<uint8_t, 20> cs_data = pk_cs.extract_as_byte_array();
            check(std::memcmp(cs_data.begin(), &decoded.end()[-4], 4) == 0, "Public key checksum mismatch");

            return public_key{0, pk_data};
        }

        struct key_weight {
            public_key key;
            uint16_t weight;
        };
        struct perm_weight {
            permission_level permission;
            uint16_t weight;
        };
        struct wait_weight {
            uint32_t wait_sec;
            uint16_t weight;
        };
        struct authority {
            uint32_t threshold;
            std::vector<key_weight> keys;
            std::vector<perm_weight> accounts;
            std::vector<wait_weight> waits;
        };
        struct newaccount { // Newaccount parameters
            name creator;
            name accname;
            authority owner;
            authority active;
        };
};
