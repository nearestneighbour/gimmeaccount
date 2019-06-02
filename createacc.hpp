#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>

using namespace eosio;

class [[eosio::contract("createacc")]] createacc : public contract {
    public:
        createacc(name receiver, name code, datastream<const char*> ds):
            contract(receiver, code, ds) {}

        [[eosio::action]]
        void helloworld(std::string bob);

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

        void decode_pubkey(std::string data, const signature sig, const public_key pk) {
            const checksum256 hash = sha256(&data[0], data.size());
            assert_recover_key(hash, sig, pk);
        }
};
