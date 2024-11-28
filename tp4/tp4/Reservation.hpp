#pragma once

#include "Offre.hpp"
#include <string>

class Reservation {
    const Offre* offre_;
    std::string metaData_;

public:
    Reservation(const Offre& offre, const std::string& metaData);
    void afficher(const std::string& voyage, const std::string& jour) const;

    const Offre* getOffre() const; // Ajout pour vérifier l'offre
};
