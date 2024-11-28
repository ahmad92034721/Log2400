#include "Journee.hpp"
#include <iostream>

Journee::Journee(const std::string& date) : date_(date) {
    std::cout << "Journee " << date_ << " creee!" << std::endl;
}

void Journee::ajouterReservation(const Reservation& reservation) {
    reservations_.push_back(reservation);
}

void Journee::afficher(const std::string& voyageNom, const std::string& segmentNom) const {
    for (const auto& reservation : reservations_) {
        if (reservation.getOffre() == nullptr) { // Vérification de l'offre
            std::cerr << "Erreur: Une reservation contient une offre nulle." << std::endl;
            continue;
        }
        reservation.afficher(voyageNom, segmentNom + "/" + date_);
    }
}
