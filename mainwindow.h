#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "ui_mainwindow.h"
#include "s.h"
#include <QTableView>
#include <QString>
#include <QPrinter>
#include <QFileDialog>
#include <QPainter>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QPushButton>
using namespace std ;
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:

    void on_pushButtonAjouter_clicked(); // Add this line
    void on_pushButtonSupprimer_clicked();
    void on_pushButtonModifier_clicked();
    void on_pushButtonAfficher_clicked();
    void on_pushButtonPDF_clicked();
    void on_pushButtonRecherche_clicked();
    void on_pushButtonTriercroissant_clicked();
    void on_pushButtonTrierdecroissant_clicked();
    void on_generatetypeButton_clicked();
    void on_pushButtonSend_clicked();
    void exporterVersPDF();
    void toggleBuzzer();
    void readData();
    //void compareDataWithEmployeeId();
    int chercher(const QString& mdp);
    void on_pushButtonpartenaire0_clicked();

private:
    QByteArray data; // variable contenant les données reçues
    Ui::MainWindow *ui;
    s ServicesObj;
    QSqlQueryModel *model;
    QString getChatbotResponse(const QString &userInput);
    QSerialPort *serialPort;
    bool doorOpen=false;
    QSqlDatabase db;
    void envoyerCommande(QSqlDatabase db, QSerialPort &arduino);

};
#endif // MAINWINDOW_H
