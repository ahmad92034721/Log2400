#include "Segment.hpp"
#include <iostream>

Segment::Segment(const std::string& nom) : nom_(nom) {}

void Segment::ajouterJournee(std::unique_ptr<Journee> journee) {
    journees_.push_back(std::move(journee));
}

void Segment::afficher(const std::string& voyage) const {
    std::cout << "Segment " << nom_ << " cree dans le " << voyage << "!" << std::endl;
    for (const auto& journee : journees_) {
        journee->afficher(voyage, nom_);
    }
}

const std::string& Segment::getNom() const {
    return nom_;
}
