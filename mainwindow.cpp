#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connection.h"
#include "partenaire.h"
#include "produit.h" // Inclure la classe Produit
#include <QMessageBox>
#include <QDateTime>
#include <QCalendarWidget>
#include <QTableView>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QPrinter>
#include <QPainter>
#include <QFileDialog>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QTimer>
#include <QStandardItemModel>

#include <QtCore>
#include <QtNetwork>

#include <QSslSocket>
#include <QTextStream>

#include <QString>
#include <QSslSocket>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QStandardPaths>


#include <QDesktopServices>

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QPushButton>

#include <QCoreApplication>


// Inclure la classe Partenaire
#include <QVBoxLayout>
#include <QSystemTrayIcon>
#include <QDate>
//service
#include "qsqlerror.h"
#include <QSqlQuery>
#include <QBrush>
#include <QPen>
#include <QPdfWriter>
#include <QSerialPortInfo>
#include <QThread>
#include <QVariant>

#include"employe.h"
#include"smartbin.h"
#include"clientwindow.h"
s service;




QSqlDatabase db;


Produit P;
Partenaire P1;
employe E;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    serialPort(new QSerialPort(this))  // Initialisation de l'objet QSerialPort


{
    ui->setupUi(this);

    // Initialiser le QSystemTrayIcon
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/res/res.png"));
    trayIcon->setVisible(true);
    //EMPLOYE
    monEmploye = new employe;
    smartBin= new SmartBin(this);
    ui->tableView_e->setModel(E.afficher());
    connect(ui->pushButton_statistique_e, &QPushButton::clicked, this, &MainWindow::afficherStatistiquesE);
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


    monPartenaire = new Partenaire;
    monProduit = new Produit;
    ui->tableView_PAR->setModel(P1.afficherpartenaire());

    // Connecte le bouton d'exportation à la fonction d'exportation PDF
    connect(ui->PDFpartenaire, &QPushButton::clicked, this, &MainWindow::exporterVersPDFpartenaire);
    // Connecte le bouton statistiques à la fonction afficherStatistiques
    connect(ui->Statistiquespartenaire, &QPushButton::clicked, this, &MainWindow::afficherStatistiquespartenaire);
    //Connecter la sélection d'une date à l'affichage des partenaires
    connect(ui->calendarWidget, &QCalendarWidget::clicked, this, &MainWindow::afficherPartenairesPourDate);


    ui->tableView->setModel(P.afficher());
    // Connecte le bouton d'exportation à la fonction d'exportation PDF
    connect(ui->pushButton_exporter, &QPushButton::clicked, this, &MainWindow::exporterVersPDF);
    connect(ui->pushButton_statistique, &QPushButton::clicked, this, &MainWindow::afficherStatistiques);

    connect(ui->pushButton_ajouter_3, &QPushButton::clicked, this, &MainWindow::on_pushButton_ajouter_clicked);
    connect(ui->pushButton_modifier_3, &QPushButton::clicked, this, &MainWindow::on_pushButton_modifier_clicked);

    // Initialisation du bouton "Oui"
    connect(ui->buttonOui, &QPushButton::clicked, this, &MainWindow::onButtonOuiClicked);

    // Initialisation du bouton "Envoyer"
    connect(ui->buttonEnvoyer, &QPushButton::clicked, this, &MainWindow::on_buttonEnvoyer_clicked);

    connect(ui->buttonNon, &QPushButton::clicked, this, &MainWindow::onButtonNonClicked);




    //timer molka notification:mariem check stock
    // Créer un timer pour vérifier la quantité toutes les 5 secondes (5000 ms)
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::verifierLivraisonDemain);
    connect(timer, &QTimer::timeout, this, &MainWindow::checkStock);
    timer->start(5000);


    // Initialiser la table au démarrage
    checkStock();


    serialPort = new QSerialPort(this);
    serialPort->setPortName("COM3");  // Changez COM3 selon le port de votre Arduino
    serialPort->setBaudRate(QSerialPort::Baud115200);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);


    // Ouvrir le port série
    if (!serialPort->open(QIODevice::ReadWrite)) {
        QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir le port série.");
    }


    connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::readFromArduino);
    // Connexion du bouton "pushButton_etat" à la fonction resetProductStateToNormal
    connect(ui->pushButton_etat, &QPushButton::clicked, this, &MainWindow::resetProductStateToNormal);


    // Connecter le bouton buzzer à une fonction pour contrôler le buzzer
    connect(ui->buzzerButton, &QPushButton::clicked, this, &MainWindow::toggleBuzzer);

    connect(ui->pushButton_48, &QPushButton::clicked, this, &MainWindow::openClientWindow);




}




MainWindow::~MainWindow()
{
    if (serialPort->isOpen()) {
        serialPort->close();
    }
    delete serialPort;  // Libérer la mémoire
    delete ui;
}


void MainWindow::openClientWindow()
{
    // Create the connection object
    Connection c;
    bool test = c.createconnect();

    // Create the client window and pass the connection object
    clientwindow *cw = new clientwindow(c, this);  // 'this' makes it a modal dialog

    if (test) {
        // Set the window modality
        cw->setWindowModality(Qt::WindowModal);

        // Optional: Set window flags to make it a popup
        cw->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);

        // Show the window
        cw->show();
    } else {
        // Show a critical error message if the connection fails
        QMessageBox::critical(this, tr("Database is not open"),
                              tr("Connection failed.\n"
                                 "Click Cancel to exit."), QMessageBox::Cancel);

        // Clean up the window if connection fails
        delete cw;
    }
}
//EMPLOYE*****************************************************************************************
//ajouter un employe
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
//supprimer un employe
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
//modifier un employe
void MainWindow::on_pushButton_modifier_e_clicked()
{
    // Récupération des informations du formulaire
    int ID_E = ui->lineEdit_ID->text().toInt();
    QString NOM_E = ui->lineEdit_nom->text();
    QString PRENOM_E = ui->lineEdit_prenom->text();
    QString POSTE_E = ui->lineEdit_poste->text();
    float SALAIRE_E = ui->lineEdit_salaire->text().toFloat();
    int HEURES_E = ui->lineEdit_h->text().toInt();
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
void MainWindow::on_pushButton_afficher_e_clicked()
{
    ui->tableView_e->setModel(monEmploye->afficher());  // Affichage des employes
}
//rechercher un employe
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
//exporter en fichier PDF
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
//trier les employes
void MainWindow::on_pushButton_trier_e_clicked() {
    ui->tableView_e->setModel(E.afficherTrieParNom());
    QMessageBox::information(this, "Succès", "Tri effectué par nom dans l'ordre croissant.");
}
//les statistiques des employes
void MainWindow::afficherStatistiquesE() {
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
//calcul du prime d'un employe
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
//selection du l'employes du moins
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
//smartBin ardouino
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
void MainWindow::on_pushButton_smart_bin_clicked() {
    QString binStatus = smartBin->getBinStatus();
    ui->textBrowserb->setText(binStatus); // Affiche l'état de la poubelle dans l'interface
}
//******************************************************************************************************



void MainWindow::on_pushButton_ajouterpartenaire_clicked()
{
    // Récupération des informations saisies dans les champs
    int ID_PA = ui->id_pa->text().toInt();
    QString NOM_PA = ui->nom_pa->text();
    QString ADRESSE_PA = ui->adresse_pa->text();
    int CONTACT_PA = ui->contact_pa->text().toInt();
    QString DATE_PA = ui->date_pa->text();

    Partenaire P1(ID_PA, NOM_PA, ADRESSE_PA, CONTACT_PA,DATE_PA);  // Création de l'objet Partenaire

    bool test = P1.ajouterpartenaire();  // Appel à la méthode ajouter() de Partenaire
    if (test) {
        ui->tableView_PAR->setModel(P1.afficherpartenaire());  // Mise à jour de l'affichage
        mettreAJourCalendrier();  // Mise à jour du calendrier
        QMessageBox::information(this, QObject::tr("Ajout effectué"),
                                 QObject::tr("Le partenaire a été ajouté avec succès."));
    } else {
        QMessageBox::critical(this, QObject::tr("Échec de l'ajout"),
                              QObject::tr("L'ajout du partenaire a échoué."));
    }
}

// Fonction d'affichage des partenaires
void MainWindow::on_pushButton_afficherpartenaire_clicked()
{
    ui->tableView_PAR->setModel(monPartenaire->afficherpartenaire());  // Affichage des partenaires
}

// Fonction suppression des partenaires
void MainWindow::on_pushButton_supprimerpartenaire_clicked()
{
    int idASupprimer = ui->supprimer_id->text().toInt();  // Récupération de l'ID du partenaire
    if (monPartenaire->supprimerpartenaire(idASupprimer)) {  // Suppression du partenaire
        ui->tableView_PAR->setModel(P1.afficherpartenaire());  // Mise à jour du tableau
        mettreAJourCalendrier();  // Mise à jour du calendrier
        QMessageBox::information(this, "Succès", "Partenaire supprimé avec succès");
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la suppression du partenaire");
    }
}
// Fonction modification des partenaires
void MainWindow::on_pushButton_modifierpartenaire_clicked()
{
    // Récupération des informations saisies dans les champs
    int ID_PA = ui->id_pa->text().toInt();  // ID du partenaire
    QString NOM_PA = ui->nom_pa->text();     // Nom du partenaire
    QString ADRESSE_PA = ui->adresse_pa->text(); // Adresse du partenaire
    int CONTACT_PA = ui->contact_pa->text().toInt();  // Contact du partenaire
    QString DATE_PA = ui->date_pa->text();

    // Vérification de la validité des données
    if (NOM_PA.isEmpty() || ADRESSE_PA.isEmpty() || CONTACT_PA <= 0 || DATE_PA.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs correctement.");
        return;
    }

    // Création de l'objet Partenaire avec les nouvelles informations
    Partenaire P1(ID_PA, NOM_PA, ADRESSE_PA, CONTACT_PA,DATE_PA);  // Crée un objet avec les nouvelles valeurs

    // Appel à la méthode modifier() de Partenaire pour mettre à jour les informations dans la base de données
    bool resultat = P1.modifierpartenaire();  // Passe les valeurs à modifier()

    // Vérification du résultat de la modification
    if (resultat) {
        ui->tableView_PAR->setModel(P1.afficherpartenaire());  // Mise à jour de l'affichage dans le QTableView
        mettreAJourCalendrier();  // Mise à jour du calendrier
        QMessageBox::information(this, "Succès", "Partenaire modifié avec succès");
    } else {
        QMessageBox::critical(this, "Erreur", "Aucun partenaire trouvé avec cet ID ou échec de la modification");
    }
}
void MainWindow::on_pushButton_afficher_tripartenaire_clicked()
{
    // Crée un objet Partenaire
    Partenaire P1;

    // Récupère les partenaires triés par nom
    QSqlQueryModel *model = P1.tri_nompartenaire();

    if (model) {
        // Associe le modèle au QTableView pour l'affichage
        ui->tableView_PAR->setModel(model);
    } else {
        // Si la récupération du modèle échoue, affiche un message d'erreur
        QMessageBox::critical(this, "Erreur", "Erreur lors du chargement des partenaires.");

    }
}

void MainWindow::on_pushButton_afficher_tri2partenaire_clicked()
{
    // Crée un objet Partenaire
    Partenaire P1;

    // Récupère les partenaires triés par nom (ordre décroissant)
    QSqlQueryModel *model = P1.tri_nom_decroissantpartenaire();

    if (model) {
        // Associe le modèle au QTableView pour l'affichage
        ui->tableView_PAR->setModel(model);
    } else {
        // Si la récupération du modèle échoue, affiche un message d'erreur
        QMessageBox::critical(this, "Erreur", "Erreur lors du chargement des partenaires.");
    }
}

void MainWindow::on_pushButton_recherchepartenaire_clicked()
{
    // Récupérer l'ID saisi par l'utilisateur
    int id = ui->idRecherche->text().toInt(); // Suppose que vous avez un champ de texte avec l'ID

    // Créer un objet Partenaire pour effectuer la recherche
    Partenaire P1;

    // Appeler la fonction recherche_par_id() pour obtenir les résultats
    QSqlQueryModel* model = P1.recherche_par_id(id);

    // Si le modèle n'est pas null, afficher le résultat
    if (model) {
        if (model->rowCount() > 0) {
            // Afficher les résultats dans un QTableView
            ui->tableView_PAR->setModel(model);
        } else {
            // Si aucun résultat n'est trouvé, afficher un message
            QMessageBox::information(this, "Aucun résultat", "Aucun partenaire trouvé avec cet ID.");
        }
    } else {
        // Afficher un message d'erreur si la recherche échoue
        QMessageBox::critical(this, "Erreur", "Erreur lors de la recherche.");
    }
}

void MainWindow::exporterVersPDFpartenaire()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Enregistrer sous"), "", tr("Fichiers PDF (*.pdf)"));
    if (fileName.isEmpty()) {
        return;
    }

    if (!fileName.endsWith(".pdf", Qt::CaseInsensitive)) {
        fileName += ".pdf";
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    QPainter painter;
    if (!painter.begin(&printer)) {
        QMessageBox::critical(this, tr("Erreur"), tr("Impossible d'ouvrir le fichier PDF pour l'écriture."));
        return;
    }

    QSqlQueryModel *model = P1.afficherpartenaire();
    if (!model) {
        QMessageBox::critical(this, tr("Erreur"), tr("Aucun partenaire à exporter."));
        return;
    }



    int yPosition = 100;  // Position verticale initiale pour le titre
    const int margin = 40;  // Marge de gauche
    const int rowHeight = 300;  // Hauteur d'une ligne
    const int columnWidths[] = { 1900, 1900, 1900, 1900 ,1900};  // Largeurs des colonnes

    // Calculer la largeur totale du tableau
    int totalWidth = 0;
    for (int width : columnWidths) {
        totalWidth += width;
    }

    // Ajouter le logo avant le titre
    QImage logo("C:/logo"); // Chemin vers l'image
    if (!logo.isNull()) {
        QRect logoRect(margin, 50, 1000, 1000);
        painter.drawImage(logoRect, logo);
        yPosition += logoRect.height() + 50; // Ajuster la position après le logo
    } else {
        QMessageBox::warning(this, tr("Logo non trouvé"), tr("Le fichier logo22.png est introuvable ou invalide."));
    }

    // Centrer le titre "Liste des Partenaires"
    QFont titleFont = painter.font();
    titleFont.setBold(true);
    titleFont.setPointSize(16);
    painter.setFont(titleFont);
    painter.setPen(QColor(0, 128, 0)); // Vert
    QRect titleRect(margin, yPosition, totalWidth, rowHeight); // Utiliser la largeur totale
    painter.drawText(titleRect, Qt::AlignCenter, "Liste des Partenaires");
    yPosition += 100; // Décalage supplémentaire après le titre

    // Décalage supplémentaire du tableau
    yPosition += 450; // Ajustez cette valeur pour descendre le tableau davantage

    // Dessiner les en-têtes du tableau
    QFont headerFont = painter.font();
    headerFont.setBold(true);
    headerFont.setPointSize(12);
    painter.setFont(headerFont);

    // Changez la couleur des en-têtes en bleu
    painter.setPen(QColor(0, 0, 255)); // Bleu

    int xPosition = margin;

    // Colonnes des en-têtes
    QStringList headers = { "Identifient", "Nom", "Adresse", "Contact" ,"Date"};

    // Dessiner les bordures et textes des en-têtes
    for (int i = 0; i < headers.size(); ++i) {
        QRect rect(xPosition, yPosition, columnWidths[i], rowHeight);
        painter.drawRect(rect);  // Dessiner la bordure
        painter.drawText(rect, Qt::AlignCenter, headers[i]);  // Centrer le texte dans la cellule
        xPosition += columnWidths[i];
    }
    yPosition += rowHeight;  // Passer à la ligne suivante

    // Dessiner les données du tableau
    QFont dataFont = painter.font();
    dataFont.setBold(false);
    dataFont.setPointSize(10);
    painter.setFont(dataFont);

    painter.setPen(Qt::black); // Noir pour les données

    for (int row = 0; row < model->rowCount(); ++row) {
        xPosition = margin;

        for (int col = 0; col < model->columnCount(); ++col) {
            QString cellData = model->data(model->index(row, col)).toString();
            QRect rect(xPosition, yPosition, columnWidths[col], rowHeight);
            painter.drawRect(rect);  // Dessiner la bordure
            painter.drawText(rect, Qt::AlignCenter, cellData);  // Centrer le texte dans la cellule
            xPosition += columnWidths[col];
        }

        yPosition += rowHeight;  // Passer à la ligne suivante
    }

    QMessageBox::information(this, tr("Exportation réussie"), tr("La liste des partenaires a été exportée avec succès sous PDF."));
}

void MainWindow::afficherStatistiquespartenaire() {
    // Préparer la requête pour obtenir les données des partenaires
    QSqlQuery query;
    query.prepare("SELECT NOM_PA, COUNT(*) AS count FROM SYSTEM.PARTENAIRES GROUP BY NOM_PA");

    // Exécuter la requête et vérifier s'il y a des résultats
    if (!query.exec()) {
        QMessageBox::critical(this, tr("Erreur"), tr("Échec de l'obtention des statistiques des partenaires"));
        return;
    }

    // Stocker les données dans une map pour compter les occurrences par nom
    QMap<QString, int> quantitesParNom;
    while (query.next()) {
        QString nomPartenaire = query.value(0).toString();
        int count = query.value(1).toInt();
        quantitesParNom[nomPartenaire] = count;
    }

    // Vérifier qu'il y a des données pour générer les statistiques
    if (quantitesParNom.isEmpty()) {
        QMessageBox::information(this, tr("Aucune donnée"), tr("Aucun partenaire disponible pour générer les statistiques."));
        return;
    }

    // Créer une série de camembert et remplir avec les données
    QPieSeries *series = new QPieSeries();
    for (auto it = quantitesParNom.begin(); it != quantitesParNom.end(); ++it) {
        QString nomPartenaire = it.key();
        int count = it.value();
        series->append(nomPartenaire, count);
    }

    // Rendre les labels visibles pour chaque part de camembert
    for (QPieSlice *slice : series->slices()) {
        slice->setLabelVisible(true);
        slice->setLabel(QString("%1 (%2)").arg(slice->label()).arg(slice->value()));
    }

    // Créer un objet QChart et ajouter la série de camembert
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Statistiques des partenaires par nom");

    // Afficher le graphique dans un QChartView
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Afficher le QChartView dans une fenêtre modale
    QDialog *dialog = new QDialog(this);
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(chartView);
    dialog->setWindowTitle("Statistiques des Partenaires");
    dialog->resize(600, 400);
    dialog->exec();
}

void MainWindow::mettreAJourCalendrier() {
    // Obtenir tous les partenaires de la base de données
    QSqlQuery query("SELECT DATE_PA FROM SYSTEM.PARTENAIRES");

    // Créer une liste pour stocker les dates uniques avec partenaires
    QSet<QDate> datesAvecPartenaires;

    // Parcourir les résultats de la requête
    while (query.next()) {
        QString dateString = query.value(0).toString();
        QDate datePartenaire = QDate::fromString(dateString, "yyyy-MM-dd"); // Format de la date
        if (datePartenaire.isValid()) {
            datesAvecPartenaires.insert(datePartenaire); // Ajouter la date à la liste
        }
    }

    // Marquer les dates avec partenaires dans le calendrier
    QTextCharFormat format;
    format.setBackground(QColor("#93daea")); // Couleur pour les dates avec partenaires
    for (const QDate &date : datesAvecPartenaires) {
        ui->calendarWidget->setDateTextFormat(date, format);
    }
}

void MainWindow::afficherPartenairesPourDate(const QDate &date) {
    // Formater la date en chaîne de caractères
    QString dateStr = date.toString("yyyy-MM-dd");

    // Requête pour obtenir les partenaires pour la date sélectionnée
    QSqlQuery query;
    query.prepare("SELECT * FROM SYSTEM.PARTENAIRES WHERE DATE_PA = :date");
    query.bindValue(":date", dateStr);

    if (query.exec()) {
        // Créez un modèle pour afficher les résultats dans un QTableView
        QSqlQueryModel *model = new QSqlQueryModel();
        model->setQuery(query);
        model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
        model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom"));
        model->setHeaderData(2, Qt::Horizontal, QObject::tr("Adresse"));
        model->setHeaderData(3, Qt::Horizontal, QObject::tr("Contact"));
        model->setHeaderData(4, Qt::Horizontal, QObject::tr("Date"));

        // Afficher les résultats dans un QTableView (par exemple, tableView_4)
        ui->tableView_PAR->setModel(model);
    } else {
        QMessageBox::critical(this, "Erreur", "Erreur lors de la récupération des partenaires pour cette date.");
    }
}

void MainWindow::verifierLivraisonDemain() {
    // Récupérer la date de demain
    QDate demain = QDate::currentDate().addDays(1);
    QString dateDemain = demain.toString("yyyy-MM-dd");

    // Vérifier dans la base de données si un partenaire a une livraison demain
    QSqlQuery query;
    query.prepare("SELECT NOM_PA, CONTACT_PA, ADRESSE_PA FROM PARTENAIRES WHERE DATE_PA = :dateDemain");
    query.bindValue(":dateDemain", dateDemain);

    if (query.exec() && query.next()) {
        QString nom = query.value(0).toString();
        QString contact = query.value(1).toString();
        QString adresse = query.value(2).toString();

        // Créer et afficher la notification
        QString message = QString("Demain, vous avez une livraison du partenaire: %1. Contact: %2, Adresse: %3")
                              .arg(nom).arg(contact).arg(adresse);

        trayIcon->showMessage("Livraison prévue", message, QSystemTrayIcon::Information, 120000);
    }
}














void MainWindow::on_pushButton_ajouter_clicked()
{
    // Récupération des informations saisies dans les champs
    int MATRICULE_P = ui->matricule_p->text().toInt();
    QString NOM_P = ui->nom_p->text();
    QString TYPE_P = ui->type_p->text();
    float QUANTITE_P = ui->quantite_p->text().toFloat();
    QString ETAT_P = "normal";

    Produit P(MATRICULE_P, NOM_P, TYPE_P, QUANTITE_P,ETAT_P );  // Création de l'objet Produit

    // Appel à la méthode ajouter() de Produit
    bool test = P.ajouter();
    if (test) {
         ui->tableView->setModel(P.afficher());  // Mise à jour de l'affichage
        QMessageBox::information(this, QObject::tr("Ajout effectué"),
                                 QObject::tr("Le produit a été ajouté avec succès."));
    } else {
        QMessageBox::critical(this, QObject::tr("Échec de l'ajout"),
                              QObject::tr("L'ajout du produit a échoué."));
    }
}

void MainWindow::on_pushButton_supprimer_4_clicked()
{
    int mASupprimer = ui->matricule_p->text().toInt();  // Récupération de la matricule du produit
    if (monProduit->supprimer(mASupprimer)) {  // Suppression du produit
        ui->tableView->setModel(P.afficher());  // Mise à jour du tableau
        QMessageBox::information(this, "Succès", "Produit supprimé avec succès");
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la suppression du produit");
    }
}

void MainWindow::on_pushButton_modifier_clicked()
{
    // Récupération des informations saisies dans les champs
    int MATRICULE_P = ui->matricule_p->text().toInt();
    QString NOM_P = ui->nom_p->text();
    QString TYPE_P = ui->type_p->text();
    float QUANTITE_P = ui->quantite_p->text().toFloat();
    QString ETAT_P = "normal";


    // Création de l'objet Partenaire avec les nouvelles informations
    Produit P(MATRICULE_P, NOM_P, TYPE_P, QUANTITE_P,ETAT_P );

    bool resultat = P.modifier();  // Appel à la méthode modifier() de Produit
    if (resultat) {
        ui->tableView->setModel(P.afficher());  // Mise à jour de l'affichage
        QMessageBox::information(this, "Succès", "Produit modifié avec succès");
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification du Produit");
    }
}

// Fonction d'affichage des produits
void MainWindow::on_pushButton_afficher_clicked()
{
    ui->tableView->setModel(monProduit->afficher());  // Affichage des partenaires
}

void MainWindow::on_pushButton_afficher_tri_clicked()
{
    // Crée un objet Produit
    Produit P;

    // Récupère les partenaires triés par nom
    QSqlQueryModel *model = P.tri_nom();

    if (model) {
        // Associe le modèle au QTableView pour l'affichage
        ui->tableView->setModel(model);
    } else {
        // Si la récupération du modèle échoue, affiche un message d'erreur
        QMessageBox::critical(this, "Erreur", "Erreur lors du chargement des produits.");
    }
}

void MainWindow::on_pushButton_afficher_tri2_clicked()
{
    // Crée un objet Produit
    Produit P;

    // Récupère les produits triés par nom (ordre décroissant)
    QSqlQueryModel *model = P.tri_nom_decroissant();

    if (model) {
        // Associe le modèle au QTableView pour l'affichage
        ui->tableView->setModel(model);
    } else {
        // Si la récupération du modèle échoue, affiche un message d'erreur
        QMessageBox::critical(this, "Erreur", "Erreur lors du chargement des produits.");
    }
}

void MainWindow::on_pushButton_recherche_clicked()
{
    // Récupérer le type saisi par l'utilisateur
    QString t = ui->typeRecherche->text(); // Suppose que vous avez un champ de texte avec le type

    // Créer un objet Produit pour effectuer la recherche
    Produit P;

    // Appeler la fonction recherche_par_type() pour obtenir les résultats
    QSqlQueryModel* model = P.recherche_par_type(t);


    // Si le modèle n'est pas null, afficher le résultat
    if (model) {
        if (model->rowCount() > 0) {
            // Afficher les résultats dans un QTableView
            ui->tableView->setModel(model);
        } else {
            // Si aucun résultat n'est trouvé, afficher un message
            QMessageBox::information(this, "Aucun résultat", "Aucun produit trouvé avec ce type.");
        }
    } else {
        // Afficher un message d'erreur si la recherche échoue
        QMessageBox::critical(this, "Erreur", "Erreur lors de la recherche.");
    }
}



void MainWindow::exporterVersPDF()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Enregistrer sous"), "", tr("Fichiers PDF (*.pdf)"));
    if (fileName.isEmpty()) {
        return;
    }

    if (!fileName.endsWith(".pdf", Qt::CaseInsensitive)) {
        fileName += ".pdf";
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    QPainter painter;
    if (!painter.begin(&printer)) {
        QMessageBox::critical(this, tr("Erreur"), tr("Impossible d'ouvrir le fichier PDF pour l'écriture."));
        return;
    }

    QSqlQueryModel *model = P.afficher();
    if (!model) {
        QMessageBox::critical(this, tr("Erreur"), tr("Aucun produit à exporter."));
        return;
    }



    int yPosition = 100;  // Position verticale initiale pour le titre
    const int margin = 40;  // Marge de gauche
    const int rowHeight = 300;  // Hauteur d'une ligne
    const int columnWidths[] = { 1900, 1900, 1900, 1900, 1900 };  // Largeurs des colonnes

    // Calculer la largeur totale du tableau
    int totalWidth = 0;
    for (int width : columnWidths) {
        totalWidth += width;
    }

    // Ajouter le logo avant le titre
    QImage logo("C:/logo"); // Chemin vers l'image
    if (!logo.isNull()) {
        QRect logoRect(margin, 50, 1000, 1000);
        painter.drawImage(logoRect, logo);
        yPosition += logoRect.height() + 50; // Ajuster la position après le logo
    } else {
        QMessageBox::warning(this, tr("Logo non trouvé"), tr("Le fichier logo22.png est introuvable ou invalide."));
    }

    // Centrer le titre "Liste des Produits"
    QFont titleFont = painter.font();
    titleFont.setBold(true);
    titleFont.setPointSize(16);
    painter.setFont(titleFont);
    painter.setPen(QColor(0, 128, 0)); // Vert
    QRect titleRect(margin, yPosition, totalWidth, rowHeight); // Utiliser la largeur totale
    painter.drawText(titleRect, Qt::AlignCenter, "Liste des Produits");
    yPosition += 100; // Décalage supplémentaire après le titre

    // Décalage supplémentaire du tableau
    yPosition += 450; // Ajustez cette valeur pour descendre le tableau davantage

    // Dessiner les en-têtes du tableau
    QFont headerFont = painter.font();
    headerFont.setBold(true);
    headerFont.setPointSize(12);
    painter.setFont(headerFont);

    // Changez la couleur des en-têtes en bleu
    painter.setPen(QColor(0, 0, 255)); // Bleu

    int xPosition = margin;

    // Colonnes des en-têtes
    QStringList headers = { "Matricule", "Nom", "Type", "Quantité" ,"Etat" };

    // Dessiner les bordures et textes des en-têtes
    for (int i = 0; i < headers.size(); ++i) {
        QRect rect(xPosition, yPosition, columnWidths[i], rowHeight);
        painter.drawRect(rect);  // Dessiner la bordure
        painter.drawText(rect, Qt::AlignCenter, headers[i]);  // Centrer le texte dans la cellule
        xPosition += columnWidths[i];
    }
    yPosition += rowHeight;  // Passer à la ligne suivante

    // Dessiner les données du tableau
    QFont dataFont = painter.font();
    dataFont.setBold(false);
    dataFont.setPointSize(10);
    painter.setFont(dataFont);

    painter.setPen(Qt::black); // Noir pour les données

    for (int row = 0; row < model->rowCount(); ++row) {
        xPosition = margin;

        for (int col = 0; col < model->columnCount(); ++col) {
            QString cellData = model->data(model->index(row, col)).toString();
            QRect rect(xPosition, yPosition, columnWidths[col], rowHeight);
            painter.drawRect(rect);  // Dessiner la bordure
            painter.drawText(rect, Qt::AlignCenter, cellData);  // Centrer le texte dans la cellule
            xPosition += columnWidths[col];
        }

        yPosition += rowHeight;  // Passer à la ligne suivante
    }

    QMessageBox::information(this, tr("Exportation réussie"), tr("La liste des produits a été exportée avec succès sous PDF."));
}



void MainWindow::afficherStatistiques()
{
    // Obtenir les données du modèle
    QSqlQueryModel *model = P.afficher();
    if (!model || model->rowCount() == 0) {
        QMessageBox::information(this, tr("Aucune donnée"), tr("Aucun produit disponible pour générer les statistiques."));
        return;
    }

    // Calculer le total des quantités
    int totalQuantite = 0;
    for (int i = 0; i < model->rowCount(); ++i) {
        totalQuantite += model->data(model->index(i, 3)).toInt(); // Quantité (colonne 3)
    }

    // Créer une série de camembert
    QPieSeries *series = new QPieSeries();

    // Remplir la série avec les quantités des produits et ajouter les pourcentages
    for (int i = 0; i < model->rowCount(); ++i) {
        QString nomProduit = model->data(model->index(i, 1)).toString(); // Nom du produit (colonne 1)
        int quantite = model->data(model->index(i, 3)).toInt(); // Quantité (colonne 3)

        // Calculer le pourcentage
        double percentage = (static_cast<double>(quantite) / totalQuantite) * 100;

        // Ajouter les données à la série, avec le pourcentage dans le label
        QPieSlice *slice = series->append(nomProduit + QString(" (%1%)").arg(percentage, 0, 'f', 1), quantite);
        slice->setLabelVisible(true); // Rendre le label visible
    }

    // Créer un objet QChart pour contenir la série
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Statistiques des produits par quantité");

    // Définir la couleur du titre en bleu
    QFont titleFont = chart->titleFont();
    titleFont.setPointSize(16); // Taille de la police
    chart->setTitleFont(titleFont);
    chart->setTitleBrush(QBrush(Qt::blue)); // Définir la couleur en bleu

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);

    // Créer une vue pour afficher le graphique
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Afficher le graphique dans une nouvelle fenêtre
    QMainWindow *statsWindow = new QMainWindow(this);
    statsWindow->setCentralWidget(chartView);
    statsWindow->resize(700, 405);
    statsWindow->setWindowTitle("Statistiques des produits");
    statsWindow->show();
}

void MainWindow::checkStock()
{
    // Exécuter la requête pour récupérer les produits avec des quantités <= 2
    QSqlQuery query;
    query.prepare("SELECT MATRICULE_P, NOM_P, TYPE_P, QUANTITE_P, ETAT_P FROM SYSTEM.PRODUITS WHERE QUANTITE_P <= 2");

    if (!query.exec()) {
        qDebug() << "Erreur lors de la requête : " << query.lastError().text();
        return;
    }

    // Préparer un texte formaté pour affichage dans QTextEdit avec une largeur personnalisée
    QString message = "<table border='1' style='border-collapse: collapse; text-align: left; width: 80%;'>"
                      "<tr style='background-color: #f2f2f2;'>"
                      "<th style='padding: 8px;'>Matricule</th>"
                      "<th style='padding: 8px;'>Nom</th>"
                      "<th style='padding: 8px;'>Type</th>"
                      "<th style='padding: 8px;'>Quantité</th>"
                      "<th style='padding: 8px;'>Etat</th>"
                      "</tr>";

    bool hasLowStock = false; // Indicateur pour savoir si un produit a une quantité <= 2

    // Parcourir les résultats de la requête et les ajouter au texte
    while (query.next()) {
        int matricule = query.value(0).toInt();
        QString nom = query.value(1).toString();
        QString type = query.value(2).toString();
        int quantite = query.value(3).toInt();
        QString etat = query.value(4).toString();

        // Ajouter une ligne pour chaque produit avec stock faible
        message += QString("<tr>"
                           "<td style='padding: 8px;'>%1</td>"
                           "<td style='padding: 8px;'>%2</td>"
                           "<td style='padding: 8px;'>%3</td>"
                           "<td style='padding: 8px;'>%4</td>"
                           "<td style='color: red; padding: 8px;'>%5</td>"
                           "</tr>")
                       .arg(matricule)
                       .arg(nom)
                       .arg(type)
                       .arg(quantite)
                       .arg(etat);

        hasLowStock = true; // Il y a des produits en stock insuffisant
    }

    message += "</table>";

    // Ajouter le message de notification si des produits ont une quantité insuffisante
    if (hasLowStock) {
        message += "<p style='color: red; font-weight: bold;'>⚠️ Afin d’éviter toute rupture, souhaitez-vous passer une commande ?</p>";
    } else {
        message += "<p style='color: green;'>Tous les produits ont une quantité suffisante en stock.</p>";
    }

    // Afficher le message dans textEdit_4
    ui->textEdit_4->setHtml(message); // Utiliser setHtml pour un affichage formaté
}


void MainWindow::onButtonOuiClicked()
{
    // Message modèle à afficher
    QString message = QString("Bonjour cher partenaire,\n\n"
                              "J'espère que vous allez bien.\n\n"
                              "Je vous contacte afin de passer une commande pour le produit suivant :\n"
                              "Produit : [Veuillez saisir le produit ici]\n"
                              "Quantité : [Veuillez saisir la quantité ici]\n\n"
                              "Pourriez-vous, s'il vous plaît, me confirmer la disponibilité du produit ainsi que "
                              "le délai de livraison et les conditions tarifaires ?\n\n"
                              "Dans l'attente de votre retour, je vous remercie par avance pour votre réponse rapide.\n\n"
                              "Cordialement,");

    // Afficher le message dans QTextEdit (nommé textEdit dans ce cas)
    ui->textEdit->setText(message);  // Assurez-vous que textEdit est le nom du QTextEdit dans votre interface
}





void MainWindow::on_buttonEnvoyer_clicked()
{
    // Define the default recipient email
    QString recipientEmail = "partenairepartenaire60@gmail.com"; // Default email address

    // Retrieve the email content from QTextEdit
    QString emailContent = ui->textEdit->toPlainText();

    if (emailContent.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Le contenu de l'email est vide.");
        return;
    }

    // Connect to Gmail SMTP server
    QSslSocket socket;
    socket.connectToHost("smtp.gmail.com", 587);
    if (!socket.waitForConnected(5000)) {
        QMessageBox::critical(this, "Erreur", "Connexion au serveur SMTP échouée.");
        return;
    }

    // Wait for initial greeting (220 response)
    if (!socket.waitForReadyRead(10000)) {
        QMessageBox::critical(this, "Erreur", "Le serveur n'a pas répondu correctement au démarrage.");
        return;
    }
    QByteArray initialResponse = socket.readAll();
    qDebug() << "Initial Response: " << initialResponse;

    // Send EHLO command
    socket.write("EHLO client\r\n");
    socket.waitForBytesWritten();

    // Wait for EHLO response
    if (!socket.waitForReadyRead(10000)) {
        QMessageBox::critical(this, "Erreur", "Le serveur n'a pas répondu à EHLO dans le délai imparti.");
        return;
    }
    QByteArray ehloResponse = socket.readAll();
    qDebug() << "EHLO Response: " << ehloResponse;

    // Verify EHLO response contains "250"
    if (!ehloResponse.contains("250")) {
        QMessageBox::critical(this, "Erreur", "Le serveur n'a pas répondu correctement à EHLO.");
        return;
    }

    // Send STARTTLS command
    socket.write("STARTTLS\r\n");
    socket.waitForBytesWritten();
    if (!socket.waitForReadyRead(10000) || !socket.readAll().contains("220")) {
        QMessageBox::critical(this, "Erreur", "Erreur lors de l'activation de STARTTLS.");
        return;
    }

    // Start encrypted communication
    socket.startClientEncryption();
    if (!socket.waitForEncrypted(5000)) {
        QMessageBox::critical(this, "Erreur", "Échec de la configuration SSL.");
        return;
    }

    // Authenticate using AUTH LOGIN
    socket.write("AUTH LOGIN\r\n");
    socket.waitForBytesWritten();
    if (!socket.waitForReadyRead(10000) || !socket.readAll().contains("334")) {
        QMessageBox::critical(this, "Erreur", "Erreur lors de l'initialisation de l'authentification.");
        return;
    }

    // Encode credentials in Base64
    QByteArray username = QByteArray("amirabenmahmoud54@gmail.com").toBase64(); // Replace with your email
    QByteArray password = QByteArray("npmf vvru ebsc wtgy").toBase64();         // Replace with your app password

    // Send username
    socket.write(username + "\r\n");
    socket.waitForBytesWritten();
    if (!socket.waitForReadyRead(10000) || !socket.readAll().contains("334")) {
        QMessageBox::critical(this, "Erreur", "Erreur d'envoi du nom d'utilisateur.");
        return;
    }

    // Send password
    socket.write(password + "\r\n");
    socket.waitForBytesWritten();
    if (!socket.waitForReadyRead(10000) || !socket.readAll().contains("235")) {
        QMessageBox::critical(this, "Erreur", "Authentification SMTP échouée.");
        return;
    }

    // Send MAIL FROM command
    socket.write("MAIL FROM:<amirabenmahmoud54@gmail.com>\r\n");
    socket.waitForBytesWritten();
    if (!socket.waitForReadyRead(10000) || !socket.readAll().contains("250")) {
        QMessageBox::critical(this, "Erreur", "Erreur lors de l'envoi du MAIL FROM.");
        return;
    }

    // Send RCPT TO command
    socket.write("RCPT TO:<" + recipientEmail.toUtf8() + ">\r\n");
    socket.waitForBytesWritten();
    if (!socket.waitForReadyRead(10000) || !socket.readAll().contains("250")) {
        QMessageBox::critical(this, "Erreur", "Erreur lors de l'envoi du RCPT TO.");
        return;
    }

    // Send DATA command
    socket.write("DATA\r\n");
    socket.waitForBytesWritten();
    if (!socket.waitForReadyRead(10000) || !socket.readAll().contains("354")) {
        QMessageBox::critical(this, "Erreur", "Erreur lors de l'envoi de la commande DATA.");
        return;
    }

    // Email content
    QString emailData = "From: Produit <amirabenmahmoud54@gmail.com>\r\n"
                        "To: " + recipientEmail + "\r\n"
                                           "Subject: Sujet de votre email\r\n\r\n" +
                        emailContent + "\r\n.\r\n";

    socket.write(emailData.toUtf8());
    socket.waitForBytesWritten();
    if (!socket.waitForReadyRead(10000) || !socket.readAll().contains("250")) {
        QMessageBox::critical(this, "Erreur", "Erreur lors de l'envoi de l'email.");
        return;
    }

    // Send QUIT command
    socket.write("QUIT\r\n");
    socket.waitForBytesWritten();

    QMessageBox::information(this, "Succès", "Email envoyé avec succès !");
}



void MainWindow::onButtonNonClicked()
{
    // Message à afficher pour le bouton Non
    QString message = QString("Très bien, je note. Je reste à votre disposition si vous changez d'avis ou avez besoin d'autre chose.");

    // Afficher le message dans QTextEdit
    ui->textEdit->setText(message);  // Assurez-vous que textEdit est le nom du QTextEdit dans votre interface
}



void MainWindow::toggleBuzzer() {
    if (serialPort->isOpen()) {
        serialPort->write("T");  // Envoyer la commande 'T'
        qDebug() << "Commande envoyée : T";
    } else {
        QMessageBox::warning(this, "Erreur", "Le port série n'est pas ouvert.");
    }
}






void MainWindow::changeProductState(const QString& newState)
{
    // Exemple de mise à jour de l'état d'un produit. Vous pouvez utiliser un QString ou interagir avec une base de données.
    if (newState == "détruite") {
        // Mettre à jour l'état du produit dans l'interface ou la base de données
        qDebug() << "Le produit est maintenant dans l'état : " << newState;
        // Ici vous pouvez mettre à jour une interface, une base de données, etc.
        // Par exemple : updateDatabaseState("ETAT_E", "détruite");
    } else {
        qDebug() << "L'état du produit reste : normal";
    }
}
void MainWindow::readFromArduino()
{
    // Lire toutes les données disponibles sur le port série
    QByteArray data = serialPort->readAll();

    // Vérifier si des données ont été reçues
    if (!data.isEmpty()) {
        // Vérifier si les données ne sont pas seulement des octets nuls
        if (data == QByteArray(data.size(), '\0')) {
            qDebug() << "Données invalides (octets nuls)";
            return;
        }

        // Convertir les données en chaîne de caractères (UTF-8)
        QString receivedText = QString::fromUtf8(data);

        // Vérifier que la conversion s'est bien déroulée
        if (receivedText.isEmpty()) {
            qDebug() << "Les données reçues ne sont pas valides (chaîne vide après conversion)";
        } else {
            // Afficher les données reçues dans le debug
            qDebug() << "Received Text:" << receivedText;

            // Si le message reçu correspond à "incendie\r\n", changer l'état du produit
            if (receivedText.contains("incendie\r\n")) {
                updateProductState("détruit"); // Fonction pour changer l'état du produit
            }
        }

    } else {
        qDebug() << "No data received.";
    }

    // Vérification des erreurs du port série
    if (serialPort->error() != QSerialPort::NoError) {
        qDebug() << "Error on serial port:" << serialPort->errorString();
    }
}

void MainWindow::updateProductState(const QString& newState)
{
    QSqlQuery query;

    // Requête pour mettre à jour l'état de tous les produits
    query.prepare("UPDATE produits SET ETAT_P = :newState");
    query.bindValue(":newState", "détruit");

    if (query.exec()) {
        qDebug() << "Tous les produits ont été mis à jour en état 'détruite'.";
    } else {
        qDebug() << "Erreur lors de la mise à jour de l'état des produits : " << query.lastError().text();
    }
}

void MainWindow::resetProductStateToNormal()
{
    // Création d'une requête pour mettre à jour l'état de tous les produits
    QSqlQuery query;
    query.prepare("UPDATE produits SET ETAT_P = :newState");  // Requête pour modifier l'état
    query.bindValue(":newState", "normal");  // L'état à attribuer

    // Exécution de la requête
    if (query.exec()) {
        qDebug() << "Tous les produits ont été réinitialisés à l'état 'normal'.";
    } else {
        qDebug() << "Erreur lors de la réinitialisation de l'état des produits : " << query.lastError().text();
    }
}
//service*****************************************************************************************************************************************************************************************************************
void MainWindow::on_pushButtonajouterservice_clicked()
{
    // Récupération des informations saisies
    QString TYPE_S = ui->lineEditType->text();
    if (TYPE_S.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Le champ 'Type' ne peut pas être vide.");
        return; // Arrête l'opération si le champ est vide
    }
    int MATRICULE_S = ui->lineEditMatricule->text().toInt();
    float PRIX_S = ui->lineEditPrix->text().toFloat();
    QString DATE_S = ui->lineEditDateAj->text();

    // Créer et ajouter le service
    s service(TYPE_S, MATRICULE_S, PRIX_S, DATE_S);
    bool test = service.ajouter();

    if (test) {
        // Rafraîchir l'affichage
        QMessageBox::information(this, "Ajout effectué", "Le service a été ajouté avec succès.");

        // Envoi d'un SMS après l'ajout
        //QString numeroClient = "94701785"; // Numéro du client prédéfini
        //service.sendAppointmentSms(numeroClient, MATRICULE_S);
    } else {
        QMessageBox::critical(this, "Échec de l'ajout", "L'ajout du service a échoué.");
    }
}
void MainWindow::on_pushButtonsupprimerservice_clicked()
{
    int matriculeASupprimer =ui->lineEditMatricule->text().toInt();

    if (ServicesObj.supprimer(matriculeASupprimer)) {
        QMessageBox::information(this, "Succès", "Service supprimé avec succès.");
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la suppression du service.");
    }
}

void MainWindow::on_pushButtonafficherservice_clicked()
{
    // Affiche les données existantes dans le QTableView
    ui->tableView_2->setModel(ServicesObj.afficher()); // Affiche tous les services
    QMessageBox::information(this, "Affichage", "Liste des services affichée.");
}

void MainWindow::on_pushButtonTriercroissant_clicked()
{
    QSqlQueryModel* model = ServicesObj.trierParPrixcroissant();
    ui->tableView_2->setModel(model); // Affiche la liste triée dans la vue
}
void MainWindow::on_pushButtonTrierdecroissant_clicked()
{
    QSqlQueryModel* model = ServicesObj.trierParPrixdecroissant();
    ui->tableView_2->setModel(model); // Affiche la liste triée dans la vue
}
void MainWindow::on_pushButtonmodifierservice_clicked()
{
    // Récupération des informations saisies dans les champs
    int MATRICULE_S = ui->lineEditMatricule->text().toInt();  // ID du partenaire
    QString TYPE_S = ui->lineEditType->text();     // Nom du partenaire
    float PRIX_S = ui->lineEditPrix->text().toFloat();  // Contact du partenaire
    QString DATE_S = ui->lineEditDateAj->text();

    // Création de l'objet Partenaire avec les nouvelles informations
    s services(TYPE_S,MATRICULE_S,PRIX_S,DATE_S); // Crée un objet avec les nouvelles valeurs

    // Appel à la méthode modifier() de Partenaire pour mettre à jour les informations dans la base de données
    bool resultat = services.modifier();  // Passe les valeurs à modifier()

    // Vérification du résultat de la modification
    if (resultat) {
        ui->tableView_2->setModel(services.afficher());  // Mise à jour de l'affichage dans le QTableView
        QMessageBox::information(this, "Succès", "servicee modifié avec succès");
    } else {
        QMessageBox::critical(this, "Erreur", "Aucun service trouvé avec cet matricule ou échec de la modification");
    }
}
void MainWindow::on_statistiqueservice_clicked()
{
    // Call the function to generate the marque statistics chart
    if (service.generateServiceStatistics()) {
        qDebug() << "Chart generated successfully.";
    }
    else {
        qDebug() << "Failed to generate chart.";
    }
}
void MainWindow::on_pushButtonPDFservice_clicked() {
    // Générer le PDF en utilisant la classe pdf
    exporterVersPDFservice();
}
void MainWindow::exporterVersPDFservice()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Enregistrer sous"), "", tr("Fichiers PDF (*.pdf)"));
    if (fileName.isEmpty()) {
        return;
    }

    if (!fileName.endsWith(".pdf", Qt::CaseInsensitive)) {
        fileName += ".pdf";
    }

    // Create a QPrinter object
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    // Create a QPainter object
    QPainter painter(&printer);
    if (!painter.isActive()) {
        QMessageBox::critical(this, tr("Erreur"), tr("Impossible d'ouvrir le fichier PDF pour l'écriture."));
        return;
    }

    QSqlQueryModel *model = service.afficher();
    if (!model) {
        QMessageBox::critical(this, tr("Erreur"), tr("Aucun produit à exporter."));
        return;
    }

    int yPosition = 100;  // Position verticale initiale pour le titre
    const int margin = 40;  // Marge de gauche
    const int rowHeight = 300;  // Hauteur d'une ligne
    const int columnWidths[] = { 2300, 2300, 2300, 2300 };  // Largeurs des colonnes

    // Calculer la largeur totale du tableau
    int totalWidth = 0;
    for (int width : columnWidths) {
        totalWidth += width;
    }

    // Ajouter le logo avant le titre
    QImage logo("C:/logo"); // Chemin vers l'image
    if (!logo.isNull()) {
        QRect logoRect(margin, 50, 1000, 1000);
        painter.drawImage(logoRect, logo);
        yPosition += logoRect.height() + 50; // Ajuster la position après le logo
    } else {
        QMessageBox::warning(this, tr("Logo non trouvé"), tr("Le fichier logo22.png est introuvable ou invalide."));
    }

    // Centrer le titre "Liste des Services"
    QFont titleFont = painter.font();
    titleFont.setBold(true);
    titleFont.setPointSize(16);
    painter.setFont(titleFont);
    painter.setPen(QColor(0, 128, 0)); // Vert
    QRect titleRect(margin, yPosition, totalWidth, rowHeight); // Utiliser la largeur totale
    painter.drawText(titleRect, Qt::AlignCenter, "Liste des Services");
    yPosition += 100; // Décalage supplémentaire après le titre

    // Décalage supplémentaire du tableau
    yPosition += 450; // Ajustez cette valeur pour descendre le tableau davantage

    // Dessiner les en-têtes du tableau
    QFont headerFont = painter.font();
    headerFont.setBold(true);
    headerFont.setPointSize(12);
    painter.setFont(headerFont);

    // Changez la couleur des en-têtes en bleu
    painter.setPen(QColor(0, 0, 255)); // Bleu

    int xPosition = margin;

    // Colonnes des en-têtes
    QStringList headers = { "MATRICULE", "Type", "Prix", "Date" };

    // Dessiner les bordures et textes des en-têtes
    for (int i = 0; i < headers.size(); ++i) {
        QRect rect(xPosition, yPosition, columnWidths[i], rowHeight);
        painter.drawRect(rect);  // Dessiner la bordure
        painter.drawText(rect, Qt::AlignCenter, headers[i]);  // Centrer le texte dans la cellule
        xPosition += columnWidths[i];
    }
    yPosition += rowHeight;  // Passer à la ligne suivante

    // Dessiner les données du tableau
    QFont dataFont = painter.font();
    dataFont.setBold(false);
    dataFont.setPointSize(10);
    painter.setFont(dataFont);

    painter.setPen(Qt::black); // Noir pour les données

    for (int row = 0; row < model->rowCount(); ++row) {
        xPosition = margin;

        for (int col = 0; col < model->columnCount(); ++col) {
            QString cellData = model->data(model->index(row, col)).toString();
            QRect rect(xPosition, yPosition, columnWidths[col], rowHeight);
            painter.drawRect(rect);  // Dessiner la bordure
            painter.drawText(rect, Qt::AlignCenter, cellData);  // Centrer le texte dans la cellule
            xPosition += columnWidths[col];
        }

        yPosition += rowHeight;  // Passer à la ligne suivante
    }

    QMessageBox::information(this, tr("Exportation réussie"), tr("La liste des services a été exportée avec succès sous PDF."));
}
void MainWindow::on_pushButtonSend_clicked()
{
    QString userInput = ui->lineEditChatInput->text(); // Récupère l'entrée utilisateur
    if (userInput.isEmpty()) {
        return; // Ne rien faire si l'entrée est vide
    }

    // Ajouter la question dans la zone de discussion
    ui->textBrowserChat->append("Vous : " + userInput);

    // Générer une réponse et l'afficher
    QString response = getChatbotResponse(userInput);
    ui->textBrowserChat->append("Chatbot : " + response);

    // Effacer le champ de saisie
    ui->lineEditChatInput->clear();
}
QString MainWindow::getChatbotResponse(const QString &userInput) {
    // Réponses basées sur les mots-clés
    if (userInput.contains("horaires", Qt::CaseInsensitive)) {
        return "Les horaires de travail sont généralement de 8h00 à 17h00, avec une pause déjeuner de 12h00 à 13h00, du lundi au jeudi.\nCependant, veuillez noter que le vendredi, la pause déjeuner est de 12h00 à 13h45.\nPendant l'été, ces horaires peuvent être modifiés. N'hésitez pas à consulter votre superviseur ou le service RH pour obtenir les horaires d'été à jour !";
    } else if (userInput.contains("avantages", Qt::CaseInsensitive)) {
        return "En tant qu'employé, vous bénéficiez de nombreux avantages qui visent à améliorer votre bien-être et votre développement professionnel. Parmi ces avantages, vous avez droit à une couverture santé, des congés payés, et des opportunités de formation pour enrichir vos compétences. L'entreprise propose également des primes de performance et des bonus selon les résultats individuels et collectifs. De plus, des réductions sur certains produits ou services peuvent être proposées. Selon votre poste, vous pourriez également avoir la possibilité de travailler à distance. Ces avantages sont pensés pour vous offrir un environnement de travail motivant et équilibré.";
    } else if (userInput.contains("ordinateur", Qt::CaseInsensitive)) {
        return "Si votre ordinateur rencontre un problème, vous devez contacter le support informatique de l'entreprise. Vous pouvez envoyer un e-mail à l'adresse support@ecoShine.com. Si le problème est urgent, il est recommandé de vous rendre directement à l'équipe informatique pour une assistance rapide. Assurez-vous de décrire précisément le problème que vous rencontrez afin qu'ils puissent vous aider efficacement.";
    } else {
        return "Désolé, je n'ai pas compris votre demande. Pouvez-vous reformuler ?";
    }
}
void MainWindow::on_pushButtonRechercheservice_clicked() {
    QString typeRecherche = ui->lineEditTypeRecherche->text();

    if (typeRecherche.isEmpty()) {
        QMessageBox::warning(this, "Champ vide", "Veuillez entrer un type de service à rechercher.");
        return;
    }

    // Effectuer la recherche
    QSqlQueryModel* model = ServicesObj.chercherParTypeservice(typeRecherche);

    // Vérifier si des résultats ont été trouvés
    if (model->rowCount() == 0) {
        QMessageBox::information(this, "Aucun résultat", "Aucun service trouvé pour le type : " + typeRecherche);
    } else {
        // Afficher les résultats dans la vue
        ui->tableView_2->setModel(model);
    }
}
void MainWindow::toggleBuzzerdoorlock() {
    if (serialPort->isWritable()) {
        static bool doorState = false;
        QByteArray command = doorState ? "0" : "1";  // 1 pour ouvrir, 0 pour fermer
        serialPort->write(command);
        qDebug() << "Commande envoyée : " << command; // Affiche la commande dans la console Qt

        // Attendre une réponse de l'Arduino (indiquant que la porte a été ouverte ou fermée)
        QByteArray response = serialPort->readAll();
        while (serialPort->canReadLine()) {
            response += serialPort->readAll();
        }

        if (command=="1") {
            ui->buzzerButton->setText("Fermer la porte");
        } else if (command=="0") {
            ui->buzzerButton->setText("Ouvrir la porte");
        }

        // Attendre un moment pour permettre à Arduino de réagir
        QThread::msleep(500);

        doorState = !doorState;
    } else {
        QMessageBox::warning(this, "Erreur", "Port série non accessible.");
        qDebug() << "Erreur : port série non accessible.";
    }
}



// bouton connection entre gestion et acc :**********************************************************************************************************************************************************************************************************

void MainWindow::on_pushButton_9_clicked()
{
    ui->tabWidget->setCurrentIndex(2);
}


void MainWindow::on_pushButton_3_clicked()
{
    ui->tabWidget->setCurrentIndex(2);
}


void MainWindow::on_pushButton_37_clicked()
{
    ui->tabWidget->setCurrentIndex(0);
}


void MainWindow::on_pushButton_40_clicked()
{
    ui->tabWidget->setCurrentIndex(1);
}


void MainWindow::on_pushButton_42_clicked()
{
    ui->tabWidget->setCurrentIndex(3);
}


void MainWindow::on_pushButton_5_clicked()
{
    ui->tabWidget->setCurrentIndex(3);
}


void MainWindow::on_pushButton_11_clicked()
{
    ui->tabWidget->setCurrentIndex(3);
}


void MainWindow::on_pushButton_16_clicked()
{
    ui->tabWidget->setCurrentIndex(2);
}


void MainWindow::on_pushButton_14_clicked()
{
    ui->tabWidget->setCurrentIndex(0);
}


void MainWindow::on_pushButton_18_clicked()
{
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_13_clicked()
{
    ui->tabWidget->setCurrentIndex(0);
}


void MainWindow::on_pushButton_10_clicked()
{
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_43_clicked()
{
     ui->tabWidget->setCurrentIndex(0);
}


void MainWindow::on_pushButton_44_clicked()
{
     ui->tabWidget->setCurrentIndex(0);
}


void MainWindow::on_pushButton_45_clicked()
{
     ui->tabWidget->setCurrentIndex(0);
}


void MainWindow::on_pushButton_53_clicked()
{
     ui->tabWidget->setCurrentIndex(1);
}


void MainWindow::on_pushButton_54_clicked()
{
     ui->tabWidget->setCurrentIndex(1);
}


void MainWindow::on_pushButton_55_clicked()
{
     ui->tabWidget->setCurrentIndex(1);
}


void MainWindow::on_pushButton_50_clicked()
{
     ui->tabWidget->setCurrentIndex(2);
}


void MainWindow::on_pushButton_51_clicked()
{
     ui->tabWidget->setCurrentIndex(2);
}


void MainWindow::on_pushButton_52_clicked()
{
     ui->tabWidget->setCurrentIndex(2);
}


void MainWindow::on_pushButton_58_clicked()
{
     ui->tabWidget->setCurrentIndex(3);
}


void MainWindow::on_pushButton_59_clicked()
{
     ui->tabWidget->setCurrentIndex(3);
}


void MainWindow::on_pushButton_60_clicked()
{
     ui->tabWidget->setCurrentIndex(3);
}


void MainWindow::on_pushButton_7_clicked()
{
     ui->tabWidget->setCurrentIndex(4);
}


void MainWindow::on_pushButton_clicked()
{
     ui->tabWidget->setCurrentIndex(4);
}


void MainWindow::on_pushButton_38_clicked()
{
     ui->tabWidget->setCurrentIndex(4);
}


void MainWindow::on_pushButton_4_clicked()
{
     ui->tabWidget->setCurrentIndex(4);
}


void MainWindow::on_pushButton_46_clicked()
{
     ui->tabWidget->setCurrentIndex(4);
}


void MainWindow::on_pushButton_47_clicked()
{
     ui->tabWidget->setCurrentIndex(4);
}


void MainWindow::on_pushButton_8_clicked()
{
     ui->tabWidget->setCurrentIndex(5);
}


void MainWindow::on_pushButton_2_clicked()
{
    ui->tabWidget->setCurrentIndex(5);
}


void MainWindow::on_pushButton_39_clicked()
{
    ui->tabWidget->setCurrentIndex(5);
}


void MainWindow::on_pushButton_15_clicked()
{
    ui->tabWidget->setCurrentIndex(5);
}


void MainWindow::on_pushButton_48_clicked()
{
    ui->tabWidget->setCurrentIndex(5);
}


void MainWindow::on_pushButton_49_clicked()
{
    ui->tabWidget->setCurrentIndex(5);
}


void MainWindow::on_pushButton_12_clicked()
{
    ui->tabWidget->setCurrentIndex(6);
}


void MainWindow::on_pushButton_6_clicked()
{
    ui->tabWidget->setCurrentIndex(6);
}


void MainWindow::on_pushButton_41_clicked()
{
    ui->tabWidget->setCurrentIndex(6);
}


void MainWindow::on_pushButton_17_clicked()
{
    ui->tabWidget->setCurrentIndex(6);
}


void MainWindow::on_pushButton_56_clicked()
{
    ui->tabWidget->setCurrentIndex(6);
}


void MainWindow::on_pushButton_57_clicked()
{
    ui->tabWidget->setCurrentIndex(6);
}

