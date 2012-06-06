//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>
// Copyright 2011-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "PlacemarkLayer.h"

#include <QtCore/QModelIndex>
#include <QtCore/QPoint>
#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QPalette>
#include <QtGui/QPixmap>

#include "MarbleDebug.h"
#include "AbstractProjection.h"
#include "GeoDataStyle.h"
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "VisiblePlacemark.h"

using namespace Marble;

PlacemarkLayer::PlacemarkLayer( QAbstractItemModel *placemarkModel,
                                QItemSelectionModel *selectionModel,
                                MarbleClock *clock,
                                QObject *parent ) :
    QObject( parent ),
    m_layout( placemarkModel, selectionModel, clock ),
    m_defaultLabelColor( Qt::black )
{
    m_useXWorkaround = testXBug();
    mDebug() << "Use workaround: " << ( m_useXWorkaround ? "1" : "0" );

    connect( &m_layout, SIGNAL( repaintNeeded() ), SIGNAL( repaintNeeded() ) );
}

PlacemarkLayer::~PlacemarkLayer()
{
}

QStringList PlacemarkLayer::renderPosition() const
{
    return QStringList() << "HOVERS_ABOVE_SURFACE";
}

qreal PlacemarkLayer::zValue() const
{
    return 2.0;
}

bool PlacemarkLayer::render( GeoPainter *geoPainter, ViewportParams *viewport,
                               const QString &renderPos, GeoSceneLayer *layer )
{
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    QVector<VisiblePlacemark*> visiblePlacemarks = m_layout.generateLayout( viewport );
    // draw placemarks less important first
    QVector<VisiblePlacemark*>::const_iterator visit = visiblePlacemarks.constEnd();
    QVector<VisiblePlacemark*>::const_iterator itEnd = visiblePlacemarks.constBegin();
    VisiblePlacemark *mark;

    QPainter *const painter = geoPainter;

    while ( visit != itEnd ) {
        --visit;
        mark = *visit;

	if ( mark->labelPixmap().isNull() ) {
            drawLabelPixmap( mark );
	}

        int symbolX = mark->symbolPosition().x();
        int textX =   mark->labelRect().x();
        QRect labelRect( mark->labelRect().toRect() );
        QPoint symbolPos( mark->symbolPosition() );

        // when the map is such zoomed out that a given place
        // appears many times, we draw one placemark at each
        if (viewport->currentProjection()->repeatX() ) {
            for ( int i = symbolX % (4 * viewport->radius());
                 i <= viewport->width();
                 i += 4 * viewport->radius() )
            {
                labelRect.moveLeft(i - symbolX + textX );
                symbolPos.setX( i );

                painter->drawPixmap( symbolPos, mark->symbolPixmap() );
                painter->drawPixmap( labelRect, mark->labelPixmap() );
            }
        } else { // simple case, one draw per placemark
            painter->drawPixmap( symbolPos, mark->symbolPixmap() );
            painter->drawPixmap( labelRect, mark->labelPixmap() );
        }
    }

    return true;
}

QVector<const GeoDataPlacemark *> PlacemarkLayer::whichPlacemarkAt( const QPoint &pos )
{
    return m_layout.whichPlacemarkAt( pos );
}

void PlacemarkLayer::setDefaultLabelColor( const QColor& color )
{
    m_defaultLabelColor = color;
}

void PlacemarkLayer::setShowPlaces( bool show )
{
    m_layout.setShowPlaces( show );
}

void PlacemarkLayer::setShowCities( bool show )
{
    m_layout.setShowCities( show );
}

void PlacemarkLayer::setShowTerrain( bool show )
{
    m_layout.setShowTerrain( show );
}

void PlacemarkLayer::setShowOtherPlaces( bool show )
{
    m_layout.setShowOtherPlaces( show );
}

void PlacemarkLayer::setShowLandingSites( bool show )
{
    m_layout.setShowLandingSites( show );
}

void PlacemarkLayer::setShowCraters( bool show )
{
    m_layout.setShowCraters( show );
}

void PlacemarkLayer::setShowMaria( bool show )
{
    m_layout.setShowMaria( show );
}

void PlacemarkLayer::requestStyleReset()
{
    m_layout.requestStyleReset();
}

inline void PlacemarkLayer::drawLabelText(QPainter &labelPainter, const QString &text,
                                            const QFont &labelFont, LabelStyle labelStyle, const QColor &color )
{
    QFont font = labelFont;
    QFontMetrics metrics = QFontMetrics( font );
    int fontAscent = metrics.ascent();

    switch ( labelStyle ) {
    case Selected: {
        labelPainter.setPen( color );
        labelPainter.setFont( font );
        QRect textRect( 0, 0, metrics.width( text ), metrics.height() );
        labelPainter.fillRect( textRect, QApplication::palette().highlight() );
        labelPainter.setPen( QPen( QApplication::palette().highlightedText(), 1 ) );
        labelPainter.drawText( 0, fontAscent, text );
        break;
    }
    case Glow: {
        font.setWeight( 75 );
        fontAscent = QFontMetrics( font ).ascent();

        QPen outlinepen( color == QColor( Qt::white ) ? Qt::black : Qt::white );
        outlinepen.setWidthF( s_labelOutlineWidth );
        QBrush  outlinebrush( color );

        QPainterPath outlinepath;

        const QPointF  baseline( s_labelOutlineWidth / 2.0, fontAscent );
        outlinepath.addText( baseline, font, text );
        labelPainter.setRenderHint( QPainter::Antialiasing, true );
        labelPainter.setPen( outlinepen );
        labelPainter.setBrush( outlinebrush );
        labelPainter.drawPath( outlinepath );
        labelPainter.setPen( Qt::NoPen );
        labelPainter.drawPath( outlinepath );
        labelPainter.setRenderHint( QPainter::Antialiasing, false );
        break;
    }
    default: {
        labelPainter.setPen( color );
        labelPainter.setFont( font );
        labelPainter.drawText( 0, fontAscent, text );
    }
    }
}

inline void PlacemarkLayer::drawLabelPixmap( VisiblePlacemark *mark )
{

    QPainter labelPainter;
    QPixmap labelPixmap;

    const GeoDataPlacemark *placemark = mark->placemark();
    Q_ASSERT(placemark);
    const GeoDataStyle* style = placemark->style();

    QString labelName = placemark->name();
    QRect  labelRect  = mark->labelRect().toRect();
    if ( !labelRect.isValid() ) {
        mark->setLabelPixmap( QPixmap() );
        return;
    }
    
    QFont  labelFont  = style->labelStyle().font();
    QColor labelColor = style->labelStyle().color();

    // FIXME: To be removed after MapTheme / KML refactoring
    if ( ( labelColor == Qt::black || labelColor == QColor( "#404040" ) )
	 && m_defaultLabelColor != Qt::black )
        labelColor = m_defaultLabelColor;

    LabelStyle labelStyle = Normal;
    if ( mark->selected() ) {
        labelStyle = Selected;
    } else if ( style->labelStyle().glow() ) {
        labelStyle = Glow;
    }


    // Due to some XOrg bug this requires a workaround via
    // QImage in some cases (at least with Qt 4.2).
    if ( !m_useXWorkaround ) {
        labelPixmap = QPixmap( labelRect.size() );
        labelPixmap.fill( Qt::transparent );

        labelPainter.begin( &labelPixmap );

        drawLabelText( labelPainter, labelName, labelFont, labelStyle, labelColor );

        labelPainter.end();
    } else {

        QImage image( labelRect.size(),
                      QImage::Format_ARGB32_Premultiplied );
        image.fill( 0 );

        labelPainter.begin( &image );

        drawLabelText( labelPainter, labelName, labelFont, labelStyle, labelColor );

        labelPainter.end();

        labelPixmap = QPixmap::fromImage( image );
    }

    mark->setLabelPixmap( labelPixmap );
}


// Test if there a bug in the X server which makes 
// text fully transparent if it gets written on 
// QPixmaps that were initialized by filling them 
// with Qt::transparent

bool PlacemarkLayer::testXBug()
{
    QString  testchar( "K" );
    QFont    font( "Sans Serif", 10 );

    int fontheight = QFontMetrics( font ).height();
    int fontwidth  = QFontMetrics( font ).width(testchar);
    int fontascent = QFontMetrics( font ).ascent();

    QPixmap  pixmap( fontwidth, fontheight );
    pixmap.fill( Qt::transparent );

    QPainter textpainter;
    textpainter.begin( &pixmap );
    textpainter.setPen( QColor( 0, 0, 0, 255 ) );
    textpainter.setFont( font );
    textpainter.drawText( 0, fontascent, testchar );
    textpainter.end();

    QImage image = pixmap.toImage();

    for ( int x = 0; x < fontwidth; ++x ) {
        for ( int y = 0; y < fontheight; ++y ) {
            if ( qAlpha( image.pixel( x, y ) ) > 0 )
                return false;
        }
    }

    return true;
}

#include "PlacemarkLayer.moc"
