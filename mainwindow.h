#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "ui_mainwindow.h"
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






private:
    Ui::MainWindow *ui;
    Produit *monProduit;
    Produit ProduitObj;
    Produit ProduitTemp;
    QSqlQueryModel *model;             // Interface utilisateur générée par Qt Designer
    QSerialPort *serialPort;
    QSqlDatabase db;
};
#endif // MAINWINDOW_H
