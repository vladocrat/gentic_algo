#pragma once

#include <string>
#include <vector>
#include <optional>
#include <algorithm>
#include <iterator>

#include "individual.h"

class Population final
{
public:
    using Bounds = std::pair<double, double>;
    using Individuals = std::vector<Individual>;
    using CrossoverResult = std::pair<Individual, Individual>;

    enum class IndividualType
    {
        None = 0,
        Discrete,
        GrayCode
    };

    enum class SelectionType
    {
        None = 0,
        Tournament,
        Rank,
        Panmixia,
        Proportional
    };

    enum class CrossoverType
    {
        None = 0,
        Discrete,
        Linear,
        TwoPoint
    };

    Population(const uint32_t size = 10, const uint8_t dimenions = 1, const IndividualType individualType = IndividualType::None,
               const Bounds& bounds = std::make_pair(-1.0, 1.0));

    //! Selections
    std::optional<Individuals> selection(const SelectionType selectionType);
    Individuals tournamentSelection(const uint32_t tournamentSize = 3);
    Individuals rankSelection();
    Individuals panmixiaSelection();
    Individuals proportionalSelection();

    template<class Func>
    void updateFitness(Func f)
    {        
        for (auto& ind : m_individuals) {
            const auto fitness = [f, &ind]() {
                const auto arg = ind.chromosomes();

                if (std::holds_alternative<Individual::Gene>(arg)) {
                    return f(std::get<Individual::Gene>(arg));
                } else if (std::holds_alternative<Individual::GrayCode>(arg)){
                    const auto& argVec = std::get<Individual::GrayCode>(arg);

                    const auto intValue = [](const std::bitset<8> argVal) -> double {
                        std::bitset<8> binary;
                        binary[7] = argVal[7];

                        for (int i = 6; i >= 0; --i) {
                            binary[i] = binary[i + 1] ^ argVal[i];
                        }

                        return static_cast<double>(binary.to_ulong());
                    };

                    std::vector<double> functionInput;
                    functionInput.resize(argVec.size());

                    std::ranges::transform(argVec, std::back_inserter(functionInput), [&intValue](const auto value) {
                        return intValue(value);
                    });

                    return f(functionInput);
                }

                return 0.0;
            }();

            ind.setFitness(fitness);
        }
    }

    //! Crossovers
    std::optional<CrossoverResult> crossover(const CrossoverType type, const Individual& parent1, const Individual& parent2);
    CrossoverResult discreteCrossover(const Individual& parent1, const Individual& parent2);
    CrossoverResult linearCrossover(const Individual& parent1, const Individual& parent2);
    CrossoverResult twoPointCrossover(const Individual& parent1, const Individual& parent2);

    size_t size() const;
    void setIndividuals(const Individuals& inds);
    const Individuals& individuals() const;
    std::string toString() const;

private:
    Individuals m_individuals;
    IndividualType m_type;
};
