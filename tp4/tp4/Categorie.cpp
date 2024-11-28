#include "Categorie.hpp"
#include <algorithm>

Categorie::Categorie(const std::string& nom) : nom_(nom) {}

void Categorie::ajouter(std::unique_ptr<Offre> offre) {
    offres_.push_back(std::move(offre)); // Ajoute une offre dans le vecteur
}

const Offre* Categorie::trouverOffre(std::string_view nom) const {
    // Recherche l'offre par nom dans le vecteur
    auto it = std::find_if(offres_.begin(), offres_.end(),
        [&nom](const std::unique_ptr<Offre>& offre) {
            return offre->getNom() == nom;
        });

    return (it != offres_.end()) ? it->get() : nullptr; // Retourne l'offre si trouvée, sinon nullptr
}
