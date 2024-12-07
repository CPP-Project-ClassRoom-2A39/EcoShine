#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "ui_mainwindow.h"
#include "partenaire.h"
#include <QMainWindow>
#include <QTableView>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QDate>



namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void on_pushButton_ajouter_clicked();
    void on_pushButton_supprimer_4_clicked();
    void on_pushButton_modifier_clicked();
    void on_pushButton_afficher_clicked();
    void on_pushButton_afficher_tri_clicked();
    void on_pushButton_afficher_tri2_clicked();
    void on_pushButton_recherche_clicked();
    void exporterVersPDF();
    void afficherStatistiques();
    void mettreAJourCalendrier();
    void afficherPartenairesPourDate(const QDate &date);
    void verifierLivraisonDemain();

    void on_pushButton_partenaire_clicked();
    void on_pushButton_acc_clicked();



    void on_pushButton_serac_clicked();

    void on_pushButton_serp_clicked();

private:
    Ui::MainWindow *ui;
    Partenaire *monPartenaire;
    Partenaire partenaireObj;
    Partenaire PartenaireTemp;
    QSqlQueryModel *model;
    // Déclare un objet QSystemTrayIcon pour gérer l'icône dans la barre système
    QSystemTrayIcon *trayIcon;
    // Déclare un menu contextuel pour l'icône de la barre système
    QMenu *trayIconMenu;
    // Déclare les actions pour le menu contextuel
    QAction *exitAction;
    QTimer *timer;



};
#endif // MAINWINDOW_H
