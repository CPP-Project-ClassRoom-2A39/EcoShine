#ifndef PARTENAIRE_H
#define PARTENAIRE_H
#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>

class Partenaire
{
    int ID_PA;
    QString NOM_PA;
    QString ADRESSE_PA;
    int CONTACT_PA;
    QString DATE_PA;

public:
    // Constructeurs
    Partenaire() {}
    Partenaire(int id, QString nom, QString adresse, int contact, QString date);
    //Partenaire(int, QString, QString, int, QString);
    //getters
    QString getnom(){return NOM_PA;}
    QString getadresse(){return ADRESSE_PA;}
    QString getdate(){return DATE_PA;}
    int getid(){return ID_PA;}
    int getcontact(){return CONTACT_PA;}

    //setters
    void setNom(QString n){NOM_PA=n;}
    void setid(int id){ID_PA=id;}
    void setcontact(int c){CONTACT_PA=c;}
    void setadresse(QString adr){ ADRESSE_PA=adr;}
    void setdate(QString d){ DATE_PA=d;}

    bool ajouter();
    QSqlQueryModel *afficher();
    bool supprimer(int);
    bool modifier();
    QSqlQueryModel* tri_nom();
    QSqlQueryModel* tri_nom_decroissant();
    QSqlQueryModel* recherche_par_id(int id);

};

#endif
