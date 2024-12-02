#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <fstream>
#include <iomanip>

using namespace std;


double EXCHANGE_RATE = 1.5;


class Offre {
protected:
     string nom_;
    double prix_;
     string commentaire_;

public:
    Offre(const  string& nom, double prix) : nom_(nom), prix_(prix) {}
    virtual ~Offre() = default;

    const  string& getNom() const { return nom_; }
    virtual double getPrix() const { return prix_; }
    void setCommentaire(const  string& commentaire) { commentaire_ = commentaire; }
    const  string& getCommentaire() const { return commentaire_; }

    virtual void afficher() const {
         cout << "Offre: " << nom_ << ", Prix: " << prix_;
        if (!commentaire_.empty()) {
             cout << ", Commentaire: " << commentaire_;
        }
         cout <<  endl;
    }
};


class OffreSimple : public Offre {
public:
    OffreSimple(const  string& nom, double prix) : Offre(nom, prix) {}
};


class OffreDecorateur : public Offre {
protected:
     shared_ptr<Offre> offre_;

public:
    OffreDecorateur( shared_ptr<Offre> offre)
        : Offre(offre->getNom(), offre->getPrix()), offre_(offre) {
    }
    virtual ~OffreDecorateur() = default;

    double getPrix() const override { return offre_->getPrix(); }
    void afficher() const override { offre_->afficher(); }
};


class OffreAvecRabais : public OffreDecorateur {
    double rabais_;

public:
    OffreAvecRabais( shared_ptr<Offre> offre, double rabais)
        : OffreDecorateur(offre), rabais_(rabais) {
    }

    double getPrix() const override {
        return offre_->getPrix() - rabais_;
    }

    void afficher() const override {
        offre_->afficher();
         cout << "  Rabais appliqu : " << rabais_ <<  endl;
    }
};

class Categorie {
     string nom_;
     vector< shared_ptr<Offre>> offres_;

public:
    Categorie(const  string& nom) : nom_(nom) {
         cout << "Categorie " << nom << " creee!" <<  endl;
    }

    void ajouterOffre( shared_ptr<Offre> offre) {
        offres_.push_back(offre);
         cout << "Entree " << offre->getNom() << " rattachee a la categorie "
            << nom_ << " creee!" <<  endl;
    }

    const  shared_ptr<Offre> trouverOffre(const  string& nom) const {
        for (const auto& offre : offres_) {

            if (offre->getNom() == nom) return offre;
        }
         cerr << "Erreur : Offre \"" << nom << "\" non trouv e dans la cat gorie \"" << nom_ << "\"." <<  endl;
        return nullptr;
    }
    size_t getNombreTotalOffres() const {
        return offres_.size();
    }


    void appliquerRabais(double rabais) {
        for (auto& offre : offres_) {
            offre =  make_shared<OffreAvecRabais>(offre, rabais);
        }
    }

    void afficher() const {
         cout << "Categorie: " << nom_ <<  endl;
        for (const auto& offre : offres_) {
            offre->afficher();
        }
    }
};


class BaseDeDonnees {
     map< string,  shared_ptr<Categorie>> categories_;

public:
    void ajouterCategorie(const  string& nom) {
        categories_[nom] =  make_shared<Categorie>(nom);
         cout << "Categorie " << nom << " ajout e   la base de donn es.\n";
    }

    void ajouterOffre(const  string& categorieNom,  shared_ptr<Offre> offre) {
        if (categories_.count(categorieNom)) {
            categories_[categorieNom]->ajouterOffre(offre);
             cout << "Offre \"" << offre->getNom() << "\" ajout e dans la cat gorie \"" << categorieNom << "\"." <<  endl;
        }
        else {
             cerr << "Erreur : Cat gorie \"" << categorieNom << "\" introuvable.\n";
        }
    }


    const  shared_ptr<Offre> trouverOffre(const  string& categorieNom, const  string& offreNom) const {
        if (!categories_.count(categorieNom)) {
             cerr << "Erreur : Cat gorie \"" << categorieNom << "\" introuvable." <<  endl;
            return nullptr;
        }

        auto offre = categories_.at(categorieNom)->trouverOffre(offreNom);
        if (!offre) {
             cerr << "Erreur : Offre \"" << offreNom << "\" introuvable dans la cat gorie \"" << categorieNom << "\"." <<  endl;
        }
        return offre;
    }
    size_t getNombreTotalOffres() const {
        size_t total = 0;
        for (auto it = categories_.begin(); it != categories_.end(); ++it) {
            total += it->second->getNombreTotalOffres();
        }
        return total;
    }



    void appliquerIndexation(double inflationHebergement, double inflationAutres) {
        for (auto& pair : categories_) {
            const  string& nomCategorie = pair.first;
            auto& categorie = pair.second;

            if (nomCategorie == "Hebergement") {
                categorie->appliquerRabais(-inflationHebergement * 0.01);
            }
            else {
                categorie->appliquerRabais(-inflationAutres * 0.01);
            }
        }
    }


    void afficher() const {
        for (const auto& pair : categories_) {
            const  string& nom = pair.first;
            const auto& categorie = pair.second;
            categorie->afficher();
        }
    }
};


class Reservation {
private:
     shared_ptr<Offre> offre_;
     string date_;
     vector< string> commentaires_;
    bool journaliser_ = false;

public:
    Reservation( shared_ptr<Offre> offre, const  string& date)
        : offre_(offre), date_(date) {
    }

    void ajouterCommentaire(const  string& commentaire) {
        commentaires_.push_back(commentaire);
    }

    void activerJournalisation() {
        journaliser_ = true;
    }

    void journaliserDansFichier(const  string& nomVoyageur) const {
        if (!journaliser_) return;

         ofstream logFile("log" + nomVoyageur + ".txt",  ios::app);
        if (logFile) {
            logFile << "Reservation : " << offre_->getNom()
                << ", prix : " << offre_->getPrix()
                << ", date : " << date_ <<  endl;
        }
    }
    const  vector< string>& getCommentaires() const {
        return commentaires_;
    }

    void afficher() const {
         cout << "Reservation " << offre_->getNom()
            << ", prix total ($ CA): " << offre_->getPrix() << "." <<  endl;
        for (const auto& commentaire : commentaires_) {
             cout << "  Commentaire: " << commentaire <<  endl;
        }
    }
     shared_ptr<Offre> getOffre() const {
        return offre_;
    }
    const  string& getDate() const {
        return date_;
    }
};




class Journee {
private:
     string date_;
     vector< shared_ptr<Reservation>> reservations_;

public:
    Journee(const  string& date) : date_(date) {};
    const  vector< shared_ptr<Reservation>>& getReservations() const {
        return reservations_;
    }

    void ajouterReservation( shared_ptr<Reservation> reservation) {
        if (!reservation || !reservation->getOffre()) {
             cerr << "Erreur : R servation invalide ou offre inexistante." <<  endl;
            return;
        }
        reservations_.push_back(reservation);
    }

    const  string& getDate() const {
        return date_;
    }

    void afficher(const  string& voyageNom) const {
         cout << "  Journee " << date_ << ":" <<  endl;
        for (const auto& reservation : reservations_) {
            reservation->afficher();
        }
    }
    void ajouterSousReservation(const  string& date, const  string& nomSousReservation) {
        auto it =  find_if(reservations_.begin(), reservations_.end(),
            [&date](const auto& res) { return res->getDate() == date; });
        if (it != reservations_.end()) {
            (*it)->ajouterCommentaire("Reservation " + nomSousReservation);
             cout << "Sous-reservation " << nomSousReservation << " ajout e pour la date " << date << "." <<  endl;
        }
        else {
             cerr << "Erreur : Aucune r servation trouv e pour la date " << date << "." <<  endl;
        }
    }

    void annulerReservation(const  string& nomOffre) {
        auto it =  remove_if(reservations_.begin(), reservations_.end(),
            [&nomOffre](const auto& res) { return res->getOffre()->getNom() == nomOffre; });
        if (it != reservations_.end()) {
            reservations_.erase(it, reservations_.end());
             cout << "Reservation " << nomOffre << " annul e." <<  endl;
        }
        else {
             cerr << "Reservation " << nomOffre << " introuvable." <<  endl;
        }
    }


    double calculerPrix() const {
        double total = 0;
        for (const auto& reservation : reservations_) {
            total += reservation->getOffre()->getPrix();
        }
        return total;
    }




};


class Segment {
private:
     string nom_;
     vector< shared_ptr<Journee>> journees_;

public:
    Segment(const  string& nom) : nom_(nom) {};
    const  vector< shared_ptr<Journee>>& getJournees() const {
        return journees_;
    }
    const  string& getNom() const {
        return nom_;
    }

    void ajouterJournee( shared_ptr<Journee> journee) {
        if (!journee) {
             cerr << "Erreur : Journ e invalide." <<  endl;
            return;
        }
        journees_.push_back(journee);
    }


    void afficher(const  string& voyageNom) const {
         cout << "  Segment " << nom_ << " cree dans le " << voyageNom << "!" <<  endl;
        for (const auto& journee : journees_) {
            journee->afficher(voyageNom);
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
     vector< shared_ptr<Segment>> segments_;

public:
    Voyage(const  string& nom) : nom_(nom) {
         cout << "Voyage " << nom << " cree!" <<  endl;
    }
    void remplacerSegment(const  string& ancienNom,  shared_ptr<Segment> nouveauSegment) {
        auto it =  find_if(segments_.begin(), segments_.end(), [&](const auto& segment) {
            return segment->getNom() == ancienNom;
            });

        if (it == segments_.end()) {
             cerr << "Erreur : Segment \"" << ancienNom << "\" introuvable." <<  endl;
            return;
        }

        *it = nouveauSegment;
         cout << "Segment \"" << ancienNom << "\" remplac  par \"" << nouveauSegment->getNom() << "\" !" <<  endl;
    }

    void setNom(const  string& nouveauNom) {
        nom_ = nouveauNom;
    }


    void ajouterSegment( shared_ptr<Segment> segment) {
        segments_.push_back(segment);
    }

    void afficher() const {
         cout << "Voyage " << nom_ << ":" <<  endl;
        for (const auto& segment : segments_) {
            segment->afficher(nom_);
        }
    }


    const  string& getNom() const {
        return nom_;
    }

    double calculerPrix() const {
        double total = 0;
        for (const auto& segment : segments_) {
            total += segment->calculerPrix();
        }
        return total;
    }

    void activerJournalisationPourToutesLesReservations() {
        for (const auto& segment : segments_) {
            for (const auto& journee : segment->getJournees()) {
                for (const auto& reservation : journee->getReservations()) {
                    reservation->activerJournalisation();
                }
            }
        }
    }
     shared_ptr<Journee> trouverJournee(const  string& date) const {
        for (const auto& segment : segments_) {
            for (const auto& journee : segment->getJournees()) {
                if (journee->getDate() == date) {
                    return journee;
                }
            }
        }
        return nullptr; 
    }
    void journaliserDansFichier(const  string& nomVoyageur) const {
         ofstream logFile("log" + nomVoyageur + ".txt",  ios::app);
        if (!logFile) {
             cerr << "Erreur : Impossible d'ouvrir le fichier de journalisation pour " << nomVoyageur <<  endl;
            return;
        }

        logFile << "Journalisation pour le voyage : " << nom_ <<  endl;

        for (const auto& segment : segments_) {
            for (const auto& journee : segment->getJournees()) {
                logFile << "  Journee " << journee->getDate() << ":" <<  endl;
                for (const auto& reservation : journee->getReservations()) {
                    logFile << "    Reservation : " << reservation->getOffre()->getNom()
                        << ", prix : " << reservation->getOffre()->getPrix() <<  endl;
                    for (const auto& commentaire : reservation->getCommentaires()) {
                        logFile << "      Commentaire : " << commentaire <<  endl;
                    }
                }
            }
        }
    }
    const  vector< shared_ptr<Segment>>& getSegments() const {
        return segments_;
    }

};


void afficherFraisTotaux(const  vector< shared_ptr<Voyage>>& voyages) {
    for (const auto& voyage : voyages) {
        double total = voyage->calculerPrix();
         cout << "Total des frais pour le " << voyage->getNom()
            << " ($ CA): " << total <<  endl;
    }
}

#include <fstream>
#include <sstream>
#include <locale>
#include <codecvt>


void lireCSV(const  string& filePath, const  string& categoryName, BaseDeDonnees& bd) {
     wifstream file(filePath);
    file.imbue( locale( locale(), new  codecvt_utf8<wchar_t>));
    if (!file.is_open()) {
         cerr << "Erreur : Impossible d'ouvrir le fichier : " << filePath <<  endl;
        return;
    }

     wstring line;
    int lineNumber = 0;

     getline(file, line);

    while ( getline(file, line)) {
        lineNumber++;
         wistringstream stream(line);

         wstring name, priceStr, devise;
        double price = 0.0;

        if (categoryName == "Excursion") {
             wstring ville, nbEtoilesStr;

            if ( getline(stream, name, L',') &&
                 getline(stream, ville, L',') &&
                 getline(stream, nbEtoilesStr, L',') &&
                 getline(stream, priceStr, L',') &&
                 getline(stream, devise, L',')) {

                try {
                    price =  stod(priceStr);
                    if (devise == L"EURO") {
                        price *= EXCHANGE_RATE;
                    }

                    auto offre =  make_shared<OffreSimple>(
                         string(name.begin(), name.end()), price);
                    bd.ajouterOffre("Excursion", offre);

                }
                catch (const  exception& e) {
                     cerr << "Erreur lors du traitement de la ligne " << lineNumber << ": " << e.what() <<  endl;
                }
            }
            else {
                 cerr << "Format invalide dans le fichier : " << filePath << ", ligne : " << lineNumber <<  endl;
            }
        }
        else if (categoryName == "Transport") {
             wstring transporteur, noVol, lieuDepart, lieuArrivee, jourPrevu, heurePrevue;

            if ( getline(stream, name, L',') &&
                 getline(stream, transporteur, L',') &&
                 getline(stream, noVol, L',') &&
                 getline(stream, lieuDepart, L',') &&
                 getline(stream, lieuArrivee, L',') &&
                 getline(stream, jourPrevu, L',') &&
                 getline(stream, heurePrevue, L',') &&
                 getline(stream, priceStr, L',') &&
                 getline(stream, devise, L',')) {

                try {
                    price =  stod(priceStr);
                    if (devise == L"EURO") {
                        price *= EXCHANGE_RATE;
                    }

                    auto offre =  make_shared<OffreSimple>(
                         string(name.begin(), name.end()), price);
                    bd.ajouterOffre("Transport", offre);

                }
                catch (const  exception& e) {
                     cerr << "Erreur lors du traitement de la ligne " << lineNumber << ": " << e.what() <<  endl;
                }
            }
            else {
                 cerr << "Format invalide dans le fichier : " << filePath << ", ligne : " << lineNumber <<  endl;
            }
        }
        else if (categoryName == "Hebergement") {
             wstring ville, secteur, coteSur10Str;

            if ( getline(stream, name, L',') &&
                 getline(stream, ville, L',') &&
                 getline(stream, secteur, L',') &&
                 getline(stream, coteSur10Str, L',') &&
                 getline(stream, priceStr, L',') &&
                 getline(stream, devise, L',')) {

                try {
                    price =  stod(priceStr);
                    if (devise == L"EURO") {
                        price *= EXCHANGE_RATE;
                    }

                    auto offre =  make_shared<OffreSimple>(
                         string(name.begin(), name.end()), price);
                    bd.ajouterOffre("Hebergement", offre);

                }
                catch (const  exception& e) {
                     cerr << "Erreur lors du traitement de la ligne " << lineNumber << ": " << e.what() <<  endl;
                }
            }
            else {
                 cerr << "Format invalide dans le fichier : " << filePath << ", ligne : " << lineNumber <<  endl;
            }
        }
    }

    file.close();
}



int main() {
     cout << "Objet BDOR cree!" <<  endl;
    BaseDeDonnees bdor;

  
    bdor.ajouterCategorie("Transport");
    bdor.ajouterCategorie("Hebergement");
    bdor.ajouterCategorie("Excursion");
 
    lireCSV("MV_offres_de_reservation.csv", "Transport", bdor);
    lireCSV("Hébergement.csv", "Hebergement", bdor);
    lireCSV("Excursions.csv", "Excursion", bdor);
    auto voyageDora =  make_shared<Voyage>("Voyage de Dora");
  
    auto segmentFrance1 =  make_shared<Segment>("France 1 re partie");
    auto journeeFrance1_1 =  make_shared<Journee>("2024-10-26");
    journeeFrance1_1->ajouterReservation( make_shared<Reservation>(
        bdor.trouverOffre("Transport", "Air Canada AC 870 2024-10-26"), "2024-10-26"));
    segmentFrance1->ajouterJournee(journeeFrance1_1);

    auto journeeFrance1_2 =  make_shared<Journee>("2024-10-27");
    journeeFrance1_2->ajouterReservation( make_shared<Reservation>(
        bdor.trouverOffre("Hebergement", "Hotel Stella"), "2024-10-27"));
    segmentFrance1->ajouterJournee(journeeFrance1_2);
    
    auto journeeFrance1_3 =  make_shared<Journee>("2024-10-28");
    journeeFrance1_3->ajouterReservation( make_shared<Reservation>(
        bdor.trouverOffre("Excursion", "Paris Dîner-croisière sur la Seine by Bateaux Mouches"), "2024-10-28"));
    journeeFrance1_3->ajouterReservation( make_shared<Reservation>(
        bdor.trouverOffre("Hebergement", "Hotel Stella"), "2024-10-28"));
    segmentFrance1->ajouterJournee(journeeFrance1_3);
    
    voyageDora->ajouterSegment(segmentFrance1);
    auto segmentPortugal =  make_shared<Segment>("Portugal");
    auto journeePortugal_1 =  make_shared<Journee>("2024-10-29");
    journeePortugal_1->ajouterReservation( make_shared<Reservation>(
        bdor.trouverOffre("Transport", "TAP Air Portugal TP 0441 2024-10-29"), "2024-10-29"));
    journeePortugal_1->ajouterReservation( make_shared<Reservation>(
        bdor.trouverOffre("Hebergement", "Hôtel Roma"), "2024-10-29"));
    segmentPortugal->ajouterJournee(journeePortugal_1);

    voyageDora->ajouterSegment(segmentPortugal);
    auto voyageDiego =  make_shared<Voyage>(*voyageDora);


    auto voyageAlicia =  make_shared<Voyage>(*voyageDiego);

    afficherFraisTotaux({ voyageDora, voyageDiego, voyageAlicia });
     cout << "debut de tp5" <<  endl;
   
    auto journee27 = voyageDora->trouverJournee("2024-10-27");
    if (journee27) {
        journee27->ajouterSousReservation("2024-10-27", "Restaurant de l'h tel Stella pour le 27 octobre 2024   19h");
    }

  
    auto journee31 = voyageDora->trouverJournee("2024-10-31");
    if (journee31) {
        journee31->ajouterSousReservation("2024-10-31", "Restaurant de l'h tel Stella pour le 31 octobre 2024   19h");
        journee31->annulerReservation("Restaurant de l'h tel Stella pour le 31 octobre 2024   19h");
    }

   
    for (auto& segment : voyageDora->getSegments()) {
        for (auto& journee : segment->getJournees()) {
            for (auto& reservation : journee->getReservations()) {
                if (reservation->getOffre()->getNom() == "H tel Stella") {
                    reservation->ajouterCommentaire("Excellent service!");
                }
                else if (reservation->getOffre()->getNom() == "Visite guid e pour voir les chefs-d' uvre du mus e du Louvre") {
                    reservation->ajouterCommentaire("Rabais de 5 dollars canadiens au Louvre pour les etudiants de Polytechnique Montreal!");
                }
            }
        }
    }

 
    bdor.appliquerIndexation(3.0, 2.0);

  
    voyageDora->journaliserDansFichier("Dora");
    voyageDiego->journaliserDansFichier("Diego");
    voyageAlicia->journaliserDansFichier("Alicia");

     cout << "--- Debut de la sortie du TP5\n\n";
    voyageDora->afficher();
    voyageDiego->afficher();
    voyageAlicia->afficher();


     cout << "\nTotal du nombre d'offres de r servations dans la BDOR: "
        << bdor.getNombreTotalOffres() << "." <<  endl;



    return 0;
}







