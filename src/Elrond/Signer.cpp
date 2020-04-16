// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Signer.h"
#include "Address.h"
#include "Serialization.h"
#include "../PublicKey.h"
#include "HexCoding.h"

using namespace TW;
using namespace TW::Elrond;


Proto::SigningOutput Signer::sign(const Proto::SigningInput &input) noexcept {
    auto privateKey = PrivateKey(input.private_key());
    auto signableAsString = Elrond::serializeTransactionToSignableString(input.transaction());
    auto signableAsData = TW::data(signableAsString);
    auto signature = privateKey.sign(signableAsData, TWCurveED25519);
    auto encodedSignature = hex(signature);
    auto serializedTransaction = Elrond::serializeSignedTransaction(input.transaction(), encodedSignature);

    auto protoOutput = Proto::SigningOutput();
    protoOutput.set_signature(encodedSignature);
    protoOutput.set_signed_transaction(serializedTransaction);
    return protoOutput;
}

std::string Signer::signJSON(const std::string& json, const Data& key) {
    auto input = Proto::SigningInput();
    google::protobuf::util::JsonStringToMessage(json, &input);
    input.set_private_key(key.data(), key.size());
    auto output = Signer::sign(input);
    return output.signed_transaction();
}
