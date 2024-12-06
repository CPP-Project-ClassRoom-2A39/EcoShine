#include "s.h"
#include <QSqlError>
#include <QDebug>
#include <QTableView>
#include <QSqlRecord>
#include <QPushButton>
#include <QChartView>
#include <QBarSeries>
#include <QBarSet>
#include <QChart>
#include <QSqlQueryModel>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QSqlQuery>
#include <QValueAxis>
#include <QVBoxLayout>
#include <QtCharts/QChartView>
#include <QtCharts/QBarCategoryAxis>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QEventLoop>
#include <QCoreApplication>
#include <QSqlDatabase>

s::s(QString type,int matricule,float prix,QString date) {
    this->TYPE_S=type;
    this->MATRICULE_S=matricule;
    this->PRIX_S=prix;
    this->DATE_S=date;
}
bool s::ajouter() {
    QSqlQuery query;

    // Préparation de la requête SQL
    query.prepare("INSERT INTO SYSTEM.SERVICES (MATRICULE_S,TYPE_S,PRIX_S,DATE_S) "
                  "VALUES (:MATRICULE_S, :TYPE_S,:PRIX_S,:DATE_S)");

    // Liaison des paramètres
    query.bindValue(":TYPE_S",TYPE_S);
    query.bindValue(":MATRICULE_S",MATRICULE_S);
    query.bindValue(":PRIX_S", PRIX_S);
    query.bindValue(":DATE_S",DATE_S);


    // Exécution de la requête
    if (query.exec()) {

        return true; // Insertion réussie
    } else {
        qDebug() << "Erreur lors de l'insertion : " << query.lastError();
        return false; // Échec de l'insertion
    }
}

bool s::supprimer(int matricule) {
    QSqlQuery query;

    // Vérification de l'existence du services avant la suppression
    query.prepare("SELECT COUNT(*) FROM SYSTEM.SERVICES WHERE MATRICULE_S= :MATRICULE");
    query.bindValue(":MATRICULE", matricule);

    if (!query.exec() || (query.first() && query.value(0).toInt() == 0)) {
        qDebug() << "Le service avec la matricule" << matricule << " n'existe pas.";
        return false;
    }

    // Suppression de l'enregistrement
    query.prepare("DELETE FROM SERVICES WHERE MATRICULE_S = :MATRICULE");
    query.bindValue(":MATRICULE",  matricule);
    if (query.exec()) {
        //QSqlQueryModel *model =afficher();
        //tableView->setModel(model);
        return true;
    } else {
        qDebug() << "Erreur lors de la suppression : " << query.lastError();
        return false;
    }
}
QSqlQueryModel *s::afficher()
{
    QSqlQueryModel * model= new QSqlQueryModel();

    model->setQuery("SELECT * FROM Services");
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("MATRICULE_S"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("TYPE_S"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("PRIX_S"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("DATE_S"));
    return model;
}
QSqlQueryModel* s::trierParPrixcroissant()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;

    // Requête SQL pour sélectionner les services triés par prix
    query.prepare("SELECT MATRICULE_S,TYPE_S, PRIX_S, DATE_S FROM Services ORDER BY PRIX_S ASC");
    query.exec();

    model->setQuery(query);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("MATRICULE_S"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("TYPE_S"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("PRIX_S"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("DATE_S"));
    return model;
}
QSqlQueryModel* s::trierParPrixdecroissant()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;

    // Requête SQL pour sélectionner les services triés par prix de manière décroissante
    query.prepare("SELECT MATRICULE_S,TYPE_S, PRIX_S, DATE_S FROM Services ORDER BY PRIX_S DESC");
    query.exec();

    model->setQuery(query);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("MATRICULE_S"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("TYPE_S"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("PRIX_S"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("DATE_S"));

    return model;
}

bool s::modifier() {
    QSqlQuery query;

    // Vérifier si l'ID existe dans la base de données avant de modifier
    query.prepare("SELECT COUNT(*) FROM SYSTEM.SERVICES WHERE MATRICULE_S = :matricule");
    query.bindValue(":matricule", MATRICULE_S);

    if (!query.exec()) {
        qDebug() << "Erreur lors de la vérification de l'ID : " << query.lastError();
        return false; // Erreur lors de la requête SELECT
    }

    // Récupérer le résultat de la requête
    query.next();
    int count = query.value(0).toInt();

    // Si l'ID n'existe pas, retourner false
    if (count == 0) {
        qDebug() << "Aucun service trouvé avec la matricule : " <<MATRICULE_S;
        return false; // Le service, modification impossible
    }
    query.exec();
    // Préparation de la requête SQL pour la modification
    query.prepare("UPDATE SYSTEM.SERVICES SET TYPE_S = :type, MATRICULE_S = :matricule, PRIX_S = :prix, DATE_S = :date "
                  "WHERE MATRICULE_S = :matricule");


    // Liaison des paramètres aux valeurs actuelles de l'objet
    query.bindValue(":matricule", MATRICULE_S);
    query.bindValue(":prix", PRIX_S);
    query.bindValue(":type", TYPE_S);
    query.bindValue(":date", DATE_S);

    // Exécution de la requête
    if (query.exec()) {
        return true; // Modification réussie
    } else {
        qDebug() << "Erreur lors de la modification : " << query.lastError();
        return false; // Échec de la modification
    }
}
QSqlQueryModel* s::chercherParType(const QString& type) {
    QSqlQueryModel* model = new QSqlQueryModel();

    QSqlQuery query;
    query.prepare("SELECT * FROM Services WHERE TYPE_S = :type");
    query.bindValue(":type", type);

    if (query.exec()) {
        model->setQuery(query);
    } else {
        qDebug() << "Erreur de recherche par type : " << query.lastError().text();
    }

    return model;
}
bool s::generateServiceStatistics() {
    QSqlQuery query;
    if (!query.exec("SELECT TYPE_S, COUNT(*) FROM SYSTEM.SERVICES GROUP BY TYPE_S")) {
        qDebug() << "Échec de l'exécution de la requête:" << query.lastError().text();
        return false;
    }

    int totalServices = 0;
    QPieSeries *pieSeries = new QPieSeries();

    // Ajout des données dans la série circulaire
    while (query.next()) {
        QString serviceType = query.value(0).toString();
        int serviceCount = query.value(1).toInt();
        totalServices += serviceCount;

        pieSeries->append(serviceType, serviceCount); // Ajoute un segment pour chaque type de service
    }

    if (totalServices == 0) {
        qDebug() << "Aucun service trouvé pour générer le graphique.";
        return false;
    }

    // Personnalisation des segments du camembert
    for (auto slice : pieSeries->slices()) {
        slice->setLabel(QString("%1 (%2%)").arg(slice->label()).arg(slice->percentage() * 100, 0, 'f', 1)); // Ajoute le pourcentage au label
    }

    // Création et configuration du graphique
    QChart *chart = new QChart();
    chart->addSeries(pieSeries);
    chart->setTitle("Répartition des Services par Type");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Affichage du graphique
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->resize(600, 400);
    chartView->show();

    return true;
}
