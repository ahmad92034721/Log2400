#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <fstream>
#include <iomanip>

// Taux de change pour les conversions (modifiable sans recompilation)
double EXCHANGE_RATE = 1.5;

// Classe de base Offre
class Offre {
protected:
    std::string nom_;
    double prix_;
    std::string commentaire_;

public:
    Offre(const std::string& nom, double prix) : nom_(nom), prix_(prix) {}
    virtual ~Offre() = default;

    const std::string& getNom() const { return nom_; }
    virtual double getPrix() const { return prix_; }
    void setCommentaire(const std::string& commentaire) { commentaire_ = commentaire; }
    const std::string& getCommentaire() const { return commentaire_; }

    virtual void afficher() const {
        std::cout << "Offre: " << nom_ << ", Prix: " << prix_;
        if (!commentaire_.empty()) {
            std::cout << ", Commentaire: " << commentaire_;
        }
        std::cout << std::endl;
    }
};

// Classe d riv e OffreSimple
class OffreSimple : public Offre {
public:
    OffreSimple(const std::string& nom, double prix) : Offre(nom, prix) {}
};

// D corateur de base pour Offre
class OffreDecorateur : public Offre {
protected:
    std::shared_ptr<Offre> offre_;

public:
    OffreDecorateur(std::shared_ptr<Offre> offre)
        : Offre(offre->getNom(), offre->getPrix()), offre_(offre) {
    }
    virtual ~OffreDecorateur() = default;

    double getPrix() const override { return offre_->getPrix(); }
    void afficher() const override { offre_->afficher(); }
};

// D corateur pour appliquer un rabais
class OffreAvecRabais : public OffreDecorateur {
    double rabais_;

public:
    OffreAvecRabais(std::shared_ptr<Offre> offre, double rabais)
        : OffreDecorateur(offre), rabais_(rabais) {
    }

    double getPrix() const override {
        return offre_->getPrix() - rabais_;
    }

    void afficher() const override {
        offre_->afficher();
        std::cout << "  Rabais appliqu : " << rabais_ << std::endl;
    }
};
// Classe Categorie pour regrouper les offres
class Categorie {
    std::string nom_;
    std::vector<std::shared_ptr<Offre>> offres_;

public:
    Categorie(const std::string& nom) : nom_(nom) {
        std::cout << "Categorie " << nom << " creee!" << std::endl;
    }

    void ajouterOffre(std::shared_ptr<Offre> offre) {
        offres_.push_back(offre);
        std::cout << "Entree " << offre->getNom() << " rattachee a la categorie "
            << nom_ << " creee!" << std::endl;
    }

    const std::shared_ptr<Offre> trouverOffre(const std::string& nom) const {
        for (const auto& offre : offres_) {

            if (offre->getNom() == nom) return offre;
        }
        std::cerr << "Erreur : Offre \"" << nom << "\" non trouv e dans la cat gorie \"" << nom_ << "\"." << std::endl;
        return nullptr;
    }
    size_t getNombreTotalOffres() const {
        return offres_.size();
    }


    void appliquerRabais(double rabais) {
        for (auto& offre : offres_) {
            offre = std::make_shared<OffreAvecRabais>(offre, rabais);
        }
    }

    void afficher() const {
        std::cout << "Categorie: " << nom_ << std::endl;
        for (const auto& offre : offres_) {
            offre->afficher();
        }
    }
};

// Classe BaseDeDonnees pour g rer les cat gories et les offres
class BaseDeDonnees {
    std::map<std::string, std::shared_ptr<Categorie>> categories_;

public:
    void ajouterCategorie(const std::string& nom) {
        categories_[nom] = std::make_shared<Categorie>(nom);
        std::cout << "Categorie " << nom << " ajout e   la base de donn es.\n";
    }

    void ajouterOffre(const std::string& categorieNom, std::shared_ptr<Offre> offre) {
        if (categories_.count(categorieNom)) {
            categories_[categorieNom]->ajouterOffre(offre);
            std::cout << "Offre \"" << offre->getNom() << "\" ajout e dans la cat gorie \"" << categorieNom << "\"." << std::endl;
        }
        else {
            std::cerr << "Erreur : Cat gorie \"" << categorieNom << "\" introuvable.\n";
        }
    }


    const std::shared_ptr<Offre> trouverOffre(const std::string& categorieNom, const std::string& offreNom) const {
        if (!categories_.count(categorieNom)) {
            std::cerr << "Erreur : Cat gorie \"" << categorieNom << "\" introuvable." << std::endl;
            return nullptr;
        }

        auto offre = categories_.at(categorieNom)->trouverOffre(offreNom);
        if (!offre) {
            std::cerr << "Erreur : Offre \"" << offreNom << "\" introuvable dans la cat gorie \"" << categorieNom << "\"." << std::endl;
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
            const std::string& nomCategorie = pair.first;
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
            const std::string& nom = pair.first;
            const auto& categorie = pair.second;
            categorie->afficher();
        }
    }
};

// Classe Reservation
class Reservation {
private:
    std::shared_ptr<Offre> offre_;
    std::string date_;
    std::vector<std::string> commentaires_;
    bool journaliser_ = false;

public:
    Reservation(std::shared_ptr<Offre> offre, const std::string& date)
        : offre_(offre), date_(date) {
    }

    void ajouterCommentaire(const std::string& commentaire) {
        commentaires_.push_back(commentaire);
    }

    void activerJournalisation() {
        journaliser_ = true;
    }

    void journaliserDansFichier(const std::string& nomVoyageur) const {
        if (!journaliser_) return;

        std::ofstream logFile("log" + nomVoyageur + ".txt", std::ios::app);
        if (logFile) {
            logFile << "Reservation : " << offre_->getNom()
                << ", prix : " << offre_->getPrix()
                << ", date : " << date_ << std::endl;
        }
    }
    const std::vector<std::string>& getCommentaires() const {
        return commentaires_;
    }

    void afficher() const {
        std::cout << "Reservation " << offre_->getNom()
            << ", prix total ($ CA): " << offre_->getPrix() << "." << std::endl;
        for (const auto& commentaire : commentaires_) {
            std::cout << "  Commentaire: " << commentaire << std::endl;
        }
    }
    std::shared_ptr<Offre> getOffre() const {
        return offre_;
    }
    const std::string& getDate() const {
        return date_;
    }
};



// Classe Journee
class Journee {
private:
    std::string date_;
    std::vector<std::shared_ptr<Reservation>> reservations_;

public:
    Journee(const std::string& date) : date_(date) {};
    const std::vector<std::shared_ptr<Reservation>>& getReservations() const {
        return reservations_;
    }

    void ajouterReservation(std::shared_ptr<Reservation> reservation) {
        if (!reservation || !reservation->getOffre()) {
            std::cerr << "Erreur : R servation invalide ou offre inexistante." << std::endl;
            return;
        }
        reservations_.push_back(reservation);
    }

    const std::string& getDate() const {
        return date_;
    }

    void afficher(const std::string& voyageNom) const {
        std::cout << "  Journee " << date_ << ":" << std::endl;
        for (const auto& reservation : reservations_) {
            reservation->afficher();
        }
    }
    void ajouterSousReservation(const std::string& date, const std::string& nomSousReservation) {
        auto it = std::find_if(reservations_.begin(), reservations_.end(),
            [&date](const auto& res) { return res->getDate() == date; });
        if (it != reservations_.end()) {
            (*it)->ajouterCommentaire("Reservation " + nomSousReservation);
            std::cout << "Sous-reservation " << nomSousReservation << " ajout e pour la date " << date << "." << std::endl;
        }
        else {
            std::cerr << "Erreur : Aucune r servation trouv e pour la date " << date << "." << std::endl;
        }
    }

    void annulerReservation(const std::string& nomOffre) {
        auto it = std::remove_if(reservations_.begin(), reservations_.end(),
            [&nomOffre](const auto& res) { return res->getOffre()->getNom() == nomOffre; });
        if (it != reservations_.end()) {
            reservations_.erase(it, reservations_.end());
            std::cout << "Reservation " << nomOffre << " annul e." << std::endl;
        }
        else {
            std::cerr << "Reservation " << nomOffre << " introuvable." << std::endl;
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

// Classe Segment
class Segment {
private:
    std::string nom_;
    std::vector<std::shared_ptr<Journee>> journees_;

public:
    Segment(const std::string& nom) : nom_(nom) {};
    const std::vector<std::shared_ptr<Journee>>& getJournees() const {
        return journees_;
    }
    const std::string& getNom() const {
        return nom_;
    }

    void ajouterJournee(std::shared_ptr<Journee> journee) {
        if (!journee) {
            std::cerr << "Erreur : Journ e invalide." << std::endl;
            return;
        }
        journees_.push_back(journee);
    }


    void afficher(const std::string& voyageNom) const {
        std::cout << "  Segment " << nom_ << " cree dans le " << voyageNom << "!" << std::endl;
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
// Classe Voyage
class Voyage {
    std::string nom_;
    std::vector<std::shared_ptr<Segment>> segments_;

public:
    Voyage(const std::string& nom) : nom_(nom) {
        std::cout << "Voyage " << nom << " cree!" << std::endl;
    }
    void remplacerSegment(const std::string& ancienNom, std::shared_ptr<Segment> nouveauSegment) {
        auto it = std::find_if(segments_.begin(), segments_.end(), [&](const auto& segment) {
            return segment->getNom() == ancienNom;
            });

        if (it == segments_.end()) {
            std::cerr << "Erreur : Segment \"" << ancienNom << "\" introuvable." << std::endl;
            return;
        }

        *it = nouveauSegment;
        std::cout << "Segment \"" << ancienNom << "\" remplac  par \"" << nouveauSegment->getNom() << "\" !" << std::endl;
    }

    void setNom(const std::string& nouveauNom) {
        nom_ = nouveauNom;
    }


    void ajouterSegment(std::shared_ptr<Segment> segment) {
        segments_.push_back(segment);
    }

    void afficher() const {
        std::cout << "Voyage " << nom_ << ":" << std::endl;
        for (const auto& segment : segments_) {
            segment->afficher(nom_);
        }
    }


    const std::string& getNom() const {
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
    std::shared_ptr<Journee> trouverJournee(const std::string& date) const {
        for (const auto& segment : segments_) {
            for (const auto& journee : segment->getJournees()) {
                if (journee->getDate() == date) {
                    return journee;
                }
            }
        }
        return nullptr; // Retourne null si la journ e n'est pas trouv e
    }
    void journaliserDansFichier(const std::string& nomVoyageur) const {
        std::ofstream logFile("log" + nomVoyageur + ".txt", std::ios::app);
        if (!logFile) {
            std::cerr << "Erreur : Impossible d'ouvrir le fichier de journalisation pour " << nomVoyageur << std::endl;
            return;
        }

        logFile << "Journalisation pour le voyage : " << nom_ << std::endl;

        for (const auto& segment : segments_) {
            for (const auto& journee : segment->getJournees()) {
                logFile << "  Journee " << journee->getDate() << ":" << std::endl;
                for (const auto& reservation : journee->getReservations()) {
                    logFile << "    Reservation : " << reservation->getOffre()->getNom()
                        << ", prix : " << reservation->getOffre()->getPrix() << std::endl;
                    for (const auto& commentaire : reservation->getCommentaires()) {
                        logFile << "      Commentaire : " << commentaire << std::endl;
                    }
                }
            }
        }
    }
    const std::vector<std::shared_ptr<Segment>>& getSegments() const {
        return segments_;
    }

};

// Fonction pour afficher les frais totaux de plusieurs voyages
void afficherFraisTotaux(const std::vector<std::shared_ptr<Voyage>>& voyages) {
    for (const auto& voyage : voyages) {
        double total = voyage->calculerPrix();
        std::cout << "Total des frais pour le " << voyage->getNom()
            << " ($ CA): " << total << std::endl;
    }
}

#include <fstream>
#include <sstream>
#include <locale>
#include <codecvt>

// Fonction pour lire un fichier CSV et remplir une cat gorie
void lireCSV(const std::string& filePath, const std::string& categoryName, BaseDeDonnees& bd) {
    std::wifstream file(filePath);
    file.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier : " << filePath << std::endl;
        return;
    }

    std::wstring line;
    int lineNumber = 0;

    // V rifiez si la cat gorie existe dans la base de donn es


    // Ignore la premi re ligne (en-t te)
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

                    auto offre = std::make_shared<OffreSimple>(
                        std::string(name.begin(), name.end()), price);
                    bd.ajouterOffre("Excursion", offre);

                }
                catch (const std::exception& e) {
                    std::cerr << "Erreur lors du traitement de la ligne " << lineNumber << ": " << e.what() << std::endl;
                }
            }
            else {
                std::cerr << "Format invalide dans le fichier : " << filePath << ", ligne : " << lineNumber << std::endl;
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

                    auto offre = std::make_shared<OffreSimple>(
                        std::string(name.begin(), name.end()), price);
                    bd.ajouterOffre("Transport", offre);

                }
                catch (const std::exception& e) {
                    std::cerr << "Erreur lors du traitement de la ligne " << lineNumber << ": " << e.what() << std::endl;
                }
            }
            else {
                std::cerr << "Format invalide dans le fichier : " << filePath << ", ligne : " << lineNumber << std::endl;
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

                    auto offre = std::make_shared<OffreSimple>(
                        std::string(name.begin(), name.end()), price);
                    bd.ajouterOffre("Hebergement", offre);

                }
                catch (const std::exception& e) {
                    std::cerr << "Erreur lors du traitement de la ligne " << lineNumber << ": " << e.what() << std::endl;
                }
            }
            else {
                std::cerr << "Format invalide dans le fichier : " << filePath << ", ligne : " << lineNumber << std::endl;
            }
        }
    }

    file.close();
}


// Fonction principale pour ex cuter le syst me MiniVoyage
int main() {
    std::cout << "Objet BDOR cree!" << std::endl;
    BaseDeDonnees bdor;

    // Ajout des cat gories
    bdor.ajouterCategorie("Transport");
    bdor.ajouterCategorie("Hebergement");
    bdor.ajouterCategorie("Excursion");
    // Lecture des fichiers CSV
    lireCSV("MV_offres_de_reservation.csv", "Transport", bdor);
    lireCSV("H bergement.csv", "Hebergement", bdor);
    lireCSV("Excursions.csv", "Excursion", bdor);
    auto voyageDora = std::make_shared<Voyage>("Voyage de Dora");
    // Segment "France 1 re partie"
    auto segmentFrance1 = std::make_shared<Segment>("France 1 re partie");
    auto journeeFrance1_1 = std::make_shared<Journee>("2024-10-26");
    journeeFrance1_1->ajouterReservation(std::make_shared<Reservation>(
        bdor.trouverOffre("Transport", "Air Canada AC 870 2024-10-26"), "2024-10-26"));
    segmentFrance1->ajouterJournee(journeeFrance1_1);

    auto journeeFrance1_2 = std::make_shared<Journee>("2024-10-27");
    journeeFrance1_2->ajouterReservation(std::make_shared<Reservation>(
        bdor.trouverOffre("Hebergement", "Hotel Stella"), "2024-10-27"));
    segmentFrance1->ajouterJournee(journeeFrance1_2);

    auto journeeFrance1_3 = std::make_shared<Journee>("2024-10-28");
    journeeFrance1_3->ajouterReservation(std::make_shared<Reservation>(
        bdor.trouverOffre("Excursion", "Paris D ner-croisi re sur la Seine by Bateaux Mouches"), "2024-10-28"));
    journeeFrance1_3->ajouterReservation(std::make_shared<Reservation>(
        bdor.trouverOffre("Hebergement", "Hotel Stella"), "2024-10-28"));
    segmentFrance1->ajouterJournee(journeeFrance1_3);

    voyageDora->ajouterSegment(segmentFrance1);
    auto segmentPortugal = std::make_shared<Segment>("Portugal");
    auto journeePortugal_1 = std::make_shared<Journee>("2024-10-29");
    journeePortugal_1->ajouterReservation(std::make_shared<Reservation>(
        bdor.trouverOffre("Transport", "TAP Air Portugal TP 0441 2024-10-29"), "2024-10-29"));
    journeePortugal_1->ajouterReservation(std::make_shared<Reservation>(
        bdor.trouverOffre("Hebergement", "H tel Roma"), "2024-10-29"));
    segmentPortugal->ajouterJournee(journeePortugal_1);

    voyageDora->ajouterSegment(segmentPortugal);
    auto voyageDiego = std::make_shared<Voyage>(*voyageDora);
    // Modifiez les segments ici pour remplacer "Portugal" par "Espagne".

    auto voyageAlicia = std::make_shared<Voyage>(*voyageDiego);

    afficherFraisTotaux({ voyageDora, voyageDiego, voyageAlicia });
    std::cout << "debut de tp5" << std::endl;
    // Add restaurant reservation for Hotel Stella on 2024-10-27
    auto journee27 = voyageDora->trouverJournee("2024-10-27");
    if (journee27) {
        journee27->ajouterSousReservation("2024-10-27", "Restaurant de l'h tel Stella pour le 27 octobre 2024   19h");
    }

    // Add restaurant reservation for Hotel Stella on 2024-10-31 and cancel it
    auto journee31 = voyageDora->trouverJournee("2024-10-31");
    if (journee31) {
        journee31->ajouterSousReservation("2024-10-31", "Restaurant de l'h tel Stella pour le 31 octobre 2024   19h");
        journee31->annulerReservation("Restaurant de l'h tel Stella pour le 31 octobre 2024   19h");
    }

    // Add comments to certain reservations
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

    // Apply inflation (3% for Hebergement, 2% for others)
    bdor.appliquerIndexation(3.0, 2.0);

    // Log reservation details
    voyageDora->journaliserDansFichier("Dora");
    voyageDiego->journaliserDansFichier("Diego");
    voyageAlicia->journaliserDansFichier("Alicia");

    // Output voyage details
    std::cout << "--- Debut de la sortie du TP5\n\n";
    voyageDora->afficher();
    voyageDiego->afficher();
    voyageAlicia->afficher();

    // Output total number of offers
    std::cout << "\nTotal du nombre d'offres de r servations dans la BDOR: "
        << bdor.getNombreTotalOffres() << "." << std::endl;



    return 0;
}







