#ifndef EMPLOYE_H
#define EMPLOYE_H
#include <QString>
#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QSqlQueryModel>
class employe
{
    public:
             // Constructeur par dÃ©faut
             employe();

// Constructeur paramÃ©trÃ©
employe(int id, QString nom, QString prenom,QString poste, float salaire, int heuresTravaillees );

// MÃ©thodes pour gÃ©rer les employe

bool ajouter();
QSqlQueryModel *afficher();
bool supprimer(int);
bool modifier();
QSqlQueryModel *afficherTrieParNom();
//QSqlQueryModel* afficherTrieParTauxDeFidelite();



// Getters et Setters
int getId() const{return ID_E;}
void setId(int id){ID_E=id;}

QString getNom() const{return NOM_E;}
void setNom(const QString& nom){NOM_E=nom;}

QString getPrenom() const{return PRENOM_E;}
void setPrenom(const QString& prenom){PRENOM_E=prenom;}

QString getPoste() const{return POSTE_E;}
void setPoste(const QString& poste){POSTE_E=poste;}

float getSalaire() const{return SALAIRE_E;}
void setSalaire(float salaire){SALAIRE_E=salaire;}

int getH() const{return HEURES_E;}
void setH(int heuresTravaillees){HEURES_E=heuresTravaillees;}
private:
int ID_E;
QString NOM_E;
QString PRENOM_E;
QString POSTE_E;
float SALAIRE_E;
int HEURES_E;
};


#endif // EMPLOYE_H
