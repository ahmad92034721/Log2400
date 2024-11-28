#include "BDOR.hpp"

// Implémentation de OffreSimple
OffreSimple::OffreSimple(const std::string& nom) : nom_(nom) {}

void OffreSimple::afficher() const {
    std::cout << "Offre: " << nom_ << std::endl;
}

const std::string& OffreSimple::getNom() const {
    return nom_;
}

// Implémentation de Categorie
Categorie::Categorie(const std::string& nom) : nom_(nom) {}

void Categorie::ajouter(std::unique_ptr<Offre> offre) {
    offres_.push_back(std::move(offre));
}

void Categorie::afficher() const {
    std::cout << "Categorie: " << nom_ << std::endl;
    for (const auto& offre : offres_) {
        offre->afficher();
    }
}
