#include "Reservation.hpp"
#include <iostream>

Reservation::Reservation(const Offre& offre, const std::string& metaData)
    : offre_(&offre), metaData_(metaData) {
}

void Reservation::afficher(const std::string& voyage, const std::string& jour) const {
    if (offre_ == nullptr) {
        std::cerr << "Erreur: Offre invalide pour cette réservation." << std::endl;
        return;
    }
    std::cout << "Reservation creee : " << voyage << "/" << jour << "/" << offre_->getNom() << std::endl;
}

const Offre* Reservation::getOffre() const {
    return offre_;
}
