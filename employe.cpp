#include "employe.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QTableView>
#include "mainwindow.h"
#include <QStandardItemModel>
employe::employe(){}
employe::employe(int ID_E,QString NOM_E,QString PRENOM_E,QString POSTE_E,float SALAIRE_E,int HEURES_E)
{
    this->ID_E=ID_E;
    this->NOM_E=NOM_E;
    this->PRENOM_E=PRENOM_E;
    this->POSTE_E=POSTE_E;
    this->SALAIRE_E=SALAIRE_E;
    this->HEURES_E=HEURES_E;


}

bool employe::ajouter() {
    QSqlQuery query;

    // Préparation de la requête SQL
    query.prepare("INSERT INTO SYSTEM.EMPLOYES (ID_E, NOM_E, PRENOM_E, POSTE_E, SALAIRE_E, HEURES_E) "
                  "VALUES (:id, :nom, :prenom, :poste, :salaire, :heuresTravaillees)");

    // Liaison des paramètres
    query.bindValue(":id", ID_E);
    query.bindValue(":nom", NOM_E);
    query.bindValue(":prenom", PRENOM_E);
    query.bindValue(":poste", POSTE_E);
    query.bindValue(":salaire", SALAIRE_E);
    query.bindValue(":heuresTravaillees", HEURES_E);



    // Exécution de la requête
    if (query.exec()) {

        return true; // Insertion réussie
    } else {
        qDebug() << "Erreur lors de l'insertion : " << query.lastError();
        return false; // Échec de l'insertion
    }
}
bool employe::supprimer(int id) {
    QSqlQuery query;

    // Vérifier l'existence de l'employé avant la suppression
    query.prepare("SELECT COUNT(*) FROM EMPLOYES WHERE ID_E = :id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        qDebug() << "Erreur lors de la vérification de l'existence : " << query.lastError();
        return false;
    }

    query.first(); // Se placer sur le premier résultat
    if (query.value(0).toInt() == 0) {
        qDebug() << "L'employé avec l'ID " << id << " n'existe pas.";
        return false;
    }

    // Suppression de l'enregistrement
    query.prepare("DELETE FROM EMPLOYES WHERE ID_E = :id");
    query.bindValue(":id", id);
    if (query.exec()) {
        return true;
    } else {
        qDebug() << "Erreur lors de la suppression : " << query.lastError();
        return false;
    }
}
QSqlQueryModel *employe::afficher()
{
    QSqlQueryModel * model= new QSqlQueryModel();


    model->setQuery("SELECT * FROM SYSTEM.EMPLOYES");
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID_E"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("NOM_E"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("PRENOM_E"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("POSTE_E"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("SALAIRE_E"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("HEURES_E"));


    return model;
}
QSqlQueryModel* employe::afficherTrieParNom() {
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM EMPLOYES ORDER BY NOM_E ASC");
    return model;
}

bool employe::modifier() {
    QSqlQuery query;

    // Préparation de la requête SQL pour la modification
    query.prepare("UPDATE SYSTEM.EMPLOYES SET NOM_E = :nom, PRENOM_E = :prenom, POSTE_E = :poste, SALAIRE_E = :salaire, HEURES_E = :heuresTravaillees WHERE ID_E = :id");

    // Liaison des paramètres aux valeurs actuelles de l'objet
    query.bindValue(":nom", NOM_E);
    query.bindValue(":prenom", PRENOM_E);
    query.bindValue(":poste", POSTE_E);
    query.bindValue(":salaire", SALAIRE_E);
    query.bindValue(":id", ID_E);
    query.bindValue(":heuresTravaillees", HEURES_E);


    // Exécution de la requête
    if (query.exec()) {
        return true; // Modification réussie
    } else {
        qDebug() << "Erreur lors de la modification : " << query.lastError();
        return false; // Échec de la modification
    }
}
