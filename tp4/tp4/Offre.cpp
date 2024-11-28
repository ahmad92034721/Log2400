#include "Offre.hpp"

// Implémentation de OffreSimple
OffreSimple::OffreSimple(const std::string& nom) : nom_(nom) {}

void OffreSimple::afficher() const {
    std::cout << "OffreSimple: " << nom_ << std::endl;
}

const std::string& OffreSimple::getNom() const {
    return nom_;
}
