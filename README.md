## The quickest and easiest way to create an EOS account.

No fees, no credit card or Paypal, no need to download any software. Completely trustless and on-chain, and fairly anonymous. Send your EOS from an exchange directly to your own newly created account.

## <a name="example"></a>How does it work?

GimmeAccount is useful if you want to withdraw your EOS from an exchange but don't have an account. You can simply send the coins from the exchange to `gimmeaccount`, and submit an account name and public key in the memo. The GimmeAccount smart contract will create the account for you with the provided name and public key, and forward your coins to the account.

Here's an example of a memo that will create an account named `mynewaccount` with public key `EOS53xc98JpPXCpyEaqhJGfLWberWrrGsFvfRWwoVcTnc5PHLQ8Vj`:

    mynewaccount,EOS53xc98JpPXCpyEaqhJGfLWberWrrGsFvfRWwoVcTnc5PHLQ8Vj

Simple as that - first the account name, then a comma, then the public key. Make sure the account name is 12 characters long, only lower case a-z and/or numbers 1-5, and doesn't already exist. Make sure the public key starts with EOS and is 53 characters long (like all EOS public keys). Don't worry, if something is wrong, the transaction will fail and your coins won't leave the exchange.

Want to know more about accounts and keys before you create your own? This is a great explanation of how this all works on EOS and how EOS accounts are different from traditional private/public key pairs you use with other cryptocurrencies:

(link coming soon)

## No fees

GimmeAccount does not charge any fees for account creation - 100% of the EOS you send to the contract will be forwarded to your new account. The amount of EOS necessary for creating an account is the price of 4 KiB of RAM, which is usually around 0.4 EOS, plus 0.2 EOS for allocating NET/CPU resources. So the minimum amount of EOS you should send to the account is around 0.6 EOS, but for the exact number see:

(link coming soon)

If you send more than the minimum required, the excess will be sent to your new account. If you send less than the minimum required, the transaction will fail and the coins will not leave the wallet they were in.

## I don't have my coins on an exchange

GimmeAccount doesn't work for exchanges specifically, it works for any incoming transaction. You can send some EOS from anywhere and it will create an account for you, as long as you submit an account name and public key in the memo. For example, here's how to create an account using cleos:

    cleos transfer myoldaccount gimmeaccount "1 EOS" "mynewaccount,EOS53xc98JpPXCpyEaqhJGfLWberWrrGsFvfRWwoVcTnc5PHLQ8Vj"

This will create the same account as in the example [above](#example), except the funds are now being send from the account `myoldaccount` instead of from an exchange.

## Completely trustless

GimmeAccount is a smart contract that works completely trustless and on-chain. If the account creation fails due to whatever reason, the entire transaction fails, which means your coins will not even leave the exchange. So no need to worry if you didn't send enough EOS, or if you entered a wrong account name or public key.

If you know a bit of c++, you can verify that the contract works as advertised by downloading and compiling the source code and comparing the code hash to the code hash of `gimmeaccount`. Here are some bash commands to get you started:

    # Download and compile code
    git clone https://github.com/nearestneighbour/gimmeaccount.git
    cd gimmeaccount
    source compile.sh
    # Check code hash from compiled contract
    openssl dgst -sha256 gimmeaccount/gimmeaccount.wasm
    # Compare to code hash from mainnet account
    cleos get code hash gimmeaccount
