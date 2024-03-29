#include "gimmeaccount.hpp"
#include "base58.hpp"

using namespace eosio;

/*
    Use the funds from an incoming EOS transaction to create an account
    based on information provided in the memo of the transaction. Function is
    not labeled as [[eosio::action]] because it should be ignored by abigen.
*/
void gimmeaccount::transfer(name from, name to, asset quantity, std::string memo) {
    // Only act on incoming EOS transactions, ignore everything else
    if (to != get_self()) {
        return;
    }
    // Check asset (only really necessary on testnet)
    check(quantity.symbol == EOS_sym, "Wrong currency");
    // Check tx memo - should be "<12 characters>,<53 charaters>"
    check(memo.length() == 66 && memo[12] == ',',
        "Memo should be <accname(12chr)>,<pubkey(53chr)>");
    // Check if funds are enough to create new account
    asset ramcost = get_ram_price();
    asset netcost = asset(1000, EOS_sym); // Use 0.1 EOS for NET/CPU
    asset cpucost = asset(1000, EOS_sym);
    check(quantity >= (ramcost + netcost + cpucost), "Amount too low");
    asset remaining = quantity - (ramcost+netcost+cpucost);

    // Check account name and public key
    name accname = name(memo.substr(0,12));
    check(!is_account(accname), "Account already exists");
    std::string pk_str = memo.substr(13);
    public_key pk_obj = decode_pubkey(pk_str);

    // Create account, buy RAM, delegate BW
    auth.keys = {key_weight{pk_obj,1}};
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
    // Remove EOS prefix and check pubkey length
    pk_str = pk_str.substr(3);
    check(pk_str.length() == 50, "Pubkey length");

    // Decode public key
    const std::array<unsigned char, 37> decoded = DecodeBase58(pk_str);
    std::array<char, 33> pk_data;
    std::copy_n(decoded.begin(), 33, pk_data.begin());

    // Evaluate checksum
    checksum160 pk_cs = ripemd160(reinterpret_cast<char*>(pk_data.data()), 33);
    std::array<uint8_t, 20> cs_data = pk_cs.extract_as_byte_array();
    check(std::memcmp(cs_data.begin(), &decoded.end()[-4], 4) == 0, "Pubkey checksum");

    return public_key{0, pk_data};
}

// Return cost of 4 KiB of RAM
asset gimmeaccount::get_ram_price() {
    multi_index<"rammarket"_n, rammarket> rp("eosio"_n, "eosio"_n.value);
    auto it = rp.find(symbol("RAMCORE", 4).raw());
    check(it != rp.end(), "RAM price");
    uint64_t base = it->base.balance.amount;
    uint64_t quote = it->quote.balance.amount;
    return asset((double)4096*quote/base, EOS_sym);
}

//Custom dispatcher to avoid "contract with no actions" error when compiling
extern "C" {
    void apply(uint64_t receiver, uint64_t code, uint64_t action) {
        if (action == "transfer"_n.value && code == "eosio.token"_n.value) {
            execute_action(name(receiver), name(code), &gimmeaccount::transfer);
        }
    }
};
