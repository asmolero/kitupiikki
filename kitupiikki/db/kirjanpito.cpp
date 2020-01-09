/*
   Copyright (C) 2017 Arto Hyvättinen

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QFileInfo>
#include <QSettings>
#include <QPrinter>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QSqlError>
#include <QTextStream>
#include <QBuffer>
#include <QRandomGenerator>

#include <QDebug>

#include <ctime>

#include <QNetworkAccessManager>

#include "kirjanpito.h"
#include "naytin/naytinikkuna.h"

#include "tilimodel.h"

#include "pilvi/pilvimodel.h"
#include "sqlite/sqlitemodel.h"
#include "tositetyyppimodel.h"
#include "alv/alvilmoitustenmodel.h"
#include "rekisteri/ryhmatmodel.h"
#include "tools/tulkki.h"

Kirjanpito::Kirjanpito(const QString& portableDir) :
    QObject(nullptr),
    harjoitusPvm( QDate::currentDate()),
    asetusModel_(new AsetusModel(this)),
    tiliModel_( new TiliModel(this)),
    tilikaudetModel_(new TilikausiModel( this)),
    kohdennukset_(new KohdennusModel(this)),
    veroTyypit_( new VerotyyppiModel(this)),
    tiliTyypit_( new TilityyppiModel(this)),
    tuotteet_( new TuoteModel(this)),
    ryhmat_( new RyhmatModel(this)),
    alvIlmoitukset_( new AlvIlmoitustenModel(this)),
    tempDir_(nullptr),
    portableDir_(portableDir),
    pilviModel_(new PilviModel(this)),
    sqliteModel_( new SQLiteModel(this)),
    yhteysModel_(nullptr),
    tositeTyypit_( new TositeTyyppiModel()),
    tulkki_( new Tulkki(":/tr/tulkki.json", this))
{
    if( portableDir.isEmpty())
        settings_ = new QSettings(this);
    else
    {
        // Asentamattomassa windows-versiossa asetukset ohjelman hakemistoon
        QDir portable(portableDir);
        settings_ = new QSettings(portable.absoluteFilePath("kitsas.ini"),QSettings::IniFormat, this);
    }

    networkManager_ = new QNetworkAccessManager(this);

    printer_ = new QPrinter(QPrinter::HighResolution);

    // Jos järjestelmässä ei ole yhtään tulostinta, otetaan käyttöön pdf-tulostus jotta
    // saadaan dialogit

    if( !printer_->isValid())
        printer()->setOutputFileName( QDir::temp().absoluteFilePath("print.pdf") );

    printer_->setPaperSize(QPrinter::A4);
    printer_->setPageMargins(10,5,5,5, QPrinter::Millimeter);


    tempDir_ = new QTemporaryDir(QDir::temp().absoluteFilePath("kitsas-XXXXXX"));
    if( !tempDir_->isValid())
    {
        delete tempDir_;

        tempDir_ = new QTemporaryDir( QDir::home().absoluteFilePath("kitsas-XXXXXX")  );
        if( !tempDir_->isValid())
            QMessageBox::critical(nullptr, tr("Tilapäishakemiston luominen epäonnistui"),
                                  tr("Kitupiikki ei onnistunut luomaan tilapäishakemistoa. Raporttien ja laskujen esikatselu ei toimi."));
    }
}

Kirjanpito::~Kirjanpito()
{
    tietokanta_.close();
    delete tempDir_;
}

QString Kirjanpito::asetus(const QString &avain) const
{
    return asetukset()->asetus(avain);
}

QString Kirjanpito::kirjanpitoPolku()
{
    SQLiteModel* sqlite = qobject_cast<SQLiteModel*>( yhteysModel() );
    if( sqlite)
        return sqlite->tiedostopolku();
    PilviModel* pilvi = qobject_cast<PilviModel*>(yhteysModel());
    if( pilvi )
        return QString::number(pilvi->pilviId());
    return QString();
}


QDate Kirjanpito::paivamaara() const
{
    if( onkoHarjoitus())
        return harjoitusPvm;
    else
        return QDate::currentDate();
}


Tilikausi Kirjanpito::tilikausiPaivalle(const QDate &paiva) const
{
    return tilikaudet()->tilikausiPaivalle(paiva);
}

void Kirjanpito::ohje(const QString &ohjesivu)
{
    QString osoite("https://kitupiikki.info/");
    osoite.append(ohjesivu);
    if(!QDesktopServices::openUrl( QUrl(osoite)))
        QMessageBox::critical(nullptr, tr("Ohjeen näyttäminen epäonnistui"),
                              tr("Kitupiikki ei saanut käynnistettyä selainta ohjeen näyttämiseksi. Onhan järjestelmässäsi "
                                 "määritelty oletusselain avaamaan internet-sivuja?\n\n"
                                 "Ohjelman ohjeet löytyvät sivulta https://kitupiikki.info"));
}

void Kirjanpito::avaaUrl(const QUrl &url)
{
    if( url.fileName().endsWith(".pdf"))
        NaytinIkkuna::naytaTiedosto( url.path() );
    else if( !QDesktopServices::openUrl(url) )
    {
        if( url.fileName().endsWith("html"))
            QMessageBox::critical(nullptr, tr("Selaimen käynnistäminen epäonnistui"),
                                  tr("Kitupiikki ei saanut käynnistettyä selainta tiedoston %1 näyttämiseksi. Onhan järjestelmässäsi "
                                     "määritelty oletusselain avaamaan internet-sivuja?\n\n").arg(url.toDisplayString()));
        else
            QMessageBox::critical(nullptr, tr("Tiedoston näyttäminen epäonnistui"),
                                  tr("Kitupiikki ei saanut käynnistettyä ulkoista ohjelmaa tiedoston %1 näyttämiseksi.").arg(url.toDisplayString() ));
    }
}

QString Kirjanpito::tilapainen(QString nimi) const
{
    return tempDir_->filePath(nimi.replace("XXXX", satujono(8)));
}

bool Kirjanpito::onkoMaksuperusteinenAlv(const QDate &paiva) const
{
    // Onko annettuna päivänä maksuperusteinen alv käytössä
    if( !asetukset()->onko("AlvVelvollinen") || !asetukset()->onko("MaksuAlvAlkaa"))
        return false;
    if( asetukset()->pvm("MaksuAlvAlkaa") > paiva )
        return false;
    if( asetukset()->onko("MaksuAlvLoppuu") && asetukset()->pvm("MaksuAlvLoppuu") <= paiva )
        return false;
    return true;
}

void Kirjanpito::asetaLogo(const QImage &logo)
{
    logo_ = logo;

    QByteArray ba;

    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    logo_.save(&buffer, "PNG");
    buffer.close();

    KpKysely *kysely = kpk("/liitteet/0/logo", KpKysely::PUT);
    kysely->lahetaTiedosto(ba);

    emit logoMuuttui();
}

QString Kirjanpito::arkistopolku()
{
    SQLiteModel *sqlite = qobject_cast<SQLiteModel*>( yhteysModel() );
    if( sqlite ) {
        QString polku = sqlite->tiedostopolku();
        if( polku.endsWith(".kitsas"))
         return polku.replace(".kitsas","-arkisto");
    }
    return "";
}


QString Kirjanpito::tositeTunnus(int tunniste, const QDate &pvm, bool vertailu)
{
    QString vuositunnus = tilikaudetModel_->tilikausiPaivalle(pvm).kausitunnus();

    // Tässä voidaan siivota pois laji jos ollaan yhdessä sarjassa

    if( vertailu )
        return QString("%1/%2")
                .arg( tunniste, 8, 10, QChar('0') )
                .arg( vuositunnus );

    return QString("%1/%2")
            .arg(tunniste)
            .arg(vuositunnus);
}

QString Kirjanpito::tositeTunnus(int tunniste, const QDate &pvm, const QString &sarja, bool samakausi, bool vertailu)
{

    if( vertailu)
        return QString("%1 %2 %3")
                .arg(sarja)
                .arg( tilikaudet()->tilikausiPaivalle(pvm).pitkakausitunnus() )
                .arg( tunniste, 8, 10, QChar('0')  );

    if( samakausi )
        return QString("%1%2")
                .arg(sarja)
                .arg(tunniste);

    return QString("%1%2/%3")
            .arg(sarja)
            .arg(tunniste)
            .arg( tilikaudet()->tilikausiPaivalle(pvm).kausitunnus() );
}



bool Kirjanpito::avaaTietokanta(const QString &tiedosto, bool ilmoitaVirheesta)
{

    return sqlite()->avaaTiedosto(tiedosto, ilmoitaVirheesta);
}



void Kirjanpito::yhteysAvattu(YhteysModel *model)
{
    if( yhteysModel_ && model != yhteysModel_ ) {
        yhteysModel_->sulje();
    }
    yhteysModel_ = model;
    logo_ = QImage();

    if( yhteysModel() ) {
        KpKysely *logokysely = kpk("/liitteet/0/logo");
        connect( logokysely, &KpKysely::vastaus, this, &Kirjanpito::logoSaapui);
        logokysely->kysy();
    }
    tuotteet()->lataa();
    ryhmat()->paivita();
    alvIlmoitukset()->lataa();

    emit tietokantaVaihtui();
}

QString Kirjanpito::kaanna(const QString &teksti, const QString &kieli) const
{
    return tulkki_->k(teksti, kieli);
}

bool Kirjanpito::lataaUudelleen()
{
    return avaaTietokanta(kirjanpitoPolku());
}

void Kirjanpito::asetaHarjoitteluPvm(const QDate &pvm)
{
    harjoitusPvm = pvm;
}

void Kirjanpito::logoSaapui(QVariant *reply)
{
    QByteArray ba = reply->toByteArray();

    logo_ = QImage::fromData( ba );
    emit logoMuuttui();
}


Kirjanpito *Kirjanpito::db()
{
    return instanssi__;
}

void Kirjanpito::asetaInstanssi(Kirjanpito *kp)
{
    instanssi__ = kp;
    kp->pilvi()->kirjaudu();
    kp->sqlite()->lataaViimeiset();
}


QString Kirjanpito::satujono(int pituus)
{
    // https://stackoverflow.com/questions/18862963/qt-c-random-string-generation
    const QString merkit("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

    QString randomString;
    for(int i=0; i<pituus; ++i)
    {
       int index = QRandomGenerator::global()->generate() % merkit.length() ;
       QChar nextChar = merkit.at(index);
       randomString.append(nextChar);
    }
    return randomString;
}

void Kirjanpito::paivita(int versioon)
{
    QFile sqltiedosto( QString(":/sql/update%1.sql").arg(versioon));
    sqltiedosto.open(QIODevice::ReadOnly);
    QTextStream in(&sqltiedosto);
    QString sqluonti = in.readAll();
    sqluonti.replace("\n"," ");
    QStringList sqlista = sqluonti.split(";");
    QSqlQuery query;

    foreach (QString kysely,sqlista)
    {
        query.exec(kysely);
        qApp->processEvents();
    }
}

Kirjanpito* Kirjanpito::instanssi__ = nullptr;

Kirjanpito *kp()  { return Kirjanpito::db(); }


KpKysely *kpk(const QString &polku, KpKysely::Metodi metodi)
{
    if( kp()->yhteysModel() )
        return kp()->yhteysModel()->kysely(polku, metodi);
    return nullptr;
}

QIcon lippu(const QString &kielikoodi)
{
    return QIcon(":/liput/" + kielikoodi + ".png");
}

QString tulkkaa(const QString &teksti, const QString &kieli)
{
    return kp()->kaanna(teksti, kieli);
}
