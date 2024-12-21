#pragma once

#include "population.h"

class IndividualFactory final
{
public:
    static Individual create(const Population::IndividualType individualType,
                             const uint8_t dimenions, const Population::Bounds& bounds);
};
