//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#ifndef MARBLE_ECLIPSESMODEL_H
#define MARBLE_ECLIPSESMODEL_H

#include <QtCore/QObject>

class EclSolar;

namespace Marble
{

class MarbleClock;
class EclipsesItem;

class EclipsesModel : public QObject
{
    Q_OBJECT
public:
    EclipsesModel( const MarbleClock *clock, QObject *parent = 0 );
    ~EclipsesModel();

    void setYear( int year );
    int year() const;

    void addItem( EclipsesItem *item );
    QList<EclipsesItem*> items() const;
    void clear();

    void update();

private:
    void updateEclipses();

    const MarbleClock *m_clock;
    EclSolar *m_ecps;
    QList<EclipsesItem*> m_items;
    int m_currentYear;
};

}

#endif // MARBLE_ECLIPSESMODEL_H