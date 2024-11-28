#include "Voyage.hpp"



// Constructeur
Voyage::Voyage(const std::string& nom) : nom_(nom) {}

// Constructeur de copie
Voyage::Voyage(const Voyage& autre) : nom_(autre.nom_) {
    for (const auto& segment : autre.segments_) {
        segments_.push_back(std::make_unique<Segment>(*segment));
    }
}

// Ajouter un segment
void Voyage::ajouterSegment(std::unique_ptr<Segment> segment) {
    segments_.push_back(std::move(segment));
}

// Remplacer un segment
void Voyage::remplacerSegment(const std::string& nomSegment, std::unique_ptr<Segment> nouveauSegment) {
    for (auto& segment : segments_) {
        if (segment->getNom() == nomSegment) {
            segment = std::move(nouveauSegment);
            return;
        }
    }
}

// Afficher le voyage
void Voyage::afficher() const {
    std::cout << "Voyage " << nom_ << " cree!" << std::endl;
    for (const auto& segment : segments_) {
        segment->afficher(nom_);
    }
}
