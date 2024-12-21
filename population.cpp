#include "population.h"

#include <algorithm>
#include <numeric>
#include <random>
#include <cassert>
#include <sstream>

#include "individualfactory.h"

Population::Population(const uint32_t size, const uint8_t dimentions,
                       const IndividualType individualType, const Bounds& bounds)
    : m_type{individualType}
{
    m_individuals.resize(size);

    const auto generator = [&individualType, &dimentions, &bounds]() {
        return IndividualFactory::create(individualType, dimentions, bounds);
    };

    std::ranges::generate(m_individuals, generator);
}

std::optional<Population::Individuals> Population::selection(const SelectionType selectionType)
{
    switch (selectionType) {
    case SelectionType::None:
        return std::nullopt;
    case SelectionType::Tournament:
        return tournamentSelection();
    case SelectionType::Rank:
        return rankSelection();
    case SelectionType::Panmixia:
        return panmixiaSelection();
    case SelectionType::Proportional:
        return proportionalSelection();
    }

    return std::nullopt;
}

Population::Individuals Population::tournamentSelection(const uint32_t tournamentSize)
{
    Individuals selected;
    selected.resize(size());

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, static_cast<int>(size() - 1));

    const auto select = [&, this]() {
        Individuals competitors;
        competitors.resize(tournamentSize);

        std::ranges::generate(competitors, [this, &dist, &gen]() {
            const size_t index = dist(gen);
            return m_individuals[index];
        });

        const auto pred = [](const auto& a, const auto& b) {
            return a.fitness() < b.fitness();
        };

        return *std::ranges::min_element(competitors, pred);
    };

    std::ranges::generate(selected, select);

    return std::move(selected);
}

Population::Individuals Population::rankSelection()
{
    auto sortedInds = m_individuals;

    std::ranges::sort(sortedInds.begin(), sortedInds.end(), [](const auto& a, const auto& b) {
        return a.fitness() < b.fitness();
    });

    return std::move(sortedInds);
}

Population::Individuals Population::panmixiaSelection()
{
    Individuals selected;
    selected.reserve(size());

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, size() - 1);

    std::ranges::transform(m_individuals, std::back_inserter(selected), [this, &dist, &gen](const auto&) {
        return m_individuals[dist(gen)];
    });

    return std::move(selected);
}

Population::Individuals Population::proportionalSelection()
{
    std::vector<double> probabilities;
    probabilities.reserve(size());

    const auto getProbabilities = [](const auto& ind) -> double {
        return 1 / ind.fitness();
    };

    std::ranges::transform(m_individuals, std::back_inserter(probabilities), getProbabilities);
    const auto total = std::accumulate(probabilities.begin(), probabilities.end(), double{});

    const auto normalize = [&total](const double prob) {
        return prob / total;
    };

    std::ranges::transform(probabilities, probabilities.begin(), normalize);

    Individuals selected;
    selected.resize(size());

    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<> dist(probabilities.begin(), probabilities.end());

    std::ranges::generate(selected, [this, &dist, &gen]() {
        const auto ix = dist(gen);
        return m_individuals[ix];
    });

    return selected;
}

std::optional<Population::CrossoverResult> Population::crossover(const CrossoverType type, const Individual& parent1, const Individual& parent2)
{
    switch (type) {
    case CrossoverType::None:
        return std::nullopt;
    case CrossoverType::Discrete:
        if (m_type != IndividualType::Discrete)
            return std::nullopt;

        return discreteCrossover(parent1, parent2);
    case CrossoverType::Linear:
        if (m_type != IndividualType::Discrete)
            return std::nullopt;

        return linearCrossover(parent1, parent2);
    }

    return std::nullopt;
}

Population::CrossoverResult Population::discreteCrossover(const Individual& parent1, const Individual& parent2)
{
    assert(parent1.size() == parent2.size());
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    Individual::Chromosomes child1Chromosomes;
    Individual::Chromosomes child2Chromosomes;

    const auto append = [](auto& to, const auto& from, const size_t ix) {
        auto& toVec = std::get<Individual::Gene>(to);
        const auto& fromVec = std::get<Individual::Gene>(from);
        toVec.push_back(fromVec[ix]);
    };

    for (size_t i = 0; i < parent1.size(); ++i) {
        if (dis(gen) < 0.5) {
            append(child1Chromosomes, parent1.chromosomes(), i);
            append(child2Chromosomes, parent2.chromosomes(), i);
        } else {
            append(child1Chromosomes, parent2.chromosomes(), i);
            append(child2Chromosomes, parent1.chromosomes(), i);
        }
    }

    Individual child1 = std::move(parent1);
    Individual child2 = std::move(parent2);

    child1.setChromosomes(child1Chromosomes);
    child2.setChromosomes(child2Chromosomes);

    return std::make_pair(child1, child2);
}

Population::CrossoverResult Population::linearCrossover(const Individual& parent1, const Individual& parent2)
{
    const auto alpha = 0.5;

    Individual::Chromosomes child1Chromosomes;
    Individual::Chromosomes child2Chromosomes;

    const auto firstChildTransform = [&alpha](const double parent1, const double parent2) {
        return (alpha * parent1) + ((1 - alpha) * parent2);
    };

    const auto secondChildTransform = [&alpha](const double parent1, const double parent2) {
        return ((1 - alpha) * parent1) + (alpha * parent2);
    };

    const auto transform = [](const auto& parent1, const auto& parent2, auto& destination, const auto func) {
        const auto& parent1Vec = std::get<Individual::Gene>(parent1.chromosomes());
        const auto& parent2Vec = std::get<Individual::Gene>(parent2.chromosomes());
        std::ranges::transform(parent1Vec, parent2Vec, std::back_inserter(destination), func);
    };

    auto& child1Vec = std::get<Individual::Gene>(child1Chromosomes);
    auto& child2Vec = std::get<Individual::Gene>(child2Chromosomes);

    transform(parent1, parent2, child1Vec, firstChildTransform);
    transform(parent2, parent1, child2Vec, secondChildTransform);

    Individual child1 = std::move(parent1);
    Individual child2 = std::move(parent2);

    child1.setChromosomes(child1Chromosomes);
    child2.setChromosomes(child2Chromosomes);

    return std::make_pair(child1, child2);
}

size_t Population::size() const
{
    return m_individuals.size();
}

void Population::setIndividuals(const Individuals& inds)
{
    m_individuals = std::move(inds);
}

const Population::Individuals& Population::individuals() const
{
    return m_individuals;
}

std::string Population::toString() const
{
    std::stringstream oss;
    oss << "Size: " << m_individuals.size() << " ";
    oss << "[";

    for (size_t i = 0; i < m_individuals.size(); ++i) {
        oss << m_individuals[i].toString();
        if (i < m_individuals.size() - 1) {
            oss << ", ";
        }
    }
    oss << "]";

    return oss.str();
}
