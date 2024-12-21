#include "individualfactory.h"

#include <bitset>
#include <iostream>
#include <random>

#include "individual.h"

Individual IndividualFactory::create(const Population::IndividualType individualType,
                                     const uint8_t dimentions, const Population::Bounds& bounds)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    switch (individualType) {
    case Population::IndividualType::None:
        break;
    case Population::IndividualType::Discrete: {
        static std::uniform_real_distribution<double> dist(bounds.first, bounds.second);
        Individual ind(dimentions);

        for (uint8_t i = 0; i < dimentions; i++) {
            ind.append(dist(gen));
        }

        return (std::move(ind));
    }
    case Population::IndividualType::GrayCode: {
        static std::uniform_real_distribution<double> dist(bounds.first, bounds.second);
        const auto val = dist(gen);
        const auto maxInt = (1 << 8) - 1;
        const auto scaledValue = static_cast<uint8_t>(std::round(val / bounds.second * maxInt));

        const auto getVal = [] (auto val) -> uint8_t {
            val ^= (val >> 1);
            return val;
        };

        std::cout << val << " " << std::bitset<8>(getVal(scaledValue)) << std::endl;

        Individual ind;
        return std::move(ind);
    }
    }

    return Individual{};
}
