#pragma once

#include "Segment.hpp"
#include <vector>
#include <memory>
#include <string>

// Classe Voyage
class Voyage {
    std::string nom_;
    std::vector<std::unique_ptr<Segment>> segments_;

public:
    explicit Voyage(const std::string& nom);
    Voyage(const Voyage& autre); // Constructeur de copie
    void ajouterSegment(std::unique_ptr<Segment> segment);
    void remplacerSegment(const std::string& nomSegment, std::unique_ptr<Segment> nouveauSegment);
    void afficher() const;
};
