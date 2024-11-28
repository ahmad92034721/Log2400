#pragma once

#include "Reservation.hpp"
#include <vector>
#include <string>

class Journee {
    std::string date_;
    std::vector<Reservation> reservations_;

public:
    explicit Journee(const std::string& date);
    void ajouterReservation(const Reservation& reservation);
    void afficher(const std::string& voyageNom, const std::string& segmentNom) const;
};
