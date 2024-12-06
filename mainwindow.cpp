#include "mainwindow.h"
#include "ui_mainwindow.h"
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
#include <QFileDialog>
#include <QPrinter>
#include <QDesktopServices>

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QPushButton>

#include <QCoreApplication>

QSqlDatabase db;







Produit P;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    serialPort(new QSerialPort(this))  // Initialisation de l'objet QSerialPort

{
    ui->setupUi(this);
    monProduit = new Produit;
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









    // Créer un timer pour vérifier la quantité toutes les 5 secondes (5000 ms)
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::checkStock);
    timer->start(5000);  // Vérifier toutes les 5 secondes

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


}






MainWindow::~MainWindow()
{
    if (serialPort->isOpen()) {
        serialPort->close();
    }
    delete serialPort;  // Libérer la mémoire
    delete ui;
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


void MainWindow::on_pushButton_13_clicked()
{
     ui->tabWidget->setCurrentIndex(0);
}


void MainWindow::on_pushButton_10_clicked()
{
     ui->tabWidget->setCurrentIndex(1);
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



