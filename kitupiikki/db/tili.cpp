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

#include "tili.h"

#include <QDebug>
#include <QSqlQuery>

#include "kirjanpito.h"

Tili::Tili() : id_(0), numero_(0), tila_(1), otsikkotaso_(0), muokattu_(false)
{

}

Tili::Tili(int id, int numero, const QString &nimi, const QString &tyyppi, int tila, int otsikkotaso, int ylaotsikkoid) :
    id_(id), numero_(numero), nimi_(nimi), tyyppi_(tyyppi), tila_(tila), otsikkotaso_(otsikkotaso),
    ylaotsikkoId_(ylaotsikkoid), muokattu_(false)

{

}

QString Tili::tyyppiKoodi() const
{
    if( otsikkotaso())
        return( QString("H%1").arg(otsikkotaso()));
    else
        return tyyppi().koodi();
}

void Tili::asetaTyyppi(const QString &tyyppikoodi)
{
    tyyppi_ = kp()->tiliTyypit()->tyyppiKoodilla(tyyppikoodi);
    muokattu_ = true;
}

bool Tili::onkoValidi() const
{
    return numero() > 0 && !nimi().isEmpty();
}

int Tili::ysivertailuluku() const
{
    return ysiluku( numero(), otsikkotaso() );
}


int Tili::saldoPaivalle(const QDate &pvm)
{
    QString kysymys = QString("SELECT SUM(debetsnt), SUM(kreditsnt) FROM vienti WHERE tili=%1 ").arg(id());
    if( onkoTasetili() )
        kysymys.append( QString(" AND pvm <= \"%1\" ").arg(pvm.toString(Qt::ISODate)));
    else
        kysymys.append( QString(" AND pvm BETWEEN \"%1\" AND \"%2\" ")
                        .arg( kp()->tilikaudet()->tilikausiPaivalle(pvm).alkaa().toString(Qt::ISODate) )
                        .arg( pvm.toString(Qt::ISODate )));

    QSqlQuery kysely(kysymys);
    if( kysely.next())
    {
        int debet = kysely.value(0).toInt();
        int kredit = kysely.value(1).toInt();

        if( onkoEdellistenYliAliJaama() )
        {
            // Edellisten yli/alijaamaan pitää laskea vielä edellisten tulokset
            QSqlQuery edelliskysely( QString("SELECT SUM(debetsnt), SUM(kreditsnt) FROM vienti, tili "
                                             "WHERE vienti.tili = tili.id AND pvm < \"%1\" "
                                             "AND ysiluku > 300000000 ")
                                     .arg(kp()->tilikaudet()->tilikausiPaivalle(pvm).alkaa().toString(Qt::ISODate)));
            if( edelliskysely.next())
            {
                return kredit + edelliskysely.value(1).toInt() - debet - edelliskysely.value(0).toInt();
            }
        }
        else if( onkoVastaavaaTili())
            return debet - kredit;
        else
            return kredit - debet;
    }
    return 0;
}

int Tili::montakoVientia() const
{
    QSqlQuery kysely( QString("SELECT sum(id) FROM vienti WHERE tili=%1").arg(id()) );
    if( kysely.next())
        return kysely.value(0).toInt();
    return 0;
}

bool Tili::onkoTasetili() const
{
    return( tyyppiKoodi().startsWith('A') || tyyppiKoodi().startsWith('B'));
}


bool Tili::onkoTulotili() const
{
    return tyyppiKoodi().startsWith('C');
}

bool Tili::onkoMenotili() const
{
    return tyyppiKoodi().startsWith('D');
}

bool Tili::onkoVastaavaaTili() const
{
    return tyyppiKoodi().startsWith('A');
}

bool Tili::onkoVastattavaaTili() const
{
    return tyyppiKoodi().startsWith('B');
}

bool Tili::onkoPoistettavaTaseTili() const
{
    return tyyppiKoodi().startsWith("AP");
}

bool Tili::onkoRahaTili() const
{
    return tyyppiKoodi().startsWith("AR");
}

bool Tili::onkoEdellistenYliAliJaama() const
{
    return tyyppiKoodi() == "BE";
}

bool Tili::onkoTaseEraSeurattava() const
{
    return ( tyyppiKoodi().startsWith('A') && !tyyppiKoodi().startsWith("AR") && tyyppiKoodi() != "AL" ) ||
           ( tyyppiKoodi().startsWith('B') && tyyppiKoodi()!="BE" && tyyppiKoodi()!="BL");
}

int Tili::ysiluku(int luku, int taso)
{
    if( !taso )
        return ysiluku(luku) + 9;
    else
        return ysiluku(luku) + taso - 1;
}

int Tili::ysiluku(int luku, bool loppuu)
{
    return laskeysiluku(luku, loppuu);
}

int Tili::laskeysiluku(int luku, bool loppuu)
{
    if( !luku )     // Nolla on nolla eikä voi muuta olla!
        return 0;

    while( luku <= 99999999 )
    {
        luku = luku * 10;
        if( loppuu )
            luku = luku + 9;    // Loppuluku täytetään ysillä
    }
    return luku;
}


