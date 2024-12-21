#pragma once

#include <string>
#include <vector>
#include <optional>

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
        Linear
    };

    Population(const uint32_t size, const uint8_t dimenions = 1, const IndividualType individualType = IndividualType::None,
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
            const auto fitness = f(std::get<Individual::Gene>(ind.chromosomes()));
            ind.setFitness(fitness);
        }
    }

    //! Crossovers
    std::optional<CrossoverResult> crossover(const CrossoverType type, const Individual& parent1, const Individual& parent2);
    CrossoverResult discreteCrossover(const Individual& parent1, const Individual& parent2);
    CrossoverResult linearCrossover(const Individual& parent1, const Individual& parent2);

    size_t size() const;
    void setIndividuals(const Individuals& inds);
    const Individuals& individuals() const;
    std::string toString() const;

private:
    Individuals m_individuals;
    IndividualType m_type;
};
