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

#ifndef ALVILMOITUSTENMODEL_H
#define ALVILMOITUSTENMODEL_H

#include <QDate>
#include <QAbstractTableModel>
#include <QList>

/**
 * @brief Tehtyjen arvonlisäilmoitusten model
 */
class AlvIlmoitustenModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Sarake
    {
        ALKAA, PAATTYY, ERAPVM, VERO
    };
    enum
    {
        TositeIdRooli = Qt::UserRole,
        PaattyyRooli = Qt::UserRole + 1,
        EraPvmRooli = Qt::UserRole + 2,
        AlkaaRooli = Qt::UserRole + 3,
        MapRooli = Qt::UserRole + 4
    };

    AlvIlmoitustenModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;

    qlonglong marginaalialijaama(const QDate& paiva, int kanta) const;
    bool onkoIlmoitettu(const QDate& paiva) const;

    QDate viimeinenIlmoitus() const;

    /**
     * @brief Alv-ilmoituksen eräpäivä
     * @param loppupaiva Verokauden viimeinen päivä
     * @return
     */
    static QDate erapaiva(const QDate& loppupaiva);

public slots:
    void lataa();
    void dataSaapuu(QVariant* data);

protected:
    QVariantList tiedot_;

};

#endif // ALVILMOITUSTENMODEL_H
