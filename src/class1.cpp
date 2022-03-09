#include <string>
#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>

#include "token.hpp"
#include "atomicassets-interface.hpp"
// #include "atomicassets.hpp"

using namespace std;
using namespace eosio;
using namespace token;

#define AWAX_SYMBOL symbol("AWAX", 4)
namespace contract_name
{
   // Keep track of deposited funds
   struct balance
   {
      eosio::name owner;
      eosio::asset balance;

      uint64_t primary_key() const { return owner.value; }
   };
   EOSIO_REFLECT(balance, owner, balance)
   typedef eosio::multi_index<"balance"_n, balance> balance_table;

   struct presale_contract : public eosio::contract
   {
        using eosio::contract::contract;

        // eosio.token transfer notification
        void notify_transfer(eosio::name from,
                           eosio::name to,
                           const eosio::asset& quantity,
                           std::string memo)
        { 
            // Only track incoming transfers
            if (from == get_self()) return;
            
            // Skip deposits 
            if (memo == "deposit") return;

            // The dispatcher has already checked the token contract.
            // We need to check the token type.
            eosio::check(quantity.symbol.code() == AWAX_SYMBOL.code(), "only AWAX token is accepted");

            // Record the change
            add_balance(from, quantity);            
        }

        // Action: mint avatar nft
        void mintavatar(eosio::name user, string name, string bio, string link, string img)
        {
            require_auth(user);

            asset price;
            price.amount = 100;
            price.symbol = symbol("AWAX", 4);
            sub_balance(user, price);

            // mint nft on atomic assets
            action(
                permission_level{get_self(), "active"_n},
                atomicassets::ATOMICASSETS_ACCOUNT,
                "mintasset"_n,
                make_tuple(
                    get_self(),
                    "multiavatarz"_n,
                    "avatar"_n,
                    -1,
                    user,
                    (atomicassets::ATTRIBUTE_MAP) {{"name", name}, {"img", img}},
                    (atomicassets::ATTRIBUTE_MAP) {{"bio", bio}, {"link", link}},
                    (vector <asset>) {}
                )
            ).send();                    
        }

        // This is not an action; it's a function internal to the contract
        void add_balance(eosio::name owner, const eosio::asset& quantity)
        {
            balance_table table(get_self(), get_self().value);
            auto record = table.find(owner.value);
            if (record == table.end())
                table.emplace(get_self(), [&](auto& a) {
                    a.owner = owner;
                    a.balance = quantity;
                });
            else
                table.modify(record, eosio::same_payer, [&](auto& a) { a.balance += quantity; });
        }

        // This is not an action; it's a function internal to the contract
        void sub_balance(eosio::name owner, const eosio::asset& quantity)
        {
            balance_table table(get_self(), get_self().value);
            const auto& record = table.get(owner.value, "user does not have a balance");
            eosio::check(record.balance.amount >= quantity.amount, "not enough funds deposited");
            table.modify(record, owner, [&](auto& a) { a.balance -= quantity; });
        }

    };

   EOSIO_ACTIONS(presale_contract,
                 "angelfarmers"_n,
                 notify("awaxdaotoken"_n, transfer))
}  // namespace contract_name

EOSIO_ACTION_DISPATCHER(contract_name::actions)
EOSIO_ABIGEN(actions(contract_name::actions))
