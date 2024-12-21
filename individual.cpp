#include "individual.h"

#include <random>
#include <sstream>


Individual::Individual(const uint8_t dimentions)
{
    if (std::holds_alternative<Gene>(m_chromoses)) {
        std::get<Gene>(m_chromoses).reserve(dimentions);
    }
}

Individual::Individual(const Individual& other)
{
    m_chromoses = other.m_chromoses;
    m_fitness = other.fitness();
}

Individual& Individual::operator=(const Individual& other)
{
    m_chromoses = other.m_chromoses;
    m_fitness = other.fitness();
    return *this;
}

Individual& Individual::operator=(Individual&& other)
{
    m_chromoses = std::move(other.m_chromoses);
    m_fitness = other.fitness();
    return *this;
}

Individual::Individual(Individual&& other)
{
    m_chromoses = std::move(other.m_chromoses);
    m_fitness = other.fitness();
}

void Individual::append(const double val)
{
    if (std::holds_alternative<Gene>(m_chromoses)) {
        std::get<Gene>(m_chromoses).push_back(val);
    }
}

void Individual::append(const std::bitset<8> bitset)
{

}

void Individual::mutate(const double probability, const Bounds& bounds)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    if (std::holds_alternative<Gene>(m_chromoses)) {
        auto& chromosomes = std::get<Gene>(m_chromoses);

        if (dis(gen) < probability) {
            std::uniform_int_distribution<> intDis(0, chromosomes.size() - 1);
            const auto ix = intDis(gen);
            std::uniform_real_distribution<> boundsDis(bounds.first, bounds.second);
            chromosomes[ix] = boundsDis(gen);
        }
    }
}

const Individual::Chromosomes& Individual::chromosomes() const
{
    return m_chromoses;
}

void Individual::setChromosomes(const Chromosomes& chromosomes)
{
    m_chromoses = std::move(chromosomes);
}



std::string Individual::toString() const
{
    std::stringstream ss;
    ss << "Individual, fitness: " << m_fitness;

    ss << " (";

    if (std::holds_alternative<Gene>(m_chromoses)) {
        const auto& chromosomes = std::get<Gene>(m_chromoses);

        for (size_t i = 0; i < chromosomes.size(); ++i) {
            ss << chromosomes[i];

            if (i < chromosomes.size() - 1) {
                ss << ", ";
            }
        }
    }

    ss << ")";

    return ss.str();
}

void Individual::setFitness(const double val)
{
    m_fitness = val;
}

double Individual::fitness() const
{
    return m_fitness;
}

size_t Individual::size() const
{
    if (std::holds_alternative<Gene>(m_chromoses)) {
        return std::get<Gene>(m_chromoses).size();
    }

    return 8;
}
