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
        public_key decode_pubkey(std::string pk_str);
        asset get_ram_price();

        //Structs for newaccount action
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

        //Structs for RAM table/action
        struct rammarket { //Inspired by https://github.com/GetScatter/CreateBridge/
            asset supply;
            struct connector {
                asset balance;
                double weight = .5;
            };
            connector base;
            connector quote;
            uint64_t primary_key()const {return supply.symbol.raw();}
        };
        symbol EOS_sym = symbol("EOS", 4);
};
