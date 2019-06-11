#include "gimmeaccount.hpp"

using namespace eosio;

[[eosio::action]]
void gimmeaccount::testdecoding(std::string pk_str, public_key pk_obj) {
    public_key pubkey = decode_pubkey(pk_str);
    if (pubkey == pk_obj)
        print_f("Public keys are the same.");
    else
        print_f("Public keys are not the same (or decoding doesn't work).");
}

[[eosio::on_notify("eosio.token::tranhttps://eosio.stackexchange.comsfer")]]
void gimmeaccount::on_transfer(name from, name to, asset quantity, std::string memo) {
    // Only act on incoming transactions, ignore outgoing
    if (to != get_self()) {
        return;
    }

    // Assert that token contract == eosio.token and that quantity > xxx
    check(get_first_receiver()=="eosio.token"_n, "Only EOS transfers allowed");
    check(quantity.amount>=2, "Deposit too smal");

    // Assert that memo is right
    size_t sep = memo.find(',');
    name accname = name(memo.substr(0,sep));
    std::string pk_str = memo.substr(sep+1);

    // Convert pkstr string to pubkey
    public_key pk_obj = decode_pubkey(pk_str);

    // Create account
    authority auth = authority{
        .threshold = 1,
        .keys = {key_weight{pubkey,1}},
        .accounts = {},
        .waits = {}
    };
    action(
        permission_level(get_self(), "active"_n),
        "eosio"_n,
        "newaccount"_n,
        accdata{get_self(), accname, auth, auth}
    ).send();
    //buyram, delegatebw
}
