#include "mainwindow.h"
#include "qsqlerror.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QDebug>
#include <QMessageBox>
#include <QBrush>
#include <QPen>
#include <QFileDialog>
#include <QPdfWriter>
#include <QPainter>
#include <QSqlQueryModel>
#include <QPrinter>
#include <QFileDialog>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QPushButton>
#include <QThread>

#include <QSqlDatabase>
#include <QSqlError>
#include <QVariant>
s service;
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ui->tableView->setModel(ServicesObj.afficher()); // Affiche les services dans le QTableView
    serialPort = new QSerialPort(this);
    serialPort->setPortName("COM7");  // Changez COM7 selon le port de votre Arduino
    serialPort->setBaudRate(QSerialPort::Baud115200);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    // Ouvrir le port série
    if (!serialPort->open(QIODevice::ReadWrite)) {
        QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir le port série.");
    }

    // Connecter le bouton buzzer à une fonction pour contrôler le buzzer
    connect(ui->buzzerButton, &QPushButton::clicked, this, &MainWindow::toggleBuzzer);
    connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::readData);

}
/*
MainWindow::~MainWindow()
{
    delete ui;
}*/
MainWindow::~MainWindow()
{
    if (serialPort->isOpen()) {
        serialPort->close();
    }
    delete serialPort;  // Libérer la mémoire
    delete ui;
}

//*******************************************************************
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
void MainWindow::on_pushButtonModifier_clicked()
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
        ui->tableView->setModel(services.afficher());  // Mise à jour de l'affichage dans le QTableView
        QMessageBox::information(this, "Succès", "servicee modifié avec succès");
    } else {
        QMessageBox::critical(this, "Erreur", "Aucun service trouvé avec cet matricule ou échec de la modification");
    }
}

void MainWindow::on_pushButtonAjouter_clicked()
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

void MainWindow::on_pushButtonSupprimer_clicked()
{
    int matriculeASupprimer =ui->lineEditMatricule->text().toInt();

    if (ServicesObj.supprimer(matriculeASupprimer)) {
        QMessageBox::information(this, "Succès", "Service supprimé avec succès.");
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la suppression du service.");
    }
}

void MainWindow::on_pushButtonAfficher_clicked()
{
    // Affiche les données existantes dans le QTableView
    ui->tableView->setModel(ServicesObj.afficher()); // Affiche tous les services
    QMessageBox::information(this, "Affichage", "Liste des services affichée.");
}


// Fonction pour le bouton d'exportation PDF
void MainWindow::on_pushButtonPDF_clicked() {
    // Générer le PDF en utilisant la classe pdf
    exporterVersPDF();
}
void MainWindow::on_pushButtonRecherche_clicked() {
    QString typeRecherche = ui->lineEditTypeRecherche->text();

    if (typeRecherche.isEmpty()) {
        QMessageBox::warning(this, "Champ vide", "Veuillez entrer un type de service à rechercher.");
        return;
    }

    // Effectuer la recherche
    QSqlQueryModel* model = ServicesObj.chercherParType(typeRecherche);

    // Vérifier si des résultats ont été trouvés
    if (model->rowCount() == 0) {
        QMessageBox::information(this, "Aucun résultat", "Aucun service trouvé pour le type : " + typeRecherche);
    } else {
        // Afficher les résultats dans la vue
        ui->tableView->setModel(model);
    }
}


void MainWindow::on_pushButtonTriercroissant_clicked()
{
    QSqlQueryModel* model = ServicesObj.trierParPrixcroissant();
    ui->tableView->setModel(model); // Affiche la liste triée dans la vue
}
void MainWindow::on_pushButtonTrierdecroissant_clicked()
{
    QSqlQueryModel* model = ServicesObj.trierParPrixdecroissant();
    ui->tableView->setModel(model); // Affiche la liste triée dans la vue
}

void MainWindow::on_generatetypeButton_clicked()
{
    // Call the function to generate the marque statistics chart
    if (service.generateServiceStatistics()) {
        qDebug() << "Chart generated successfully.";
    }
    else {
        qDebug() << "Failed to generate chart.";
    }
}


/*
void MainWindow::on_SendSmsButton_clicked()
{
    // Create an object of the Piece class (you may need to pass the necessary arguments here)
    s services;
    int MATRICULE_S = ui->lineEditTypeRecherche_2->text().toInt();

    // Call the method to send SMS for low stock pieces
    services.sendAppointmentSms();}
*/

void MainWindow::exporterVersPDF()
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
    QStringList headers = { "MATRICULE", "Nom", "Type", "Date" };

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

/*
void MainWindow::toggleBuzzer() {
    if (serialPort->isWritable()) {
        static bool doorState = false;
        QByteArray command = doorState ? "0" : "1";  // 1 pour ouvrir, 0 pour fermer
        serialPort->write(command);
        qDebug() << "Commande envoyée : " << command; // Affiche la commande dans la console Qt

        // Attendre un moment pour permettre à Arduino de réagir
        QThread::msleep(500);

        doorState = !doorState;
    } else {
        QMessageBox::warning(this, "Erreur", "Port série non accessible.");
        qDebug() << "Erreur : port série non accessible.";
    }
}
*/
void MainWindow::toggleBuzzer() {
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
/*
QString MainWindow::readData() {
    // Lire toutes les données disponibles sur le port série
    QByteArray data = serialPort->readAll();
    qDebug() << "Raw data (QByteArray):" << data;
    QString receivedText;
    // Vérifier si des données ont été reçues
    if (!data.isEmpty()) {
        // Convertir les données en chaîne de caractères (UTF-8)
         receivedText = QString::fromUtf8(data);

        // Afficher les données reçues dans un QTextEdit ou autre widget
        qDebug() << "Received Text:" << receivedText;
         int a=chercher(receivedText);
        QString
         qDebug() << "a=" << a;


    } else {
        qDebug() << "No data received.";
    }
    return receivedText;
}*/

void MainWindow::readData() {
    // Lire toutes les données disponibles sur le port série
    QByteArray data = serialPort->readAll();
    qDebug() << "Raw data (QByteArray):" << data;

    // Vérifier si des données ont été reçues
    if (!data.isEmpty()) {
        // Convertir les données en chaîne de caractères (UTF-8)
        QString receivedText  = QString::fromUtf8(data);

        // Afficher les données reçues dans un QTextEdit ou autre widget
        qDebug() << "Received Text:" << receivedText;

        int a = chercher(receivedText);
        qDebug() << "a=" << a;

        // Convertir l'entier 'a' en QString
        QString command = QString::number(a);
        qDebug() << "Command to send to Arduino:" << command;

        // Envoyer la commande à l'Arduino
        if (serialPort->isWritable()) {
            // Envoyer la commande en ajoutant un caractère de fin (par exemple '\n')
            serialPort->write(command.toUtf8() + '\n');
            qDebug() << "Commande envoyée : " << command;
        } else {
            qDebug() << "Le port série n'est pas accessible.";
        }

    } else {
        qDebug() << "No data received.";
    }

}

int MainWindow::chercher(const QString& mdp) {
    QSqlQuery query;
    int idE=mdp.toInt();
    query.prepare("SELECT COUNT(*) FROM SYSTEM.EMPLOYES WHERE ID_E = :idE");
    query.bindValue(":idE", idE);
    qDebug() << "code entree:"<<idE;

    if (query.exec()) {
        query.next();  // Aller au premier résultat
        int count = query.value(0).toInt();  // Récupérer le nombre de résultats
        if (count > 0) {
            qDebug() << "Type trouvé : " << idE;
            return 2;  // Si le type existe
        } else {
            qDebug() << "Type non trouvé : " << idE;
            return 3;  // Si le type n'existe pas
        }
    } else {
        qDebug() << "Erreur lors de la recherche du type : " << query.lastError().text();
        return 0;  // En cas d'erreur, retourner 0
    }
}

//***********************************************************
/*
void MainWindow::readData() {
    // Lire toutes les données disponibles sur le port série
    QByteArray data = serialPort->readAll();
    qDebug() << "Raw data (QByteArray):" << data;

    // Vérifier si des données ont été reçues
    if (!data.isEmpty()) {
        // Convertir les données en chaîne de caractères (UTF-8)
        QString receivedText = QString::fromUtf8(data);

        // Afficher les données reçues dans un QTextEdit ou autre widget
        qDebug() << "Received Text:" << receivedText;

        // Appeler la fonction chercher pour déterminer la commande
        int a = chercher(receivedText);
        qDebug() << "a=" << a;

        // Convertir l'entier 'a' en QString pour envoyer à l'Arduino
        QString command = QString::number(a);
        qDebug() << "Command to send to Arduino:" << command;

        // Envoyer la commande à l'Arduino uniquement si le port est accessible
        if (serialPort->isWritable()) {
            // Envoyer la commande en ajoutant un caractère de fin
            serialPort->write(command.toUtf8() + '\n');
            qDebug() << "Commande envoyée : " << command;
        } else {
            qDebug() << "Le port série n'est pas accessible.";
        }

    } else {
        qDebug() << "No data received.";
    }
}
int MainWindow::chercher(const QString& mdp) {
    QSqlQuery query;
    int idE = mdp.toInt();  // Conversion de la chaîne en entier pour l'identifiant de l'employé
    query.prepare("SELECT COUNT(*) FROM SYSTEM.EMPLOYES WHERE ID_E = :idE");
    query.bindValue(":idE", idE);
    qDebug() << "Code entrée:" << idE;

    if (query.exec()) {
        query.next();  // Aller au premier résultat
        int count = query.value(0).toInt();  // Récupérer le nombre de résultats
        if (count > 0) {
            qDebug() << "Employé trouvé : " << idE;
            return 2;  // Si l'employé existe, envoyer une commande pour ouvrir la porte
        } else {
            qDebug() << "Employé non trouvé : " << idE;
            return 3;  // Si l'employé n'existe pas, envoyer une commande pour fermer la porte
        }
    } else {
        qDebug() << "Erreur lors de la recherche de l'employé : " << query.lastError().text();
        return 0;  // En cas d'erreur, retourner 0
    }
}

*/
//***********************************************************



