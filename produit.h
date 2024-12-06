#ifndef PRODUIT_H
#define PRODUIT_H
#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>





class Produit
{
    int MATRICULE_P;
    QString NOM_P;
    QString TYPE_P;
    float QUANTITE_P;
    QString ETAT_P;


public:
    // Constructeurs
    Produit() {}
    Produit(int, QString, QString, float, QString);
    //getters
    QString getNom(){return NOM_P;}
    QString getType(){return TYPE_P;}
    int getMatricule(){return MATRICULE_P;}
    float getQuantite(){return QUANTITE_P;}

    //setters
    void setNom(QString n){NOM_P=n;}
    void setMatricule(int m){MATRICULE_P=m;}
    void setQuantite(float q){QUANTITE_P=q;}
    void setType(QString t){ TYPE_P=t;}

    bool ajouter();
    QSqlQueryModel *afficher();
    bool supprimer(int);
    bool modifier();
    QSqlQueryModel *tri_nom();
    QSqlQueryModel *tri_nom_decroissant();
    QSqlQueryModel *recherche_par_type(QString t);


};
#endif // PRODUIT_H


