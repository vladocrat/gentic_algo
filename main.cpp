#include <algorithm>
#include <iostream>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

//Турнир Ранговый Дискретная Триадный Низкая/высокая вероятность Вещ. числа/ Код Грея
#include "geneticalgo.h"

const auto print = [](const auto& vec) {
    std::cout << "[";

    for (const auto& x : vec) {
        std::cout << x.toString() << ", ";
    }

    std::cout << "]";
};

int main()
{
    const auto Michalewicz = [](const std::vector<double>& x) {
        double result = 0.0;
        constexpr int m = 1;

        for (int i = 0; i < x.size(); ++i) {
            const auto sinArg = ((i + 1) / M_PI) * std::pow(x[i], 2);
            result += std::sin(x[i]) * std::pow(std::sin(sinArg), 2 * m);
        }

        return -result;
    };

    GeneticAlgo::PopulationSettings settings;
    settings.size = 5;
    settings.dimentions = 5;
    settings.bounds = std::make_pair(0, M_PI);
    settings.type = Population::IndividualType::Discrete;
    settings.selection = Population::SelectionType::Rank;
    settings.crossover = Population::CrossoverType::Discrete;
    settings.mutationChance = 0.05;

    constexpr auto target = -4.650;

    Population::Individuals inds;
    GeneticAlgo algo(100);

    for (int i = 0; i < 100 ; i++) {
        const auto ind = algo.run(settings, Michalewicz, target);
        std::cout << ind.toString() << std::endl;
        inds.push_back(ind);
    }

    const auto minFitnessPred = [](const auto& ind, const auto& other) -> bool {
        return ind.fitness() < other.fitness();
    };

    const auto minElem = *std::ranges::min_element(inds, minFitnessPred);

    std::cout << "best of 100: " << minElem.toString() << std::endl;

    return 0;
}
