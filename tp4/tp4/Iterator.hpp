#pragma once
#include "BDOR.hpp"
#include <vector>

class Iterator {
    const std::vector<std::unique_ptr<Offre>>& offres_;
    size_t index_;
public:
    explicit Iterator(const std::vector<std::unique_ptr<Offre>>& offres);
    const Offre* next();
};
