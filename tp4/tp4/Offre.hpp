#pragma once

#include <string>
#include <iostream>

// Classe abstraite Offre
class Offre {
public:
    virtual void afficher() const = 0;
    virtual const std::string& getNom() const = 0;
    virtual ~Offre() = default;
};

// Classe OffreSimple
class OffreSimple : public Offre {
    std::string nom_;
public:
    explicit OffreSimple(const std::string& nom);
    void afficher() const override;
    const std::string& getNom() const override;
};
