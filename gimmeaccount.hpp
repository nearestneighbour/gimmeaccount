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
        void testdecode(std::string pk);

        [[eosio::action]]
        void testpk(public_key pk);

        [[eosio::on_notify("eosio.token::transfer")]]
        void on_transfer(name from, name to, asset quantity, std::string memo);

    private:
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
            key_weight keys;
            perm_weight accounts;
            wait_weight waits;
        };
        struct accdata { // Newaccount parameters
            name creator;
            name accname;
            authority owner;
            authority active;
        };

        //I have no idea what I'm doing here
        //http://knowledge.cryptokylin.io/topics/115
        public_key decode_pubkey(std::string datastr) {
            if (datastr.length() == 53) {
                datastr = datastr.substr(3);
            }
            check(datastr.length() == 50, "Public key str wrong length");
            std::array<unsigned char, 37> r = DecodeBase58(datastr);
            std::array<char, 33> pubkey_data;
            std::copy_n(r.begin(), 33, pubkey_data.begin());
            checksum160 pkcheck = ripemd160(reinterpret_cast<char*>(pubkey_data.data()), 33);
            check(std::memcmp(&pkcheck, &r.end()[-4], 4) == 0, "Checksum mismatch");
            public_key pk{0, pubkey_data};
            return pk;
        }
};
