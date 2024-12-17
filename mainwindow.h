#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "ui_mainwindow.h"
#include "partenaire.h"
#include <QMainWindow>
#include "produit.h"
#include <QTableView>
#include <QDebug>


#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>

#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QSerialPort>


#include <QSqlDatabase>

//partenaire:
#include <QTimer>
#include <QSystemTrayIcon>
#include <QDate>
//service:
#include "s.h"
#include <QString>
#include <QPrinter>
#include <QFileDialog>
#include <QPainter>
#include <QSerialPortInfo>
#include <QPushButton>
#include"employe.h"
#include"smartbin.h"

// integration amine rjab
#include <QDialog>
#include <QSqlTableModel>
#include "connection.h"
#include "messagedialog.h"
#include "updateclientdialog.h"
#include <QtCharts>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChartView>

using namespace std ;

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
    //service
    void on_pushButtonajouterservice_clicked(); // Add this line
    void on_pushButtonsupprimerservice_clicked();
    void on_pushButtonafficherservice_clicked();
    void on_pushButtonTriercroissant_clicked();
    void on_pushButtonTrierdecroissant_clicked();
    void on_pushButtonmodifierservice_clicked();
    void on_statistiqueservice_clicked();
    void on_pushButtonPDFservice_clicked();
    void exporterVersPDFservice();
    void on_pushButtonSend_clicked();//CHATBOT
    void on_pushButtonRechercheservice_clicked();
    void toggleBuzzerdoorlock();




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
    //EMPLOYE
    void on_pushButton_ajouter_e_clicked();
    void on_pushButton_supprimer_e_clicked();
    void on_pushButton_modifier_e_clicked();
    void on_pushButton_recherche_e_clicked();
    void on_pushButton_exporter_e_clicked();
    void on_pushButton_trier_e_clicked();
    void afficherStatistiquesE();
    void on_pushButton_afficher_e_clicked();
    void on_pushButton_prime_clicked();
    void on_pushButton_employe_du_mois_clicked();
    void on_pushButton_smart_bin_clicked();
    void readArduinoData();

    // integration amine rjab


    void on_createButton_clicked();
    void createClient();
    void refreshClientList();
    void on_clientTableView_doubleClicked(const QModelIndex &index);
    void on_deleteButton_clicked();
    void on_searchLineEdit_textChanged(const QString &text);
    void on_pdfButton_clicked();
    void on_submitReviewButton_clicked();
    void updateStarRating(int rating);
    void onMainWindowMessageSent();
    void onPopupMessageReceived(const QString &message);


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
    //service
    s ServicesObj;
    QString getChatbotResponse(const QString &userInput);
    //EMPLOYE
    employe *monEmploye;
    employe employeObj;
    employe E;
    void colorerDates();
    SmartBin *smartBin;
    QSerialPort *arduino;
    QString arduinoBuffer;

    // integration AMine Rjab

    // Connection &connection;
    QSqlTableModel *tableModel;
    void updateStatsChart();
    QChart *statsChart;
    QChartView *chartView;
    int currentRating;
    QVector<QPushButton*> starButtons;
    MessageDialog *messageDialog;
    QTextEdit *chatHistory;
    QLineEdit *messageInput;
    QPushButton *sendButton;
    void setupMessagingTab();
    Connection cnx;


};
#endif // MAINWINDOW_H
