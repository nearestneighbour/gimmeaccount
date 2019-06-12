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

[[eosio::on_notify("eosio.token::transfer")]]
void gimmeaccount::on_transfer(name from, name to, asset quantity, std::string memo) {
    // Only act on incoming transactions, ignore outgoing
    if (to != get_self()) {
        return;
    }
    // Assert that token contract == eosio.token and that quantity > xxx
    check(get_first_receiver()=="eosio.token"_n, "Only EOS transfers allowed");

    //Check tx details (memo should be [(12 characters),(53 charaters)])
    check(memo.length() == 66 && memo[12] == ',', "Memo format is wrong");
    asset ramcost = get_ram_price();
    asset netcost = asset(1000, EOS_sym);
    asset cpucost = asset(1000, EOS_sym);
    check(quantity >= (ramcost + netcost + cpucost), "Amount of EOS too low");
    asset remaining = quantity - (ramcost+netcost+cpucost);

    //Check parameters
    name accname = name(memo.substr(0,12));
    //check(!is_account(accname), "Account already exists");
    std::string pk_str = memo.substr(13);
    public_key pk_obj = decode_pubkey(pk_str);

    // Create account, buy RAM, delegate BW
    authority auth = authority{
        .threshold = 1,
        .keys = {key_weight{pk_obj,1}},
        .accounts = {},
        .waits = {}
    };
    action(
        permission_level(get_self(), "active"_n),
        "eosio"_n,
        "newaccount"_n,
        std::make_tuple(get_self(), accname, auth, auth)
    ).send();
    action(
        permission_level(get_self(), "active"_n),
        "eosio"_n,
        "buyram"_n,
        std::make_tuple(get_self(), accname, ramcost)
    ).send();
    action(
        permission_level(get_self(), "active"_n),
        "eosio"_n,
        "delegatebw"_n,
        std::make_tuple(get_self(), accname, netcost, cpucost, true)
    ).send();
    action(
        permission_level(get_self(), "active"_n),
        "eosio.token"_n,
        "transfer"_n,
        std::make_tuple(get_self(), accname, remaining, std::string(""))
    ).send();
}

//Inspired by http://knowledge.cryptokylin.io/topics/115
public_key gimmeaccount::decode_pubkey(std::string pk_str) {
    //Remove EOS prefix and check pubkey length
    pk_str = pk_str.substr(3);
    check(pk_str.length() == 50, "Public key string wrong length");

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

// Return cost of 4 KiB of RAM
asset gimmeaccount::get_ram_price() {
    multi_index<"rammarket"_n, rammarket> rp(name("eosio"), name("eosio").value);
    auto it = rp.find(symbol("RAMCORE", 4).raw());
    check(it != rp.end(), "Could not get RAM price");
    uint64_t base = it->base.balance.amount;
    uint64_t quote = it->quote.balance.amount;
    return asset(4096*(double)quote/base, EOS_sym);
}
