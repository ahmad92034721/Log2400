#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <codecvt>
#include <locale>
#include <map>

// Constante pour le taux de change
double EXCHANGE_RATE = 1.5; // Exchange rate for CAD to EUR

// Classe de base pour les offres
class Offre {
    std::string nom_;
    double prix_;

public:
    Offre(const std::string& nom, double prix) : nom_(nom), prix_(prix) {}
    const std::string& getNom() const { return nom_; }
    double getPrix() const { return prix_; }
};

// Classe dérivée pour les offres simples
class OffreSimple : public Offre {
public:
    OffreSimple(const std::string& nom, double prix) : Offre(nom, prix) {}
};

// Classe Categorie pour regrouper les offres
class Categorie {
    std::string nom_;
    std::vector<std::unique_ptr<Offre>> offres_;

public:
    Categorie(const std::string& nom) : nom_(nom) {
        std::cout << "Categorie " << nom << " creee!" << std::endl;
    }

    void ajouter(std::unique_ptr<Offre> offre) {
        std::cout << "Entree " << offre->getNom() << " rattachee a la categorie " << nom_ << " creee!" << std::endl;
        offres_.push_back(std::move(offre));
    }

    const Offre* trouverOffre(const std::string& nom) const {
        for (const auto& offre : offres_) {
            if (offre->getNom() == nom) return offre.get();
        }
        std::cerr << "Error: Offer not found: " << nom << std::endl;
        return nullptr;
    }
};

// Classe BaseDeDonnees pour gérer les catégories et les offres
class BaseDeDonnees {
    std::map<std::string, std::unique_ptr<Categorie>> categories_;

public:
    void ajouterCategorie(const std::string& nom) {
        categories_[nom] = std::make_unique<Categorie>(nom);
        std::cout << "Categorie " << nom << " ajoutée à la base de données.\n";
    }

    void ajouterOffre(const std::string& categorieNom, std::unique_ptr<Offre> offre) {
        if (categories_.count(categorieNom)) {
            categories_[categorieNom]->ajouter(std::move(offre));
        }
        else {
            std::cerr << "Erreur : Categorie " << categorieNom << " introuvable.\n";
        }
    }

    const Offre* trouverOffre(const std::string& categorieNom, const std::string& offreNom) const {
        if (categories_.count(categorieNom)) {
            return categories_.at(categorieNom)->trouverOffre(offreNom);
        }
        else {
            std::cerr << "Erreur : Categorie " << categorieNom << " introuvable.\n";
            return nullptr;
        }
    }

    Categorie* getCategorie(const std::string& nom) {
        return categories_.count(nom) ? categories_[nom].get() : nullptr;
    }
};

// Classe Reservation
class Reservation {
    const Offre* offre_;

public:
    Reservation(const Offre* offre) : offre_(offre) {}
    void afficher(const std::string& voyageNom, const std::string& segmentNom, const std::string& date) const {
        std::cout << "      Reservation creee : " << voyageNom << "/" << date << "/" << offre_->getNom() << "!" << std::endl;
    }
    double getPrix() const { return offre_->getPrix(); }
};

// Classe Journee
class Journee {
    std::string date_;
    std::vector<Reservation> reservations_;

public:
    Journee(const std::string& date) : date_(date) {
        std::cout << "    Journee " << date << " creee!" << std::endl;
    }

    std::unique_ptr<Journee> clone() const {
        auto journeeClone = std::make_unique<Journee>(date_);
        journeeClone->reservations_ = reservations_;
        return journeeClone;
    }

    void ajouterReservation(const Reservation& reservation) {
        if (reservation.getPrix() > 0) {
            reservations_.push_back(reservation);
        }
        else {
            std::cerr << "Invalid reservation not added for date: " << date_ << std::endl;
        }
    }

    void afficher(const std::string& voyageNom, const std::string& segmentNom) const {
        for (const auto& reservation : reservations_) {
            reservation.afficher(voyageNom, segmentNom, date_);
        }
    }

    double calculerPrix() const {
        double total = 0;
        for (const auto& reservation : reservations_) {
            total += reservation.getPrix();
        }
        return total;
    }
};

// Classe Segment
class Segment {
    std::string nom_;
    std::vector<std::unique_ptr<Journee>> journees_;

public:
    Segment(const std::string& nom) : nom_(nom) {
        std::cout << "  Segment " << nom << " cree!" << std::endl;
    }

    Segment(const Segment& other) : nom_(other.nom_) {
        for (const auto& journee : other.journees_) {
            journees_.push_back(journee->clone());
        }
    }

    std::unique_ptr<Segment> clone() const {
        return std::make_unique<Segment>(*this);
    }

    void ajouterJournee(std::unique_ptr<Journee> journee) {
        journees_.push_back(std::move(journee));
    }

    void afficher(const std::string& voyageNom) const {
        for (const auto& journee : journees_) {
            journee->afficher(voyageNom, nom_);
        }
    }

    double calculerPrix() const {
        double total = 0;
        for (const auto& journee : journees_) {
            total += journee->calculerPrix();
        }
        return total;
    }
};

// Classe Voyage
class Voyage {
    std::string nom_;
    std::vector<std::unique_ptr<Segment>> segments_;

public:
    Voyage(const std::string& nom) : nom_(nom) {
        std::cout << "Voyage " << nom << " cree!" << std::endl;
    }

    Voyage(const Voyage& other, const std::string& nouveauNom) : nom_(nouveauNom) {
        for (const auto& segment : other.segments_) {
            segments_.push_back(segment->clone());
        }
        std::cout << "Voyage copie a partir du Voyage " << other.nom_ << " sous le nom " << nouveauNom << "!" << std::endl;
    }

    void ajouterSegment(std::unique_ptr<Segment> segment) {
        segments_.push_back(std::move(segment));
    }

    void afficher() const {
        std::cout << "Voyage " << nom_ << " :" << std::endl;
        for (const auto& segment : segments_) {
            segment->afficher(nom_);
        }
    }

    double calculerPrix() const {
        double total = 0;
        for (const auto& segment : segments_) {
            total += segment->calculerPrix();
        }
        return total;
    }
};

// Fonction pour lire les données depuis un fichier CSV
void lireCSV(const std::string& filePath, const std::string& categoryName, BaseDeDonnees& bd) {
    std::wifstream file(filePath);
    file.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + filePath);
    }



    std::wstring line;
    int lineNumber = 0;

    // Vérifier si la catégorie existe
    if (!bd.getCategorie(categoryName)) {
        std::cerr << "Erreur : Categorie " << categoryName << " introuvable." << std::endl;
        return;
    }

    // Skip the first line (header)
    std::getline(file, line);

    while (std::getline(file, line)) {
        lineNumber++;
        std::wistringstream stream(line);

        std::wstring name, priceStr, devise;
        double price = 0.0;

        if (categoryName == "Excursion") {
            std::wstring ville, nbEtoilesStr;

            if (std::getline(stream, name, L',') &&
                std::getline(stream, ville, L',') &&
                std::getline(stream, nbEtoilesStr, L',') &&
                std::getline(stream, priceStr, L',') &&
                std::getline(stream, devise, L',')) {

                try {
                    price = std::stod(priceStr);
                    if (devise == L"EURO") {
                        price *= EXCHANGE_RATE;
                    }
                    bd.ajouterOffre("Excursion", std::make_unique<OffreSimple>(std::string(name.begin(), name.end()), price));
                }
                catch (const std::exception& e) {
                    std::cerr << "Error processing line " << lineNumber << ": " << e.what() << std::endl;
                }
            }
            else {
                std::cerr << "Invalid format in file: " << filePath << ", line: " << lineNumber << std::endl;
            }
        }
        else if (categoryName == "Transport") {
            std::wstring transporteur, noVol, lieuDepart, lieuArrivee, jourPrevu, heurePrevue;

            if (std::getline(stream, name, L',') &&
                std::getline(stream, transporteur, L',') &&
                std::getline(stream, noVol, L',') &&
                std::getline(stream, lieuDepart, L',') &&
                std::getline(stream, lieuArrivee, L',') &&
                std::getline(stream, jourPrevu, L',') &&
                std::getline(stream, heurePrevue, L',') &&
                std::getline(stream, priceStr, L',') &&
                std::getline(stream, devise, L',')) {

                try {
                    price = std::stod(priceStr);
                    if (devise == L"EURO") {
                        price *= EXCHANGE_RATE;
                    }
                    bd.ajouterOffre("Transport", std::make_unique<OffreSimple>(std::string(name.begin(), name.end()), price));
                }
                catch (const std::exception& e) {
                    std::cerr << "Error processing line " << lineNumber << ": " << e.what() << std::endl;
                }
            }
            else {
                std::cerr << "Invalid format in file: " << filePath << ", line: " << lineNumber << std::endl;
            }
        }
        else if (categoryName == "Hebergement") {
            std::wstring ville, secteur, coteSur10Str;

            if (std::getline(stream, name, L',') &&
                std::getline(stream, ville, L',') &&
                std::getline(stream, secteur, L',') &&
                std::getline(stream, coteSur10Str, L',') &&
                std::getline(stream, priceStr, L',') &&
                std::getline(stream, devise, L',')) {

                try {
                    price = std::stod(priceStr);
                    if (devise == L"EURO") {
                        price *= EXCHANGE_RATE;
                    }
                    bd.ajouterOffre("Hebergement", std::make_unique<OffreSimple>(std::string(name.begin(), name.end()), price));
                }
                catch (const std::exception& e) {
                    std::cerr << "Error processing line " << lineNumber << ": " << e.what() << std::endl;
                }
            }
            else {
                std::cerr << "Invalid format in file: " << filePath << ", line: " << lineNumber << std::endl;
            }
        }
    }

    file.close();

}


int main() {
    std::cout << "Objet BDOR cree!" << std::endl;

    // Initialisation de la base de données
    BaseDeDonnees bdor;

    // Création des catégories
    bdor.ajouterCategorie("Transport");
    bdor.ajouterCategorie("Hebergement");
    bdor.ajouterCategorie("Excursion");

    // Lecture des fichiers CSV et remplissage des catégories
    lireCSV("MV_offres_de_reservation.csv", "Transport", bdor);
    lireCSV("Hébergement.csv", "Hebergement", bdor);
    lireCSV("Excursions.csv", "Excursion", bdor);

    // Création du voyage de Dora
    auto voyageDora = std::make_unique<Voyage>("Voyage de Dora");

    // Segment : France 1ère partie
    auto segmentFrance = std::make_unique<Segment>("France 1ère partie");

    // Jour 1 : Transport
    auto journee1 = std::make_unique<Journee>("2024-10-26");
    const Offre* offre1 = bdor.trouverOffre("Transport", "Air Canada AC 870 2024-10-26");
    if (offre1) {
        journee1->ajouterReservation(Reservation(offre1));
    }
    segmentFrance->ajouterJournee(std::move(journee1));

    // Jour 2 : Hébergement
    auto journee2 = std::make_unique<Journee>("2024-10-27");
    const Offre* offre2 = bdor.trouverOffre("Hebergement", "Hotel Stella");
    if (offre2) {
        journee2->ajouterReservation(Reservation(offre2));
    }
    segmentFrance->ajouterJournee(std::move(journee2));

    // Jour 3 : Hébergement et Excursion
    auto journee3 = std::make_unique<Journee>("2024-10-28");
    const Offre* offre3 = bdor.trouverOffre("Excursion", "Paris Dîner-croisière sur la Seine by Bateaux Mouches");
    const Offre* offre4 = bdor.trouverOffre("Hebergement", "Hotel Stella");
    if (offre3) {
        journee3->ajouterReservation(Reservation(offre3));
    }
    if (offre4) {
        journee3->ajouterReservation(Reservation(offre4));
    }
    segmentFrance->ajouterJournee(std::move(journee3));

    // Ajouter le segment au voyage de Dora
    voyageDora->ajouterSegment(std::move(segmentFrance));

    // Segment : Portugal
    auto segmentPortugal = std::make_unique<Segment>("Portugal");

    // Jour 4 : Transport et Hébergement
    auto journee4 = std::make_unique<Journee>("2024-10-29");
    const Offre* offre5 = bdor.trouverOffre("Transport", "TAP Air Portugal TP 0441 2024-10-29");
    const Offre* offre6 = bdor.trouverOffre("Hebergement", "Hôtel Roma");
    if (offre5) {
        journee4->ajouterReservation(Reservation(offre5));
    }
    if (offre6) {
        journee4->ajouterReservation(Reservation(offre6));
    }
    segmentPortugal->ajouterJournee(std::move(journee4));

    // Jour 5 : Hébergement et Excursion
    auto journee5 = std::make_unique<Journee>("2024-10-30");
    const Offre* offre7 = bdor.trouverOffre("Excursion", "Visite du Musée Calouste-Gulbenkian");
    const Offre* offre8 = bdor.trouverOffre("Hebergement", "Hôtel Roma");
    if (offre7) {
        journee5->ajouterReservation(Reservation(offre7));
    }
    if (offre8) {
        journee5->ajouterReservation(Reservation(offre8));
    }
    segmentPortugal->ajouterJournee(std::move(journee5));

    // Ajouter le segment au voyage de Dora
    voyageDora->ajouterSegment(std::move(segmentPortugal));

    // Afficher le voyage de Dora
    voyageDora->afficher();
    double prixDora = voyageDora->calculerPrix();
    std::cout << "Total des frais pour le Voyage de Dora ($ CA): " << prixDora << std::endl;

    // Copier le voyage de Dora pour Diego
    auto voyageDiego = std::make_unique<Voyage>(*voyageDora, "Voyage de Diego");

    // Modifier pour l'Espagne
    auto segmentEspagne = std::make_unique<Segment>("Espagne");
    auto journee6 = std::make_unique<Journee>("2024-10-31");
    const Offre* offre9 = bdor.trouverOffre("Transport", "easyJet 4592 2024-10-31");
    if (offre9) {
        journee6->ajouterReservation(Reservation(offre9));
    }
    segmentEspagne->ajouterJournee(std::move(journee6));
    voyageDiego->ajouterSegment(std::move(segmentEspagne));

    // Afficher le voyage de Diego
    voyageDiego->afficher();
    double prixDiego = voyageDiego->calculerPrix();
    std::cout << "Total des frais pour le Voyage de Diego ($ CA): " << prixDiego << std::endl;

    // Copier le voyage de Diego pour Alicia
    auto voyageAlicia = std::make_unique<Voyage>(*voyageDiego, "Voyage d'Alicia");

    // Afficher le voyage d'Alicia
    voyageAlicia->afficher();
    double prixAlicia = voyageAlicia->calculerPrix();
    std::cout << "Voyage d’Alicia copie a partir du Voyage de Diego!" << std::endl;
    std::cout << "Total des frais pour le Voyage de Dora ($ CA): " << prixDora << std::endl;
    std::cout << "Total des frais pour le Voyage de Diego ($ CA): " << prixDiego << std::endl;
    std::cout << "Total des frais pour le Voyage d'Alicia ($ CA): " << prixAlicia << std::endl;

    return 0;
}




