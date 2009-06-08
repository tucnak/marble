//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef GEODATALINESTRINGPRIVATE_H
#define GEODATALINESTRINGPRIVATE_H

#include "GeoDataGeometry_p.h"

namespace Marble
{

class GeoDataLineStringPrivate : public Marble::GeoDataGeometryPrivate
{
  public:
    GeoDataLineStringPrivate( TessellationFlags f )
         : m_dirtyRange( true ),
           m_dirtyBox( true ),
           m_tessellationFlags( f )
    {
    }

    GeoDataLineStringPrivate()
         : m_dirtyRange( true ),
           m_dirtyBox( true )
    {
    }

    virtual GeoDataGeometryPrivate* copy()
    { 
        GeoDataLineStringPrivate* copy = new GeoDataLineStringPrivate;
        *copy = *this;
        return copy;
    }

    virtual EnumGeometryId geometryId() const 
    {
        return GeoDataLineStringId;
    }

    QVector<GeoDataCoordinates> m_vector;

    QVector<GeoDataLineString>  m_rangeCorrected;
    bool                        m_dirtyRange;

    GeoDataLatLonAltBox         m_latLonAltBox;
    bool                        m_dirtyBox; // tells whether there have been changes to the
                                            // GeoDataPoints since the LatLonAltBox has 
                                            // been calculated. Saves performance. 
    TessellationFlags           m_tessellationFlags;
};

} // namespace Marble

#endif //GEODATALINESTRINGPRIVATE_H
