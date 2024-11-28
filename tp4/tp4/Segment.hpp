#pragma once

#include "Journee.hpp"
#include <vector>
#include <memory>
#include <string>

class Segment {
    std::string nom_;
    std::vector<std::unique_ptr<Journee>> journees_;

public:
    explicit Segment(const std::string& nom);
    void ajouterJournee(std::unique_ptr<Journee> journee);
    void afficher(const std::string& voyage) const;

    const std::string& getNom() const;
};
