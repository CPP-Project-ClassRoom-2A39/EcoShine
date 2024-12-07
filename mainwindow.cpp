#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "partenaire.h"  // Inclure la classe Partenaire
#include <QMessageBox>
#include <QDateTime>
#include <QCalendarWidget>
#include <QTableView>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPrinter>
#include <QPainter>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QDate>
#include <QSqlQuery>
#include <QTimer>

Partenaire P;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialiser le QSystemTrayIcon
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/img/res.png"));
    trayIcon->setVisible(true);

    // Initialiser un QTimer pour vérifier chaque jour
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::verifierLivraisonDemain);
    timer->start(60000);  // Vérifier toutes les minutes


    monPartenaire = new Partenaire;
    ui->tableView->setModel(P.afficher());

    // Connecte le bouton d'exportation à la fonction d'exportation PDF
    connect(ui->pushButton_exporter, &QPushButton::clicked, this, &MainWindow::exporterVersPDF);
    // Connecte le bouton statistiques à la fonction afficherStatistiques
    connect(ui->pushButton_statistiques, &QPushButton::clicked, this, &MainWindow::afficherStatistiques);
    //Connecter la sélection d'une date à l'affichage des partenaires
    connect(ui->calendarWidget, &QCalendarWidget::clicked, this, &MainWindow::afficherPartenairesPourDate);
}


MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_pushButton_ajouter_clicked()
{
    // Récupération des informations saisies dans les champs
    int ID_PA = ui->id_pa->text().toInt();
    QString NOM_PA = ui->nom_pa->text();
    QString ADRESSE_PA = ui->adresse_pa->text();
    int CONTACT_PA = ui->contact_pa->text().toInt();
    QString DATE_PA = ui->date_pa->text();

    Partenaire P(ID_PA, NOM_PA, ADRESSE_PA, CONTACT_PA,DATE_PA);  // Création de l'objet Partenaire

    bool test = P.ajouter();  // Appel à la méthode ajouter() de Partenaire
    if (test) {
        ui->tableView->setModel(P.afficher());  // Mise à jour de l'affichage
        mettreAJourCalendrier();  // Mise à jour du calendrier
        QMessageBox::information(this, QObject::tr("Ajout effectué"),
                                 QObject::tr("Le partenaire a été ajouté avec succès."));
    } else {
        QMessageBox::critical(this, QObject::tr("Échec de l'ajout"),
                              QObject::tr("L'ajout du partenaire a échoué."));
    }
}

void MainWindow::on_pushButton_supprimer_4_clicked()
{
    int idASupprimer = ui->supprimer_id->text().toInt();  // Récupération de l'ID du partenaire
    if (monPartenaire->supprimer(idASupprimer)) {  // Suppression du partenaire
        ui->tableView->setModel(P.afficher());  // Mise à jour du tableau
        mettreAJourCalendrier();  // Mise à jour du calendrier
        QMessageBox::information(this, "Succès", "Partenaire supprimé avec succès");
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la suppression du partenaire");
    }
}

void MainWindow::on_pushButton_modifier_clicked()
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
    Partenaire P(ID_PA, NOM_PA, ADRESSE_PA, CONTACT_PA,DATE_PA);  // Crée un objet avec les nouvelles valeurs

    // Appel à la méthode modifier() de Partenaire pour mettre à jour les informations dans la base de données
    bool resultat = P.modifier();  // Passe les valeurs à modifier()

    // Vérification du résultat de la modification
    if (resultat) {
        ui->tableView->setModel(P.afficher());  // Mise à jour de l'affichage dans le QTableView
        mettreAJourCalendrier();  // Mise à jour du calendrier
        QMessageBox::information(this, "Succès", "Partenaire modifié avec succès");
    } else {
        QMessageBox::critical(this, "Erreur", "Aucun partenaire trouvé avec cet ID ou échec de la modification");
    }
}

// Fonction d'affichage des partenaires
void MainWindow::on_pushButton_afficher_clicked()
{
    ui->tableView->setModel(monPartenaire->afficher());  // Affichage des partenaires
}


void MainWindow::on_pushButton_afficher_tri_clicked()
{
    // Crée un objet Partenaire
    Partenaire P;

    // Récupère les partenaires triés par nom
    QSqlQueryModel *model = P.tri_nom();

    if (model) {
        // Associe le modèle au QTableView pour l'affichage
        ui->tableView->setModel(model);
    } else {
        // Si la récupération du modèle échoue, affiche un message d'erreur
        QMessageBox::critical(this, "Erreur", "Erreur lors du chargement des partenaires.");

    }
}

void MainWindow::on_pushButton_afficher_tri2_clicked()
{
    // Crée un objet Partenaire
    Partenaire P;

    // Récupère les partenaires triés par nom (ordre décroissant)
    QSqlQueryModel *model = P.tri_nom_decroissant();

    if (model) {
        // Associe le modèle au QTableView pour l'affichage
        ui->tableView->setModel(model);
    } else {
        // Si la récupération du modèle échoue, affiche un message d'erreur
        QMessageBox::critical(this, "Erreur", "Erreur lors du chargement des partenaires.");
    }
}

void MainWindow::on_pushButton_recherche_clicked()
{
    // Récupérer l'ID saisi par l'utilisateur
    int id = ui->idRecherche->text().toInt(); // Suppose que vous avez un champ de texte avec l'ID

    // Créer un objet Partenaire pour effectuer la recherche
    Partenaire P;

    // Appeler la fonction recherche_par_id() pour obtenir les résultats
    QSqlQueryModel* model = P.recherche_par_id(id);

    // Si le modèle n'est pas null, afficher le résultat
    if (model) {
        if (model->rowCount() > 0) {
            // Afficher les résultats dans un QTableView
            ui->tableView->setModel(model);
        } else {
            // Si aucun résultat n'est trouvé, afficher un message
            QMessageBox::information(this, "Aucun résultat", "Aucun partenaire trouvé avec cet ID.");
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

void MainWindow::afficherStatistiques() {
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
        ui->tableView->setModel(model);
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

void MainWindow::on_pushButton_partenaire_clicked()
{
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_acc_clicked()
{
    ui->tabWidget->setCurrentIndex(0);
}


void MainWindow::on_pushButton_serac_clicked()
{
     ui->tabWidget->setCurrentIndex(2);
}


void MainWindow::on_pushButton_serp_clicked()
{
     ui->tabWidget->setCurrentIndex(2);
}

