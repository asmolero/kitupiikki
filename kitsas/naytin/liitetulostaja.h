/*
   Copyright (C) 2019 Arto Hyvättinen

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
#ifndef LIITETULOSTAJA_H
#define LIITETULOSTAJA_H

#include <QDate>
#include <QVariantMap>

class QPagedPaintDevice;
class QPainter;

class LiiteTulostaja {
public:
    static bool tulostaLiite(QPagedPaintDevice *printer, QPainter* painter,
                        const QByteArray& data, const QString& tyyppi,
                        const QVariantMap& tosite, bool ensisivu, const QString& kieli = QString());

    static bool tulostaMuistiinpanot(QPagedPaintDevice *printer, QPainter* painter,
                                     const QVariantMap& tosite, const QString& kieli = QString());

protected:
    static bool tulostaPdfLiite(QPagedPaintDevice *printer, QPainter* painter,
                        const QByteArray& data,
                        const QVariantMap& tosite, bool ensisivu, const QString& kieli = QString());

    static bool tulostaKuvaLiite(QPagedPaintDevice *printer, QPainter* painter,
                        const QByteArray& data,
                        const QVariantMap& tosite, bool ensisivu, const QString& kieli = QString());



    static void tulostaYlatunniste(QPainter* painter, const QVariantMap& tosite, const QString& kieli = QString());
    static void tulostaAlatunniste(QPainter* painter, const QVariantMap& tosite, const QString& kieli = QString());
    static void tulostaViennit(QPainter* painter, const QVariantList& viennit, const QString& kieli = QString());


};

#endif // LIITETULOSTAJA_H
