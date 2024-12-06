#ifndef S_H
#define S_H
#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>

class s
{
    QString TYPE_S;
    int MATRICULE_S;
    float PRIX_S;
    QString DATE_S;
public:
    s(){}
    s(QString type,int matricule,float prix,QString date);
    //getters
    QString getType(){return TYPE_S;}
    int getMatricule(){return MATRICULE_S;}
    float getPrix(){return PRIX_S;}
    QString getDate(){return DATE_S;}
    //setters
    void setType(QString n){TYPE_S=n;}
    void setMatricule(int matricule){MATRICULE_S=matricule;}
    void setPrix(float prix){PRIX_S=prix;}
    void setDate(QString date){DATE_S=date;}
    //methodes
    bool ajouter();
    QSqlQueryModel *afficher();
    QSqlQueryModel* trierParPrixcroissant();
    QSqlQueryModel* trierParPrixdecroissant();
    bool supprimer(int matricule);
    bool modifier();
    QSqlQueryModel* chercherParType(const QString& type);
    bool generateServiceStatistics();
    void sendAppointmentSms(const QString &to, int matricule);
    bool sendSms(const QString &to, const QString &message);

};

#endif // S_H
