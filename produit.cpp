#include "produit.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QTableView>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QMessageBox>



Produit::Produit(int m, QString n, QString t, float q, QString e)
{
    this->MATRICULE_P = m;
    this->NOM_P = n;
    this->TYPE_P = t;
    this->QUANTITE_P = q;
    this->ETAT_P = e;
}

// Function to add a Produit to the database
bool Produit::ajouter() {
    QSqlQuery query;
    query.prepare("INSERT INTO SYSTEM.PRODUITS (MATRICULE_P, NOM_P, TYPE_P, QUANTITE_P,ETAT_P) "
                  "VALUES (:matricule_p, :nom_p, :type_p, :quantite_p, :etat_p)");
    query.bindValue(":matricule_p", MATRICULE_P);
    query.bindValue(":nom_p", NOM_P);
    query.bindValue(":type_p", TYPE_P);
    query.bindValue(":quantite_p", QUANTITE_P);
    query.bindValue(":etat_p", ETAT_P);


    // Exécution de la requête
    if (query.exec()) {
        return true; // Insertion réussie
    } else {
        qDebug() << "Erreur lors de l'insertion : " << query.lastError().text();
        return false; // Échec de l'insertion
    }
}


// Function to display all Produit records from the database

QSqlQueryModel *Produit::afficher()
{
    QSqlQueryModel * model= new QSqlQueryModel();

    model->setQuery("SELECT * FROM SYSTEM.PRODUITS");
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("MATRICULE_P"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("NOM_P"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("TYPE_P"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("QUANTITE_P"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("ETAT_P"));


    return model;
}
// Function to delete a Produit record from the database by ID

bool Produit::supprimer(int m) {
    QSqlQuery query;

    // Vérification de l'existence du Produit avant la suppression
    query.prepare("SELECT COUNT(*) FROM SYSTEM.PRODUITS WHERE MATRICULE_P = :MATRICULE");
    query.bindValue(":MATRICULE", m);
    if (!query.exec() || (query.first() && query.value(0).toInt() == 0)) {
        qDebug() << "Le Produit avec MATRICULE " << m << " n'existe pas.";
        return false;
    }

    // Suppression de l'enregistrement dans la table correcte
    query.prepare("DELETE FROM SYSTEM.PRODUITS WHERE MATRICULE_P = :MATRICULE");
    query.bindValue(":MATRICULE", m);
    if (query.exec()) {
        return true;
    } else {
        qDebug() << "Erreur lors de la suppression : " << query.lastError();
        return false;
    }
}

bool Produit::modifier() {
    QSqlQuery query;

    // Préparation de la requête SQL pour la modification
    query.prepare("UPDATE SYSTEM.PRODUITS SET QUANTITE_P = :quantite, NOM_P = :nom, TYPE_P = :type "
                  "WHERE MATRICULE_P = :matricule_p");

    // Liaison des paramètres aux valeurs actuelles de l'objet
    query.bindValue(":quantite", QUANTITE_P);
    query.bindValue(":nom", NOM_P );
    query.bindValue(":type", TYPE_P);
    query.bindValue(":matricule_p", MATRICULE_P);

    // Exécution de la requête
    if (query.exec()) {
        return true; // Modification réussie
    } else {
        qDebug() << "Erreur lors de la modification : " << query.lastError();
        return false; // Échec de la modification
    }
}

QSqlQueryModel* Produit::tri_nom()
{
    // Crée un modèle de requête SQL pour stocker les résultats
    QSqlQueryModel *model = new QSqlQueryModel();

    // Prépare la requête SQL pour sélectionner tous les partenaires, triés par NOM_P dans un ordre alphabétique croissant
    model->setQuery("SELECT * FROM SYSTEM.PRODUITS ORDER BY NOM_P ASC");

    // Vérifie si la requête a été exécutée correctement
    if (model->lastError().isValid()) {
        qDebug() << "Erreur lors de la requête SQL : " << model->lastError().text();
        return nullptr;
    }

    // Définition des en-têtes de colonnes
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("MATRICULE"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("NOM"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("TYPE"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("QUANTITE"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("ETAT"));

    return model;
}

QSqlQueryModel* Produit::tri_nom_decroissant()
{
    // Crée un modèle de requête SQL pour stocker les résultats
    QSqlQueryModel *model = new QSqlQueryModel();

    // Prépare la requête SQL pour sélectionner tous les produits, triés par NOM_P dans un ordre décroissant (Z-A)
    model->setQuery("SELECT * FROM SYSTEM.PRODUITS ORDER BY NOM_P DESC");

    // Vérifie si la requête a été exécutée correctement
    if (model->lastError().isValid()) {
        qDebug() << "Erreur lors de la requête SQL : " << model->lastError().text();
        return nullptr;
    }

    // Définition des en-têtes de colonnes
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("MATRICULE"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("NOM"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("TYPE"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("QUANTITE"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("ETAT"));

    return model;
}

QSqlQueryModel* Produit::recherche_par_type(QString t)
{
    // Crée un modèle de requête SQL pour stocker les résultats
    QSqlQueryModel *model = new QSqlQueryModel();

    // Prépare la requête SQL pour rechercher un produit par son TYPE
    QString queryStr = "SELECT * FROM SYSTEM.PRODUITS WHERE TYPE_P = :type";
    QSqlQuery query;
    query.prepare(queryStr);
    query.bindValue(":type", t); // Lier le type (de type QString) pour la recherche

    // Exécute la requête
    if (query.exec()) {
        model->setQuery(std::move(query)); // Charger les résultats dans le modèle
    } else {
        qDebug() << "Erreur lors de la requête SQL : " << query.lastError().text();
        delete model; // Nettoyer pour éviter une fuite de mémoire
        return nullptr; // Retourner null en cas d'erreur
    }

    // Définir les en-têtes de colonnes
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("MATRICULE"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("NOM"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("TYPE"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("QUANTITE"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("ETAT"));

    return model;
}

