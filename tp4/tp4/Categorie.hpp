#pragma once

#include "Offre.hpp"
#include <vector>
#include <memory>
#include <string>
#include <string_view>

class Categorie {
    std::string nom_;
    std::vector<std::unique_ptr<Offre>> offres_; // Contient toutes les offres de cette catégorie

public:
    explicit Categorie(const std::string& nom); // Constructeur avec un nom

    void ajouter(std::unique_ptr<Offre> offre); // Ajoute une offre à la catégorie
    const Offre* trouverOffre(std::string_view nom) const; // Trouve une offre par son nom
};
