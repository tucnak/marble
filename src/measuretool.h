//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

//
// MeasureTool enables Marble to set and display measure points
//


#ifndef MEASURETOOL_H
#define MEASURETOOL_H


#include <QtCore/QVector>
#include <QtGui/QFont>

#include "GeoPoint.h"
#include "placemark.h"
#include "clippainter.h"


class MeasureTool : public QObject
{
    Q_OBJECT

 public:
    MeasureTool( QObject *parent = 0 );

    void  paintMeasurePoints( ClipPainter*, int, int, int, Quaternion, bool );
    void  setLineColor( QColor linecolor ) { m_linecolor = linecolor; }
    void  paintTotalDistanceLabel( ClipPainter*, int, int, float );

 public slots:
    void addMeasurePoint( float lng, float lat ) {
        m_pMeasurePointList << new GeoPoint( lng, lat );
    }
    void removeMeasurePoints() {
        m_pMeasurePointList.clear();
    }

 protected:
    bool  testbug(); 
    void  paintMark( ClipPainter* painter, int x, int y );
    void  drawDistancePath( ClipPainter* painter, Quaternion, Quaternion, 
                            int imgrx, int imgry, int radius, 
                            bool antialiasing );

 protected:
    float   m_totalDistance;

    QFont   m_font_regular;
    int     m_fontheight;
    int     m_fontascent;

    QColor  m_linecolor;

    QVector<GeoPoint*>  m_pMeasurePointList;

    QPen    m_pen;

    QPixmap m_empty;

    bool    m_useworkaround;
};


#endif // MEASURETOOL_H
