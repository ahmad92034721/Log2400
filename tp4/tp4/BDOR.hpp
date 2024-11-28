#pragma once

#include "Categorie.hpp"
#include <vector>

// Classe abstraite Offre
class Offre {
public:
    virtual void afficher() const = 0;
    virtual const std::string& getNom() const = 0;
    virtual ~Offre() = default;
};

// Classe OffreSimple (Feuille)
class OffreSimple : public Offre {
    std::string nom_;
public:
    explicit OffreSimple(const std::string& nom);
    void afficher() const override;
    const std::string& getNom() const override;
};

// Classe Categorie (Composite)
class Categorie {
    std::string nom_;
    std::vector<std::unique_ptr<Offre>> offres_;
public:
    explicit Categorie(const std::string& nom);
    void ajouter(std::unique_ptr<Offre> offre);
    void afficher() const;
};
