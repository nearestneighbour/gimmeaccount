#include "gimmeaccount.hpp"

using namespace eosio;

[[eosio::action]]
void gimmeaccount::testdecode(std::string pk) {
    public_key pubkey = decode_pubkey(pk);
    print_f(pubkey.data.begin());
}

[[eosio::action]]
void gimmeaccount::testpk(public_key pk) {
    print_f(pk.data.begin());
}

[[eosio::on_notify("eosio.token::transfer")]]
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
    std::string pubkeystr = memo.substr(sep+1);
    // Convert pkstr string to pubkey
    public_key pubkey = decode_pubkey(pubkeystr);
    // Create account
    authority auth = authority{
        1,
        key_weight{pubkey,1},
        perm_weight{},
        wait_weight{}
    };
    action(
        permission_level(get_self(), "active"_n),
        "eosio"_n,
        "newaccount"_n,
        accdata{get_self(), accname, auth, auth}
    ).send();
}
