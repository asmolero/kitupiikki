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
#include <QtTest>

#include "model/euro.h"

class EuroTest : public QObject
{
    Q_OBJECT

public:
    EuroTest();
    ~EuroTest();

private slots:
    void initTestCase();
    void string_to_euro();
    void less1string_to_euro();
    void qlonglong_to_string();
    void euro_to_qstring();
    void qvariant_to_euro();
    void neq_double_to_euro();
    void euro_to_dbl();
    void euroSumma();
    void euro_to_bool();
    void str1();
    void str2();
    void neg_to_str();
    void euro_eq_euro();
};

EuroTest::EuroTest()
{
    qRegisterMetaTypeStreamOperators<Euro>("Euro");
}

EuroTest::~EuroTest()
{

}

void EuroTest::initTestCase()
{
    qRegisterMetaTypeStreamOperators<Euro>("Euro");
}

void EuroTest::string_to_euro()
{
    Euro euro1("123.45");
    QCOMPARE(euro1.cents(), 12345);

    Euro euro2("123.4");
    QCOMPARE(euro2.cents(), 12340);

    Euro euro3("-123.45");
    QCOMPARE(euro3.cents(), -12345);

    Euro euro4("-123.5");
    QCOMPARE(euro4.cents(), -12350);

    Euro euro5("123");
    QCOMPARE(euro5.cents(), 12300);

    Euro euro6("-123");
    QCOMPARE(euro6.cents(), -12300);
}

void EuroTest::less1string_to_euro()
{
    Euro euro1("0.01");
    QCOMPARE(euro1.cents(), 1);

    Euro euro2("-0.01");
    QCOMPARE(euro2.cents(), -1);

    Euro euro3("0.00");
    QCOMPARE(euro3.cents(), 0);

    Euro euro4("0.99");
    QCOMPARE(euro4.cents(), 99);

    Euro euro5("-0.99");
    QCOMPARE(euro5.cents(), -99);

    Euro euro6("0.5");
    QCOMPARE(euro6.cents(), 50);

    Euro euro7("-0.5");
    QCOMPARE(euro7.cents(), -50);
}

void EuroTest::qlonglong_to_string()
{
    Euro euro1(12345);
    QCOMPARE(euro1.toString(),"123.45");

    Euro euro2(-12304);
    QCOMPARE(euro2.toString(), "-123.04");

    Euro euro3(10000);
    QCOMPARE(euro3.toString(), "100.00");
}

void EuroTest::euro_to_qstring()
{
    Euro euro1(12345);

    QString str;
    str << euro1;

    QCOMPARE(str, "123.45");
}

void EuroTest::qvariant_to_euro()
{
    QVariant variant(123.5);
    Euro euro;
    euro << variant;
    QCOMPARE(euro.cents(), 12350);
}

void EuroTest::neq_double_to_euro()
{
    Euro euro = Euro::fromDouble(-123.45);
    QCOMPARE( euro.cents(), -12345);
    QCOMPARE( euro, Euro("-123.45"));
}

void EuroTest::euro_to_dbl()
{
    Euro euro("12345.67");
    QVERIFY( qAbs(euro.toDouble() - 12345.67) < 1e-5 );
}


void EuroTest::euroSumma()
{
    Euro euro1(1200);
    Euro euro2(2450);
    Euro summa = euro1 + euro2;

    QCOMPARE(summa.cents(), 3650);
}

void EuroTest::euro_to_bool()
{
    Euro euro1(123);
    QVERIFY(euro1);
    Euro euro2(0);
    QVERIFY(!euro2);
}

void EuroTest::str1()
{
    Euro euro = Euro::fromString("1 234,56 -");
    QCOMPARE(euro.cents(), -123456);
    Euro euro2 = Euro::fromString("- 7 654,32");
    QCOMPARE(euro2.cents(), -765432);
}

void EuroTest::str2()
{
    Euro euro = Euro::fromString("1 234,56 +");
    QCOMPARE(euro.cents(), 123456);
    Euro euro2 = Euro::fromString("+ 7 654,32");
    QCOMPARE(euro2.cents(), 765432);
}

void EuroTest::neg_to_str()
{
    Euro euro = Euro(-65);
    QCOMPARE(euro.toString(),"-0.65");
}

void EuroTest::euro_eq_euro()
{
    Euro a("5.12");
    Euro b("5.12");
    Euro c("-1.12");
    Euro d("0.12");

    QVERIFY( a == b );
    QVERIFY(a != c);
    QVERIFY(c != d);
}



QTEST_APPLESS_MAIN(EuroTest)

#include "tst_eurotest.moc"
