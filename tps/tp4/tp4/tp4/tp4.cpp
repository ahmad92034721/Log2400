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

using namespace  std;


double EXCHANGE_RATE = 1.5;


class Offre {
    string nom_;
    double prix_;

public:
    Offre(const string& nom, double prix) : nom_(nom), prix_(prix) {}
    const string& getNom() const { return nom_; }
    double getPrix() const { return prix_; }
};


class OffreSimple : public Offre {
public:
    OffreSimple(const string& nom, double prix) : Offre(nom, prix) {}
};


class Categorie {
    string nom_;
    vector<unique_ptr<Offre>> offres_;

public:
    Categorie(const string& nom) : nom_(nom) {
        cout << "Categorie " << nom << " creee!" << endl;
    } 

    void ajouter(unique_ptr<Offre> offre) {
        cout << "Entree " << offre->getNom() << " rattachee a la categorie " << nom_ << " creee!" << endl;
        offres_.push_back(  move(offre));
    }

    const Offre* trouverOffre(const string& nom) const {
        for (const auto& offre : offres_) {
            if (offre->getNom() == nom) return offre.get();
        }
       
        return nullptr;
    }
};


class BaseDeDonnees {
    map<string, unique_ptr<Categorie>> categories_;

public:
    void ajouterCategorie(const string& nom) {
        categories_[nom] = make_unique<Categorie>(nom);
        cout << "Categorie " << nom << " ajoutée à la base de données.\n";
    }

    void ajouterOffre(const string& categorieNom, unique_ptr<Offre> offre) {
        if (categories_.count(categorieNom)) {
            categories_[categorieNom]->ajouter(  move(offre));
        }
    }

    const Offre* trouverOffre(const   string& categorieNom, const   string& offreNom) const {
        if (categories_.count(categorieNom)) {
            return categories_.at(categorieNom)->trouverOffre(offreNom);
        }
        else {
            return nullptr;
        }
    }

    Categorie* getCategorie(const   string& nom) {
      
        if (categories_.count(nom)) {
           
            return categories_[nom].get();
        }
        else {
         
            return nullptr;
        }
    }

};


class Reservation {
    const Offre* offre_;

public:
    Reservation(const Offre* offre) : offre_(offre) {}
    void afficher(const   string& voyageNom, const   string& segmentNom, const   string& date) const {
        cout << "      Reservation creee : " << voyageNom << "/" << date << "/" << offre_->getNom() << "!" << endl;
    }
    double getPrix() const { return offre_->getPrix(); }
};


class Journee {
      string date_;
      vector<Reservation> reservations_;

public:
    Journee(const   string& date) : date_(date) {
        cout << "    Journee " << date << " creee!" << endl;
    }

    unique_ptr<Journee> clone() const {
        auto journeeClone = make_unique<Journee>(date_);
        journeeClone->reservations_ = reservations_;
        return journeeClone;
    }

    void ajouterReservation(const Reservation& reservation) {
        if (reservation.getPrix() > 0) {
            reservations_.push_back(reservation);
        }
    }

    void afficher(const   string& voyageNom, const   string& segmentNom) const {
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


class Segment {
    string nom_;
    vector<unique_ptr<Journee>> journees_;

public:
    Segment(const string& nom) : nom_(nom) {
        cout << "  Segment " << nom << " cree!" << endl;
    }

    Segment(const Segment& other) : nom_(other.nom_) {
        for (const auto& journee : other.journees_) {
            journees_.push_back(journee->clone());
        }
    }

    unique_ptr<Segment> clone() const {
        return make_unique<Segment>(*this);
    }

    void ajouterJournee(unique_ptr<Journee> journee) {
        journees_.push_back(move(journee));
    }

    void afficher(const   string& voyageNom) const {
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


class Voyage {
    string nom_;
    vector<unique_ptr<Segment>> segments_;

public:
    Voyage(const string& nom) : nom_(nom) {
        cout << "Voyage " << nom << " cree!" << endl;
    }

    Voyage(const Voyage& other, const   string& nouveauNom) : nom_(nouveauNom) {
        for (const auto& segment : other.segments_) {
            segments_.push_back(segment->clone());
        }
        cout << "Voyage copie a partir du Voyage " << other.nom_ << " sous le nom " << nouveauNom << "!" << endl;
    }

    void ajouterSegment(unique_ptr<Segment> segment) {
        segments_.push_back(move(segment));
    }

    void afficher() const {
          cout << "Voyage " << nom_ << " :" << endl;
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


void lireCSV(const string& filePath, const string& categoryName, BaseDeDonnees& bd) {
      wifstream file(filePath);
    file.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
    if (!file.is_open()) {
        throw runtime_error("Unable to open file: " + filePath);
    }



      wstring line;
    int lineNumber = 0;

    // Vérifier si la catégorie existe
    if (!bd.getCategorie(categoryName)) {
          cerr << "Erreur : Categorie " << categoryName << " introuvable." <<   endl;
        return;
    }
    while (  getline(file, line)) {
        lineNumber++;
        wistringstream stream(line);

        wstring name, priceStr, devise;
        double price = 0.0;

        if (categoryName == "Excursion") {
              wstring ville, nbEtoilesStr;

            if (getline(stream, name, L',') &&
                getline(stream, ville, L',') &&
                getline(stream, nbEtoilesStr, L',') &&
                getline(stream, priceStr, L',') &&
                getline(stream, devise, L',')) {

                try {
                    price =   stod(priceStr);
                    if (devise == L"EURO") {
                        price *= EXCHANGE_RATE;
                    }
                    bd.ajouterOffre("Excursion", make_unique<OffreSimple>(string(name.begin(), name.end()), price));
                }
                catch (const   exception& e) {
                    cerr << "error dans lire la line" << lineNumber << ": " << e.what() <<   endl;
                }
            }
            else {
                  cerr << "format invalide " << filePath << ", line: " << lineNumber <<   endl;
            }
        }
        else if (categoryName == "Transport") {
              wstring transporteur, noVol, lieuDepart, lieuArrivee, jourPrevu, heurePrevue;

            if (  getline(stream, name, L',') &&
                  getline(stream, transporteur, L',') &&
                  getline(stream, noVol, L',') &&
                  getline(stream, lieuDepart, L',') &&
                  getline(stream, lieuArrivee, L',') &&
                  getline(stream, jourPrevu, L',') &&
                  getline(stream, heurePrevue, L',') &&
                  getline(stream, priceStr, L',') &&
                  getline(stream, devise, L',')) {

                try {
                    price =   stod(priceStr);
                    if (devise == L"EURO") {
                        price *= EXCHANGE_RATE;
                    }
                    bd.ajouterOffre("Transport",   make_unique<OffreSimple>(  string(name.begin(), name.end()), price));
                }
                catch (const   exception& e) {
                      cerr << "Error processing line " << lineNumber << ": " << e.what() <<   endl;
                }
            }
            else {
                  cerr << "Invalid format in file: " << filePath << ", line: " << lineNumber <<   endl;
            }
        }
        else if (categoryName == "Hebergement") {
              wstring ville, secteur, coteSur10Str;

            if (  getline(stream, name, L',') &&
                  getline(stream, ville, L',') &&
                  getline(stream, secteur, L',') &&
                  getline(stream, coteSur10Str, L',') &&
                  getline(stream, priceStr, L',') &&
                  getline(stream, devise, L',')) {

                try {
                    price =   stod(priceStr);
                    if (devise == L"EURO") {
                        price *= EXCHANGE_RATE;
                    }
                    bd.ajouterOffre("Hebergement",   make_unique<OffreSimple>(  string(name.begin(), name.end()), price));
                }
                catch (const   exception& e) {
                      cerr << "Error processing line " << lineNumber << ": " << e.what() <<   endl;
                }
            }
            else {
                  cerr << "Invalid format in file: " << filePath << ", line: " << lineNumber <<   endl;
            }
        }
    }

    file.close();

}


int main() {
      cout << "Objet BDOR cree!" <<   endl;

    BaseDeDonnees bdor;

    bdor.ajouterCategorie("Transport");
    bdor.ajouterCategorie("Hebergement");
    bdor.ajouterCategorie("Excursion");

    lireCSV("MV_offres_de_reservation.csv", "Transport", bdor);
    lireCSV("Hébergement.csv", "Hebergement", bdor);
    lireCSV("Excursions.csv", "Excursion", bdor);

   
    auto voyageDora =   make_unique<Voyage>("Voyage de Dora");

  
    auto segmentFrance =   make_unique<Segment>("France 1ère partie");

 
    auto journee1 =   make_unique<Journee>("2024-10-26");
    const Offre* offre1 = bdor.trouverOffre("Transport", "Air Canada AC 870 2024-10-26");
    if (offre1) {
        journee1->ajouterReservation(Reservation(offre1));
    }
    segmentFrance->ajouterJournee(  move(journee1));


    auto journee2 = make_unique<Journee>("2024-10-27");
    const Offre* offre2 = bdor.trouverOffre("Hebergement", "Hotel Stella");
    if (offre2) {
        journee2->ajouterReservation(Reservation(offre2));
    }
    segmentFrance->ajouterJournee(move(journee2));


    auto journee3 =   make_unique<Journee>("2024-10-28");
    const Offre* offre3 = bdor.trouverOffre("Excursion", "Paris Dîner-croisière sur la Seine by Bateaux Mouches");
    const Offre* offre4 = bdor.trouverOffre("Hebergement", "Hotel Stella");
    if (offre3) {
        journee3->ajouterReservation(Reservation(offre3));
    }
    if (offre4) {
        journee3->ajouterReservation(Reservation(offre4));
    }
    segmentFrance->ajouterJournee(move(journee3));

  
    voyageDora->ajouterSegment(move(segmentFrance));

   
    auto segmentPortugal = make_unique<Segment>("Portugal");

   
    auto journee4 = make_unique<Journee>("2024-10-29");
    const Offre* offre5 = bdor.trouverOffre("Transport", "TAP Air Portugal TP 0441 2024-10-29");
    const Offre* offre6 = bdor.trouverOffre("Hebergement", "Hôtel Roma");
    if (offre5) {
        journee4->ajouterReservation(Reservation(offre5));
    }
    if (offre6) {
        journee4->ajouterReservation(Reservation(offre6));
    }
    segmentPortugal->ajouterJournee(move(journee4));

   
    auto journee5 = make_unique<Journee>("2024-10-30");
    const Offre* offre7 = bdor.trouverOffre("Excursion", "Visite du Musée Calouste-Gulbenkian");
    const Offre* offre8 = bdor.trouverOffre("Hebergement", "Hôtel Roma");
    if (offre7) {
        journee5->ajouterReservation(Reservation(offre7));
    }
    if (offre8) {
        journee5->ajouterReservation(Reservation(offre8));
    }
    segmentPortugal->ajouterJournee(move(journee5));

 
    voyageDora->ajouterSegment(move(segmentPortugal));

    voyageDora->afficher();
    double prixDora = voyageDora->calculerPrix();
    cout << "Total des frais pour le Voyage de Dora ($ CA): " << prixDora << endl;


    auto voyageDiego = make_unique<Voyage>(*voyageDora, "Voyage de Diego");

    
    auto segmentEspagne = make_unique<Segment>("Espagne");
    auto journee6 =  make_unique<Journee>("2024-10-31");
    const Offre* offre9 = bdor.trouverOffre("Transport", "easyJet 4592 2024-10-31");
    if (offre9) {
        journee6->ajouterReservation(Reservation(offre9));
    }
    segmentEspagne->ajouterJournee(move(journee6));
    voyageDiego->ajouterSegment(move(segmentEspagne));

  
    voyageDiego->afficher();
    double prixDiego = voyageDiego->calculerPrix();
    cout << "Total des frais pour le Voyage de Diego ($ CA): " << prixDiego << endl;

  
    auto voyageAlicia =   make_unique<Voyage>(*voyageDiego, "Voyage d'Alicia");

    voyageAlicia->afficher();
    double prixAlicia = voyageAlicia->calculerPrix();
    cout << "Voyage d’Alicia copie a partir du Voyage de Diego!" << endl;
    cout << "Total des frais pour le Voyage de Dora ($ CA): " << prixDora << endl;
    cout << "Total des frais pour le Voyage de Diego ($ CA): " << prixDiego << endl;
    cout << "Total des frais pour le Voyage d'Alicia ($ CA): " << prixAlicia << endl;

    return 0;
}




