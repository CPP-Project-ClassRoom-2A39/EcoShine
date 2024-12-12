#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "ui_mainwindow.h"
#include "partenaire.h"
#include <QMainWindow>
#include "produit.h"
#include <QTableView>
#include <QDebug>
#include <QMainWindow>

#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>

#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>

#include <QSerialPort>

#include <QMainWindow>
#include <QSqlDatabase>

//partenaire:
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
    void checkStock();
    void onButtonOuiClicked();
    void on_buttonEnvoyer_clicked();
    void onButtonNonClicked();
    void toggleBuzzer();
    void on_pushButton_13_clicked();
    void on_pushButton_10_clicked();
    void readFromArduino();

    void changeProductState(const QString& newState);
    void updateProductState(const QString& newState);
    void resetProductStateToNormal();
//partenaire:
    void on_pushButton_ajouterpartenaire_clicked();
    void on_pushButton_afficherpartenaire_clicked();
    void on_pushButton_supprimerpartenaire_clicked();
    void on_pushButton_modifierpartenaire_clicked();
    void on_pushButton_afficher_tripartenaire_clicked();
    void on_pushButton_afficher_tri2partenaire_clicked();
    void on_pushButton_recherchepartenaire_clicked();
    void exporterVersPDFpartenaire();
    void afficherStatistiquespartenaire();
    void mettreAJourCalendrier();
    void afficherPartenairesPourDate(const QDate &date);
    void verifierLivraisonDemain();





    void on_pushButton_9_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_37_clicked();

    void on_pushButton_40_clicked();

    void on_pushButton_42_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_16_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_18_clicked();

    void on_pushButton_43_clicked();

    void on_pushButton_44_clicked();

    void on_pushButton_45_clicked();

    void on_pushButton_53_clicked();

    void on_pushButton_54_clicked();

    void on_pushButton_55_clicked();

    void on_pushButton_50_clicked();

    void on_pushButton_51_clicked();

    void on_pushButton_52_clicked();

    void on_pushButton_58_clicked();

    void on_pushButton_59_clicked();

    void on_pushButton_60_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_clicked();

    void on_pushButton_38_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_46_clicked();

    void on_pushButton_47_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_39_clicked();

    void on_pushButton_15_clicked();

    void on_pushButton_48_clicked();

    void on_pushButton_49_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_41_clicked();

    void on_pushButton_17_clicked();

    void on_pushButton_56_clicked();

    void on_pushButton_57_clicked();

private:
    Ui::MainWindow *ui;
    Produit *monProduit;
    Produit ProduitObj;
    Produit ProduitTemp;
    QSqlQueryModel *model;             // Interface utilisateur générée par Qt Designer
    QSerialPort *serialPort;
    QSqlDatabase db;
    //partenaire:
    Partenaire *monPartenaire;
    Partenaire partenaireObj;
    Partenaire PartenaireTemp;
    // Déclare un objet QSystemTrayIcon pour gérer l'icône dans la barre système
    QSystemTrayIcon *trayIcon;
    // Déclare un menu contextuel pour l'icône de la barre système
    QMenu *trayIconMenu;
    // Déclare les actions pour le menu contextuel
    QAction *exitAction;
    QTimer *timer;
};
#endif // MAINWINDOW_H
