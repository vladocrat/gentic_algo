#pragma once

#include <bitset>
#include <string>
#include <vector>
#include <variant>

class Individual final
{
public:
    using Bounds = std::pair<double, double>;
    using Gene = std::vector<double>;
    using Chromosomes = std::variant<Gene, std::bitset<8>>;

    Individual(const uint8_t dimentions = 1);
    Individual(const Individual&);
    Individual& operator=(const Individual&);
    Individual& operator=(Individual&&);
    Individual(Individual&&);

    void append(const double val);
    void append(const std::bitset<8>);

    void mutate(const double probability, const Bounds& bounds = std::pair(-1.0, 1.0));

    void setFitness(const double val);
    double fitness() const;

    size_t size() const;
    const Chromosomes& chromosomes() const;
    void setChromosomes(const Chromosomes&);

    std::string toString() const;

private:
    double m_fitness = 0.0;
    Chromosomes m_chromoses;
};
