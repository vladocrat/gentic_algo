#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

#include "population.h"

class GeneticAlgo final
{
public:    
    struct PopulationSettings
    {
        uint32_t size;
        uint8_t dimentions;
        Population::IndividualType type;
        Population::Bounds bounds;
        Population::SelectionType selection;
        Population::CrossoverType crossover;
        double mutationChance;
    };

    GeneticAlgo(const uint8_t epochs)
        : m_epochs{epochs}
    {}

    template<class FitnessFunc>
    Individual run(const PopulationSettings& settings, FitnessFunc func, const double target)
    {
        Population population(settings.size, settings.dimentions, settings.type, settings.bounds);
        std::cout << population.toString();

        double bestFitness = std::numeric_limits<double>::max();
        Individual bestInd;

        for (uint8_t epoch = 0; epoch < m_epochs; epoch++) {
            std::cout << "Epoch: " << size_t(epoch) + 1 << std::endl;
            std::cout << "----------------------------------" << std::endl;
            population.updateFitness(func);

            const auto minFitnessPred = [](const auto& ind, const auto& other) -> bool {
                return ind.fitness() < other.fitness();
            };

            const auto minFitness = *std::ranges::min_element(population.individuals(), minFitnessPred);

            std::cout << "min fitness per epoch: " << minFitness.fitness() << std::endl;

            if (minFitness.fitness() < bestFitness) {
                bestFitness = minFitness.fitness();
                bestInd = minFitness;
            }

            if (bestFitness <= target) {
                return std::move(bestInd);
            }

            const auto selected = population.selection(settings.selection);

            if (!selected) {
                throw std::runtime_error("Failed to select");
            }

            const auto& selectedVal = selected.value();

            Population::Individuals newInds;
            newInds.reserve(selected->size());

            for (size_t i = 0; i < selected->size(); i += 2) {
                const auto& parent1 = selectedVal[i];
                const auto& parent2 = selectedVal[(i + 1) % selectedVal.size()];

                const auto res = population.crossover(settings.crossover, parent1, parent2);

                if (!res) {
                    throw std::runtime_error("Failed to crossover");
                }

                auto [child1, child2] = res.value();
                child1.mutate(settings.mutationChance, settings.bounds);
                child2.mutate(settings.mutationChance, settings.bounds);
                newInds.push_back(child1);
                newInds.push_back(child2);
            }

            population.setIndividuals(newInds);
        }

        return bestInd;
    }

private:
    uint8_t m_epochs;
};
