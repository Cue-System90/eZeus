#include "epakhelpers.h"

#include "engine/eresourcetype.h"

eResourceType ePakHelpers::pakResourceByteToType(
    const uint8_t byte, const bool poseidon) {
    if(byte == 0) {
        return eResourceType::none;
    } else if(byte == 1) {
        return eResourceType::urchin;
    } else if(byte == 2) {
        return eResourceType::fish;
    } else if(byte == 3) {
        return eResourceType::meat;
    } else if(byte == 4) {
        return eResourceType::cheese;
    } else if(byte == 5) {
        return eResourceType::carrots;
    } else if(byte == 6) {
        return eResourceType::onions;
    } else if(byte == 7) {
        return eResourceType::wheat;
    } else if(poseidon && byte == 8) {
        return eResourceType::oranges;
    } else if(byte == 9 + (poseidon ? 0 : -1)) {
        return eResourceType::wood;
    } else if(byte == 10 + (poseidon ? 0 : -1)) {
        return eResourceType::bronze;
    } else if(byte == 11 + (poseidon ? 0 : -1)) {
        return eResourceType::marble;
    } else if(byte == 12 + (poseidon ? 0 : -1)) {
        return eResourceType::grapes;
    } else if(byte == 13 + (poseidon ? 0 : -1)) {
        return eResourceType::olives;
    } else if(byte == 14 + (poseidon ? 0 : -1)) {
        return eResourceType::fleece;
    } else if(!poseidon && byte == 14) {
        return eResourceType::horse;
    } else if(!poseidon && byte == 15) {
        return eResourceType::armor;
    } else if(!poseidon && byte == 16) {
        return eResourceType::sculpture;
    } else if(!poseidon && byte == 17) {
        return eResourceType::oliveOil;
    } else if(!poseidon && byte == 18) {
        return eResourceType::wine;
    } else if(!poseidon && byte == 19) {
        return eResourceType::drachmas;
    } else if(!poseidon && byte == 21) {
        return eResourceType::food;
    } else if(poseidon && byte == 15) {
        return eResourceType::horse;
    } else if(poseidon && byte == 16) {
        return eResourceType::blackMarble;
    } else if(poseidon && byte == 17) {
        return eResourceType::orichalc;
    } else if(poseidon && byte == 18) {
        return eResourceType::armor;
    } else if(poseidon && byte == 19) {
        return eResourceType::sculpture;
    } else if(poseidon && byte == 20) {
        return eResourceType::oliveOil;
    } else if(poseidon && byte == 21) {
        return eResourceType::wine;
    } else if(poseidon && byte == 23) {
        return eResourceType::drachmas;
    } else if(poseidon && byte == 25) {
        return eResourceType::food;
    }

    printf("Invalid resource byte %i\n", byte);
    return eResourceType::none;
}

int ePakHelpers::pakBitmapIdConvert(const int id) {
    if(id == 1) return 0;
    else if(id == 2) return 6;
    else if(id == 3) return 1;
    else if(id == 4) return 11;
    else if(id == 5) return 9;
    else if(id == 6) return 10;
    else if(id == 7) return 3;
    else if(id == 8) return 0;
    else if(id == 9) return 12;
    else if(id == 10) return 13;
    else if(id == 11) return 14;
    else if(id == 12) return 15;
    else if(id == 13) return 16;
    else if(id == 14) return 4;
    return 0;
}
