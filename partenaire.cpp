#include "partenaire.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QTableView>
#include <QSqlQueryModel>
#include <QVariant>

Partenaire::Partenaire(int id, QString nom, QString adresse, int contact , QString date )
{
    this->ID_PA=id;
    this->NOM_PA=nom;
    this->ADRESSE_PA=adresse;
    this->CONTACT_PA=contact;
    this->DATE_PA=date;
}

// Function to add a Partenaire to the database
bool Partenaire::ajouter() {
    QSqlQuery query;
    query.prepare("INSERT INTO SYSTEM.PARTENAIRES (ID_PA, NOM_PA, ADRESSE_PA, CONTACT_PA,DATE_PA ) "
                  "VALUES (:id_pa, :nom_pa, :adresse_pa, :contact_pa, :date_pa)");
    query.bindValue(":id_pa", ID_PA);
    query.bindValue(":adresse_pa", ADRESSE_PA);
    query.bindValue(":nom_pa", NOM_PA);
    query.bindValue(":contact_pa", CONTACT_PA);
    query.bindValue(":date_pa", DATE_PA);
    // Exécution de la requête
    if (query.exec()) {

        return true; // Insertion réussie
    } else {
        qDebug() << "Erreur lors de l'insertion : " << query.lastError();
        return false; // Échec de l'insertion
    }
}

// Function to display all Partenaire records from the database

QSqlQueryModel *Partenaire::afficher()
{
    QSqlQueryModel * model= new QSqlQueryModel();

    model->setQuery("SELECT * FROM SYSTEM.PARTENAIRES");
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID_PA"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("NOM_PA"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("ADRESSE_PA"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("CONTACT_PA"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("DATE_PA"));


    return model;
}
// Function to delete a Partenaire record from the database by ID

bool Partenaire::supprimer(int id) {
    QSqlQuery query;

    // Vérification de l'existence du partenaire avant la suppression
    query.prepare("SELECT COUNT(*) FROM SYSTEM.PARTENAIRES WHERE ID_PA = :ID");
    query.bindValue(":ID", id);
    if (!query.exec() || (query.first() && query.value(0).toInt() == 0)) {
        qDebug() << "Le partenaire avec ID " << id << " n'existe pas.";
        return false;
    }

    // Suppression de l'enregistrement dans la table correcte
    query.prepare("DELETE FROM SYSTEM.PARTENAIRES WHERE ID_PA = :ID");
    query.bindValue(":ID", id);
    if (query.exec()) {
        return true;
    } else {
        qDebug() << "Erreur lors de la suppression : " << query.lastError();
        return false;
    }
}

bool Partenaire::modifier() {
    QSqlQuery query;

    // Vérifier si l'ID existe dans la base de données avant de modifier
    query.prepare("SELECT COUNT(*) FROM SYSTEM.PARTENAIRES WHERE ID_PA = :id");
    query.bindValue(":id", ID_PA);

    if (!query.exec()) {
        qDebug() << "Erreur lors de la vérification de l'ID : " << query.lastError();
        return false; // Erreur lors de la requête SELECT
    }

    // Récupérer le résultat de la requête
    query.next();
    int count = query.value(0).toInt();

    // Si l'ID n'existe pas, retourner false
    if (count == 0) {
        qDebug() << "Aucun partenaire trouvé avec l'ID : " << ID_PA;
        return false; // L'ID n'existe pas, modification impossible
    }

    // Préparation de la requête SQL pour la modification
    query.prepare("UPDATE SYSTEM.PARTENAIRES SET NOM_PA = :nom, ADRESSE_PA = :adresse, CONTACT_PA = :contact, DATE_PA = :date "
                  "WHERE ID_PA = :id");

    // Liaison des paramètres aux valeurs actuelles de l'objet
    query.bindValue(":nom", NOM_PA);
    query.bindValue(":adresse", ADRESSE_PA);
    query.bindValue(":contact", CONTACT_PA);
    query.bindValue(":id", ID_PA);
    query.bindValue(":date", DATE_PA);

    // Exécution de la requête
    if (query.exec()) {
        return true; // Modification réussie
    } else {
        qDebug() << "Erreur lors de la modification : " << query.lastError();
        return false; // Échec de la modification
    }
}

QSqlQueryModel* Partenaire::tri_nom()
{
    // Crée un modèle de requête SQL pour stocker les résultats
    QSqlQueryModel *model = new QSqlQueryModel();

    // Prépare la requête SQL pour sélectionner tous les partenaires, triés par NOM_PA dans un ordre alphabétique croissant
    model->setQuery("SELECT * FROM SYSTEM.PARTENAIRES ORDER BY NOM_PA ASC");

    // Vérifie si la requête a été exécutée correctement
    if (model->lastError().isValid()) {
        qDebug() << "Erreur lors de la requête SQL : " << model->lastError().text();
        return nullptr;
    }

    // Définition des en-têtes de colonnes
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("NOM"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("ADRESSE"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("CONTACT"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("DATE"));

    return model;
}

QSqlQueryModel* Partenaire::tri_nom_decroissant()
{
    // Crée un modèle de requête SQL pour stocker les résultats
    QSqlQueryModel *model = new QSqlQueryModel();

    // Prépare la requête SQL pour sélectionner tous les partenaires, triés par NOM_PA dans un ordre décroissant (Z-A)
    model->setQuery("SELECT * FROM SYSTEM.PARTENAIRES ORDER BY NOM_PA DESC");

    // Vérifie si la requête a été exécutée correctement
    if (model->lastError().isValid()) {
        qDebug() << "Erreur lors de la requête SQL : " << model->lastError().text();
        return nullptr;
    }

    // Définition des en-têtes de colonnes
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("NOM"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("ADRESSE"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("CONTACT"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("DATE"));

    return model;
}

QSqlQueryModel* Partenaire::recherche_par_id(int id)
{
    // Crée un modèle de requête SQL pour stocker les résultats
    QSqlQueryModel *model = new QSqlQueryModel();

    // Prépare la requête SQL pour rechercher un partenaire par son ID
    QString queryStr = QString("SELECT * FROM SYSTEM.PARTENAIRES WHERE ID_PA = :id");
    QSqlQuery query;
    query.prepare(queryStr);
    query.bindValue(":id", id); // Bind l'ID pour la recherche

    // Exécute la requête
    if (query.exec()) {
        model->setQuery(query); // Charge les résultats dans le modèle
    } else {
        qDebug() << "Erreur lors de la requête SQL : " << query.lastError().text();
        return nullptr; // Retourne null en cas d'erreur
    }

    // Définir les en-têtes de colonnes
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("NOM"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("ADRESSE"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("CONTACT"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("DATE"));

    return model;
}




