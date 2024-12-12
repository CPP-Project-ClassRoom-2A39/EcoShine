#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "ui_mainwindow.h"
#include "employe.h"
#include <QMainWindow>
#include <QTableView>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QMap>
#include <QMessageBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QtCharts/QChartView>
#include <QTableWidgetItem>
#include "smartBin.h"
#include <QSerialPort>
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

    void on_pushButton_ajouter_e_clicked(); // Add this line
    void on_pushButton_supprimer_e_clicked();
    void on_pushButton_modifier_e_clicked();
    void on_pushButton_recherche_e_clicked();
    void on_pushButton_exporter_e_clicked();
    void on_pushButton_trier_e_clicked();
    void afficherStatistiques();
    void on_pushButton_afficher_clicked();
    void on_pushButton_prime_clicked();
    void on_pushButton_employe_du_mois_clicked();
    void on_pushButton_smart_bin_clicked();
    void readArduinoData();

private:
    Ui::MainWindow *ui;
    QSerialPort *arduino;
    QString arduinoBuffer;
    employe *monEmploye;
    employe employeObj;
    employe E;
    QSqlQueryModel *model;
    void colorerDates();
    SmartBin *smartBin;

};

#endif // MAINWINDOW_H
