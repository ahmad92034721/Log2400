#include "BDOR.hpp"
#include "Categorie.hpp"
#include "Reservation.hpp"
#include "Voyage.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

void chargerDepuisCSV(Categorie& categorie, const std::string& fichierCSV, const std::string& nomCategorie) {
    std::ifstream fichier(fichierCSV);
    if (!fichier.is_open()) {
        std::cerr << "Erreur: Impossible d'ouvrir le fichier " << fichierCSV << std::endl;
        return;
    }

    std::cout << "Categorie " << nomCategorie << " creee!" << std::endl;

    std::string ligne;
    bool premiereLigne = true; // Ignore la première ligne (en-têtes)
    while (std::getline(fichier, ligne)) {
        if (premiereLigne) {
            premiereLigne = false;
            continue;
        }
        ligne = ligne.substr(ligne.find_first_not_of("\xEF\xBB\xBF")); // Supprimer le BOM si présent
        std::string offreNom = ligne.substr(0, ligne.find(',')); // Récupère uniquement la première colonne
        if (!offreNom.empty()) {
            categorie.ajouter(std::make_unique<OffreSimple>(offreNom));
            std::cout << "Entree " << offreNom << " rattachee a la categorie " << nomCategorie << " creee!" << std::endl;
        }
    }

    fichier.close();
}

std::unique_ptr<Voyage> creerVoyageDora(Categorie& transport, Categorie& hebergement, Categorie& excursion) {
    auto voyage = std::make_unique<Voyage>("Voyage de Dora");

    // Segment France 1ère partie
    auto segmentFrance1 = std::make_unique<Segment>("France 1ère partie");

    auto jour1 = std::make_unique<Journee>("2024-10-26");
    const Offre* offre1 = transport.trouverOffre("Air Canada AC 870 2024-10-26");
    if (offre1) {
        jour1->ajouterReservation(Reservation(*offre1, ""));
    }
    segmentFrance1->ajouterJournee(std::move(jour1));

    auto jour2 = std::make_unique<Journee>("2024-10-27");
    const Offre* offre2 = hebergement.trouverOffre("Hôtel Stella");
    if (offre2) {
        jour2->ajouterReservation(Reservation(*offre2, ""));
    }
    segmentFrance1->ajouterJournee(std::move(jour2));

    voyage->ajouterSegment(std::move(segmentFrance1));
    return voyage;
}

std::unique_ptr<Voyage> creerVoyageDiego(const Voyage& voyageDora, Categorie& transport, Categorie& hebergement, Categorie& excursion) {
    auto voyage = std::make_unique<Voyage>(voyageDora);

    auto segmentMadrid = std::make_unique<Segment>("Madrid");

    auto jour1 = std::make_unique<Journee>("2024-10-29");
    const Offre* offre1 = transport.trouverOffre("Air Europa 1026");
    if (offre1) {
        jour1->ajouterReservation(Reservation(*offre1, ""));
    }
    const Offre* offre2 = hebergement.trouverOffre("Far Home Atocha");
    if (offre2) {
        jour1->ajouterReservation(Reservation(*offre2, ""));
    }
    segmentMadrid->ajouterJournee(std::move(jour1));

    voyage->remplacerSegment("Portugal", std::move(segmentMadrid));
    return voyage;
}

std::unique_ptr<Voyage> creerVoyageAlicia(const Voyage& voyageDiego) {
    return std::make_unique<Voyage>(voyageDiego);
}

int main() {
    std::cout << "Objet BDOR cree!" << std::endl;

    auto transport = std::make_unique<Categorie>("Transport");
    auto hebergement = std::make_unique<Categorie>("Hebergement");
    auto excursion = std::make_unique<Categorie>("Excursion");

    chargerDepuisCSV(*transport, "MV_offres_de_reservation.csv", "Transport");
    chargerDepuisCSV(*hebergement, "Hébergement.csv", "Hebergement");
    chargerDepuisCSV(*excursion, "Excursions.csv", "Excursion");

    auto voyageDora = creerVoyageDora(*transport, *hebergement, *excursion);
    voyageDora->afficher();

    auto voyageDiego = creerVoyageDiego(*voyageDora, *transport, *hebergement, *excursion);
    voyageDiego->afficher();

    auto voyageAlicia = creerVoyageAlicia(*voyageDiego);
    voyageAlicia->afficher();

    return 0;
}
