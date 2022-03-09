#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/name.hpp>
#include <eosio/singleton.hpp>
#include <string>
#include <vector>

// #include "Schema1.hpp"
#include "class1-ricardian.cpp"

namespace contract_name
{
    // using contract_name::Schema1;
    using eosio::check;
    using eosio::contract;
    using eosio::datastream;
    using eosio::name;
    using eosio::print;
    using std::string;

    // Ricardian contracts live in ricardian/class1-ricardian.cpp
    extern const char* mintavatar;

    // The account this contract is normally deployed to
    inline constexpr auto default_contract_account = "angelfarmers"_n;

    class presale_contract : public contract
    {
       public:
        using eosio::contract::contract;
        presale_contract(name receiver, name code, datastream<const char*> ds);
        void mintavatar(eosio::name user, const std::string& avatar_name, const std::string& bio, const std::string& link, const std::string& img);
        // using Schema1Table = eosio::multi_index<"schema1"_n, Schema1>;
    };

    EOSIO_ACTIONS(contract_name::presale_contract, default_contract_account,
                action(mintavatar, avatar_name, bio, link, img, ricardian_contract(mintavatar_ricardian)))

}  // namespace contract_name