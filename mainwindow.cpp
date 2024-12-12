#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"employe.h"
#include <QMessageBox>
#include <QDateTime>
#include <QCalendarWidget>
#include <QTableView>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QSqlQuery>
#include <QStandardItemModel>
#include <QSqlError>
#include <QPdfWriter>
#include <QPainter>
#include <QFileDialog>
#include <QSqlRecord>
#include <QDate>
#include <QMap>
#include <QDebug>
#include <QTextCharFormat>
#include <QSqlQueryModel>
#include "smartBin.h"
#include <QSerialPortInfo>



employe E;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),smartBin(new SmartBin(this)) {

{
    ui->setupUi(this);
    monEmploye = new employe;
    smartBin = new SmartBin(this);
    ui->tableView_e->setModel(E.afficher());
    connect(ui->pushButton_statistique_e, &QPushButton::clicked, this, &MainWindow::afficherStatistiques);
    arduino = new QSerialPort(this);
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        if (info.vendorIdentifier() == 0x2341 && info.productIdentifier() == 0x0043) { // Remplacez par les IDs de votre Arduino
            arduino->setPortName(info.portName());
        }
    }

    if (arduino->open(QIODevice::ReadOnly)) {
        arduino->setBaudRate(QSerialPort::Baud9600);
        connect(arduino, &QSerialPort::readyRead, this, &MainWindow::readArduinoData);
    } else {
        QMessageBox::critical(this, "Erreur", "Impossible de se connecter à Arduino.");
    }
}
}

MainWindow::~MainWindow()
{
    if (arduino->isOpen()) {
        arduino->close();
    }
    delete ui;
}
void MainWindow::on_pushButton_ajouter_e_clicked()
{
    //Récupération des informations saistes dans les 3 champs
    int ID_E = ui->lineEdit_ID->text().toInt();
    QString  NOM_E = ui->lineEdit_nom->text().trimmed();
    QString PRENOM_E = ui->lineEdit_prenom->text().trimmed();
    QString POSTE_E= ui->lineEdit_poste->text().trimmed();
    float SALAIRE_E= ui->lineEdit_salaire->text().toFloat();
    int HEURES_E = ui->lineEdit_h->text().toInt();
    employe Emp(ID_E,NOM_E,PRENOM_E,POSTE_E,SALAIRE_E,HEURES_E);

    bool test=Emp.ajouter();
    if(test)
    { ui->tableView_e->setModel(E.afficher());
        QMessageBox::information(this, QObject::tr("Ajout effectué"),

                                 QObject::tr("Lemploye a été ajouté avec succès."));

    }
    else
        QMessageBox::critical(this, QObject::tr("Échec de l'ajout"),
                              QObject::tr("L'ajout du l'employe a échoué."));
}
void MainWindow::on_pushButton_supprimer_e_clicked()
{
    int idASupprimer=ui->lineEdit_id_e->text().toInt();
    if (monEmploye->supprimer(idASupprimer)) {
        ui->tableView_e->setModel(E.afficher());
        QMessageBox::information(this, "Succès", "employe supprimé avec succès");
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la suppression du employe");
    }

}
void MainWindow::on_pushButton_modifier_e_clicked()
{
    // Récupération des informations du formulaire
    int ID_E = ui->lineEdit_ID_2->text().toInt();
    QString NOM_E = ui->lineEdit_Nom2->text();
    QString PRENOM_E = ui->lineEdit_Prenom2->text();
    QString POSTE_E = ui->lineEdit_Poste2->text();
    float SALAIRE_E = ui->lineEdit_Salaire2->text().toFloat();
    int HEURES_E = ui->lineEdit_H_2->text().toInt();


    // Création d'un objet employe avec les nouvelles informations
    employe Emp(ID_E,NOM_E, PRENOM_E, POSTE_E, SALAIRE_E,HEURES_E);

    // Appel de la fonction modifier avec l'ID pour identifier l'employe à modifier
    bool resultat = Emp.modifier();

    if(resultat) {
        ui->tableView_e->setModel(E.afficher()); // Met à jour l'affichage
        QMessageBox::information(this, "Succès", "Employe modifié avec succès");
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification du l'employe");
    }
}
// Fonction d'affichage des employe
void MainWindow::on_pushButton_afficher_clicked()
{
    ui->tableView_e->setModel(monEmploye->afficher());  // Affichage des employes
}

void MainWindow::on_pushButton_recherche_e_clicked()
{
    // Récupérer la valeur saisie dans le lineEdit
    int id_a_chercher = ui->lineEdit_id_e_recherche->text().toInt();

    // Construire la requête SQL
    QSqlQuery query;
    query.prepare("SELECT * FROM SYSTEM.EMPLOYES WHERE ID_E = :id");
    query.bindValue(":id", id_a_chercher);
    query.exec();

    // Créer un nouveau modèle et l'affecter au QTableView
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(std::move(query));
    ui->tableView_e->setModel(model);

}
void MainWindow::on_pushButton_exporter_e_clicked() {
    QString fileName = QFileDialog::getSaveFileName(this, "Enregistrer en PDF", "", "PDF (*.pdf)");

    if (fileName.isEmpty()) {
        QMessageBox::information(this, "Annulation", "L'export a été annulé.");
        return;
    }

    QPdfWriter pdfWriter(fileName);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4)); // Taille A4
    pdfWriter.setPageMargins(QMarginsF(0, 0, 0, 0)); // Marges plus grandes (50px)
    QPainter painter(&pdfWriter);

    // Réglage de la police pour une meilleure lisibilité (taille de police 12)
    QFont font("Times New Roman",8);
    painter.setFont(font);

    // Récupération du modèle et des dimensions de la table
    QTableView *tableView = ui->tableView_e;
    QAbstractItemModel *model = tableView->model();
    int rowCount = model->rowCount();
    int columnCount = model->columnCount();

    // Calcul de la largeur des colonnes pour s'adapter à la largeur de la page A4
    int pageWidth = pdfWriter.width() - 60; // Largeur de la page moins les marges (100px)
    QVector<int> columnWidths(columnCount);

    // Augmenter la largeur des colonnes en modifiant le facteur de distribution
    int totalColumnWidth = pageWidth / 2; // Par exemple, diviser par 2 pour augmenter les colonnes
    for (int col = 0; col < columnCount; ++col) {
        columnWidths[col] = totalColumnWidth / columnCount; // Distribution modifiée des colonnes
    }

    // Position initiale de l’écriture
    int x = 30; // À partir de la marge de gauche
    int y = 50; // À partir du haut avec une marge

    // Dessin de l'en-tête du tableau avec une hauteur de 1000px pour les cellules
    for (int col = 0; col < columnCount; ++col) {
        QRect rect(x, y, columnWidths[col], 1000); // Hauteur des cellules fixée à 1000px
        painter.drawRect(rect); // Bordure de la cellule
        painter.drawText(rect, Qt::AlignCenter, model->headerData(col, Qt::Horizontal).toString());
        x += columnWidths[col]; // Déplacement horizontal pour la prochaine cellule
    }

    y += 1000; // Espacement pour passer à la ligne suivante avec la hauteur définie

    // Dessin des lignes de la table (lignes et colonnes agrandies)
    for (int row = 0; row < rowCount; ++row) {
        x = 50; // Réinitialisation de x pour chaque nouvelle ligne
        for (int col = 0; col < columnCount; ++col) {
            QRect rect(x, y, columnWidths[col], 1000); // Hauteur des cellules fixée à 1000px
            painter.drawRect(rect); // Bordure de la cellule
            QString cellText = model->data(model->index(row, col)).toString();
            painter.drawText(rect, Qt::AlignCenter, cellText);
            x += columnWidths[col]; // Déplacement horizontal pour la prochaine cellule
        }
        y += 1000; // Espacement pour chaque nouvelle ligne avec la hauteur définie
    }

    painter.end();
    QMessageBox::information(this, "Succès", "Le fichier PDF a été enregistré avec succès.");
}
void MainWindow::on_pushButton_trier_e_clicked() {
    ui->tableView_e->setModel(E.afficherTrieParNom());
    QMessageBox::information(this, "Succès", "Tri effectué par nom dans l'ordre croissant.");
}

void MainWindow::afficherStatistiques() {
    // Préparer la requête pour obtenir les données nécessaires
    QSqlQuery query;
    query.prepare("SELECT POSTE_E, COUNT(*) AS count FROM SYSTEM.EMPLOYES GROUP BY POSTE_E");

    // Exécuter la requête et vérifier s'il y a des résultats
    if (!query.exec()) {
        QMessageBox::critical(this, tr("Erreur"), tr("Échec de l'obtention des statistiques"));
        return;
    }

    // Stocker les données dans une map pour compter les employés par poste
    QMap<QString, int> quantitesParPoste;
    while (query.next()) {
        QString poste = query.value(0).toString();
        int count = query.value(1).toInt();
        quantitesParPoste[poste] = count;
    }

    // Vérifier qu'il y a des données pour générer les statistiques
    if (quantitesParPoste.isEmpty()) {
        QMessageBox::information(this, tr("Aucune donnée"), tr("Aucun poste d'employé disponible pour générer les statistiques."));
        return;
    }

    // Créer une série de camembert et remplir avec les données
    QPieSeries *series = new QPieSeries();
    for (auto it = quantitesParPoste.begin(); it != quantitesParPoste.end(); ++it) {
        QString poste = it.key();
        int count = it.value();
        series->append(poste, count);
    }

    // Rendre les labels visibles pour chaque part de camembert
    for (QPieSlice *slice : series->slices()) {
        slice->setLabelVisible(true);
    }

    // Créer un objet QChart et ajouter la série de camembert
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Statistiques des employés par poste");

    // Afficher le graphique dans un QChartView
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Afficher le QChartView dans une fenêtre modale
    QDialog *dialog = new QDialog(this);
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(chartView);
    dialog->setLayout(layout);
    dialog->setWindowTitle("Statistiques des postes des employés");
    dialog->resize(500, 400);
    dialog->exec();
}
void MainWindow::on_pushButton_prime_clicked()
{
    // Étape 1 : Récupérer l'ID de l'employé depuis le champ de saisie
    int employeID = ui->lineEdit_id->text().toInt();

    // Étape 2 : Récupérer les informations de l'employé depuis la base de données
    QSqlQuery query;
    query.prepare("SELECT NOM_E, PRENOM_E, HEURES_E FROM SYSTEM.EMPLOYES WHERE ID_E = :id");
    query.bindValue(":id", employeID);

    if (!query.exec()) {
        // Affichage d'un message d'erreur en cas de problème avec la requête
        ui->textBrowser->setText("Erreur : Impossible de récupérer les données de l'employé.\n" +
                                 query.lastError().text());
        return;
    }

    if (!query.next()) {
        // Aucun résultat trouvé
        ui->textBrowser->setText("Erreur : Aucun employé trouvé avec l'ID " + QString::number(employeID));
        return;
    }

    // Étape 3 : Extraire les données de l'employé
    QString nom = query.value("NOM_E").toString();
    QString prenom = query.value("PRENOM_E").toString();
    int heuresTravaillees = query.value("HEURES_E").toInt();

    // Étape 4 : Calculer les heures supplémentaires et la prime
    int heuresSupplementaires = (heuresTravaillees > 12) ? (heuresTravaillees - 12) : 0;
    double prime = heuresSupplementaires * 3.0; // 3 DT par heure supplémentaire

    // Étape 5 : Préparer le message à afficher
    QString message;
    if (prime > 0) {
        message = "L'employé " + nom + " " + prenom +
                  " (ID : " + QString::number(employeID) + ")" +
                  " a une prime de " + QString::number(prime) + " DT pour " +
                  QString::number(heuresSupplementaires) + " heures supplémentaires.";
    } else {
        message = "L'employé " + nom + " " + prenom +
                  " (ID : " + QString::number(employeID) + ")" +
                  " n'a pas d'heures supplémentaires et donc pas de prime.";
    }

    // Étape 6 : Afficher le message dans le QTextBrowser
    ui->textBrowser->setText(message);
}
void MainWindow::on_pushButton_employe_du_mois_clicked()
{
    // Étape 1 : Rechercher tous les employés avec des heures supplémentaires
    QSqlQuery query;
    query.prepare("SELECT ID_E, NOM_E, PRENOM_E, HEURES_E, "
                  "(HEURES_E - 12) AS HEURES_SUPP "
                  "FROM SYSTEM.EMPLOYES "
                  "WHERE HEURES_E > 12");

    if (!query.exec()) {
        ui->textBrowser_2->setText("Erreur : Impossible de récupérer les employés du mois.\n" +
                                 query.lastError().text());
        return;
    }

    // Étape 2 : Trouver le maximum des heures supplémentaires
    int maxHeuresSupp = 0;
    QVector<QSqlRecord> employees;

    while (query.next()) {
        QSqlRecord record = query.record();
        int heuresSupplementaires = record.value("HEURES_SUPP").toInt();
        employees.append(record); // Stocker tous les employés dans un tableau
        // Mettre à jour le maximum des heures supplémentaires
        if (heuresSupplementaires > maxHeuresSupp) {
            maxHeuresSupp = heuresSupplementaires;
        }
    }

    // Étape 3 : Préparer le message pour l'employé du mois
    QString message = "Félicitations ! Les employés du mois sont :\n\n";
    bool found = false;

    // Étape 4 : Afficher tous les employés ayant le même nombre d'heures supplémentaires
    for (const QSqlRecord& record : employees) {
        int heuresSupplementaires = record.value("HEURES_SUPP").toInt();
        if (heuresSupplementaires == maxHeuresSupp) {
            found = true;
            int id = record.value("ID_E").toInt();
            QString nom = record.value("NOM_E").toString();
            QString prenom = record.value("PRENOM_E").toString();
            int heuresTravaillees = record.value("HEURES_E").toInt();
            double prime = heuresSupplementaires * 3.0;

            message += "Nom : " + nom + "\n";
            message += "Prénom : " + prenom + "\n";
            message += "ID : " + QString::number(id) + "\n";
            message += "Heures travaillées : " + QString::number(heuresTravaillees) + "\n";
            message += "Heures supplémentaires : " + QString::number(heuresSupplementaires) + "\n";
            message += "Prime : " + QString::number(prime) + " DT\n\n";
        }
    }

    // Étape 5 : Vérifier si des employés ont été trouvés et afficher le message
    if (found) {
        ui->textBrowser_2->setText(message);
    } else {
        ui->textBrowser_2->setText("Erreur : Aucun employé trouvé avec des heures supplémentaires.");
    }
}

void MainWindow::on_pushButton_smart_bin_clicked() {
    QString binStatus = smartBin->getBinStatus();
    ui->textBrowser_3->setText(binStatus); // Affiche l'état de la poubelle dans l'interface
}
void MainWindow::readArduinoData() {
    arduinoBuffer += arduino->readAll();

    if (arduinoBuffer.contains("\n")) {
        QStringList lines = arduinoBuffer.split("\n");
        for (const QString &line : lines) {
            if (line.trimmed().startsWith("Movement Count:")) {
                QString countStr = line.split(":").last().trimmed();
                int movementCount = countStr.toInt();
                smartBin->setMovementCount(movementCount);
            }
        }
        arduinoBuffer.clear();
    }
}
