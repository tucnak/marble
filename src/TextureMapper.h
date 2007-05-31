/**
 * This file is part of the Marble Desktop Globe.
 *
 * Copyright 2005-2007 Torsten Rahn <tackat@kde.org>"
 * Copyright 2007      Inge Wallin  <ingwa@kde.org>"
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */


#ifndef __MARBLE__TEXTUREMAPPER_H
#define __MARBLE__TEXTUREMAPPER_H


#include <QtCore/QString>
#include <QtGui/QColor>

#include "Quaternion.h"


class QImage;
class TextureTile;
class TileLoader;


/*
 * @short Texture mapping onto a sphere
 *
 * This class provides a fast way to map textures onto a sphere
 * without making use of hardware acceleration. 
 *
 * @author Torsten Rahn <rahn@kde.org>
 */

class TextureMapper
{
 public:
    TextureMapper( const QString& path );
    virtual ~TextureMapper();

    void setMap( const QString& path );
    void setMaxTileLevel( int level ){ m_maxTileLevel = level; }
    void resizeMap(const QImage* canvasImage);
    void mapTexture(QImage* canvasImage, const int&, Quaternion& planetAxis);
    void selectTileLevel(const int& radius);

 protected:
    void pixelValueApprox(const float& lng, const float& lat, 
                          QRgb* scanLine);
    void pixelValue(const float& lng, const float& lat, QRgb* scanLine);
    void nextTile();

    void tileLevelInit( int tileLevel );

    int          m_posX;
    int          m_posY;

    TileLoader  *m_tileLoader;
    QRgb        *m_scanLine;
    QRgb        *m_fastScanLine;

    int          m_maxTileLevel;
    bool         m_interpolate;
    int          m_nBest;

    int    m_n;
    float  m_ninv;

    int    m_x;
    int    m_y;
    int    m_z;

    float  m_qr;
    float  m_qx;
    float  m_qy;
    float  m_qz;

    int    m_imageHalfWidth;
    int    m_imageHalfHeight;
    int    m_imageRadius;

    float  m_prevLat;
    float  m_prevLng;

    int    m_tilePosX;
    int    m_tilePosY;

    int    m_fullRangeLng;
    int    m_halfRangeLat;
    float  m_halfRangeLng;
    float  m_quatRangeLat;

    int    m_fullNormLng;
    int    m_halfNormLat;
    float  m_halfNormLng;
    float  m_quatNormLat;

    float  m_rad2PixelX;
    float  m_rad2PixelY;

    TextureTile  *m_tile;
    int           m_tileLevel;
};


#endif // __MARBLE__TEXTUREMAPPER_H
