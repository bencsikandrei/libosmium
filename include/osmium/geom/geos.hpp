#ifndef OSMIUM_GEOM_GEOS_HPP
#define OSMIUM_GEOM_GEOS_HPP

/*

This file is part of Osmium (http://osmcode.org/libosmium).

Copyright 2013,2014 Jochen Topf <jochen@topf.org> and others (see README).

Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#define OSMIUM_COMPILE_WITH_CFLAGS_GEOS `geos-config --cflags`
#define OSMIUM_LINK_WITH_LIBS_GEOS `geos-config --libs`

#include <utility>

#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/LinearRing.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/Point.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/util/GEOSException.h>

#include <osmium/geom/factory.hpp>
#include <osmium/geom/coordinates.hpp>

namespace osmium {

    struct geos_geometry_error : public geometry_error {

        geos_geometry_error() :
            geometry_error("geometry creation failed in GEOS library, see nested exception for details") {
        }

    }; // struct geos_geometry_error

    namespace geom {

        namespace detail {

            class GEOSFactoryImpl {

                geos::geom::PrecisionModel m_precision_model;
                geos::geom::GeometryFactory m_geos_factory;

                std::unique_ptr<geos::geom::CoordinateSequence> m_coordinate_sequence;
                std::vector<std::unique_ptr<geos::geom::LinearRing>> m_rings;
                std::vector<std::unique_ptr<geos::geom::Polygon>> m_polygons;

            public:

                typedef std::unique_ptr<geos::geom::Point>        point_type;
                typedef std::unique_ptr<geos::geom::LineString>   linestring_type;
                typedef std::unique_ptr<geos::geom::Polygon>      polygon_type;
                typedef std::unique_ptr<geos::geom::MultiPolygon> multipolygon_type;
                typedef std::unique_ptr<geos::geom::LinearRing>   ring_type;

                explicit GEOSFactoryImpl(int srid = -1) :
                    m_precision_model(),
                    m_geos_factory(&m_precision_model, srid) {
                }

                /* Point */

                point_type make_point(const osmium::geom::Coordinates& xy) const {
                    try {
                        return point_type(m_geos_factory.createPoint(geos::geom::Coordinate(xy.x, xy.y)));
                    } catch (geos::util::GEOSException&) {
                        std::throw_with_nested(osmium::geos_geometry_error());
                    }
                }

                /* LineString */

                void linestring_start() {
                    try {
                        m_coordinate_sequence.reset(m_geos_factory.getCoordinateSequenceFactory()->create(static_cast<size_t>(0), 2));
                    } catch (geos::util::GEOSException&) {
                        std::throw_with_nested(osmium::geos_geometry_error());
                    }
                }

                void linestring_add_location(const osmium::geom::Coordinates& xy) {
                    try {
                        m_coordinate_sequence->add(geos::geom::Coordinate(xy.x, xy.y));
                    } catch (geos::util::GEOSException&) {
                        std::throw_with_nested(osmium::geos_geometry_error());
                    }
                }

                linestring_type linestring_finish(size_t /* num_points */) {
                    try {
                        return linestring_type(m_geos_factory.createLineString(m_coordinate_sequence.release()));
                    } catch (geos::util::GEOSException&) {
                        std::throw_with_nested(osmium::geos_geometry_error());
                    }
                }

                /* MultiPolygon */

                void multipolygon_start() {
                    m_polygons.clear();
                }

                void multipolygon_polygon_start() {
                    m_rings.clear();
                }

                void multipolygon_polygon_finish() {
                    try {
                        assert(!m_rings.empty());
                        auto inner_rings = new std::vector<geos::geom::Geometry*>;
                        std::transform(std::next(m_rings.begin(), 1), m_rings.end(), std::back_inserter(*inner_rings), [](std::unique_ptr<geos::geom::LinearRing>& r) {
                            return r.release();
                        });
                        m_polygons.emplace_back(m_geos_factory.createPolygon(m_rings[0].release(), inner_rings));
                        m_rings.clear();
                    } catch (geos::util::GEOSException&) {
                        std::throw_with_nested(osmium::geos_geometry_error());
                    }
                }

                void multipolygon_outer_ring_start() {
                    try {
                        m_coordinate_sequence.reset(m_geos_factory.getCoordinateSequenceFactory()->create(static_cast<size_t>(0), 2));
                    } catch (geos::util::GEOSException&) {
                        std::throw_with_nested(osmium::geos_geometry_error());
                    }
                }

                void multipolygon_outer_ring_finish() {
                    try {
                        m_rings.emplace_back(m_geos_factory.createLinearRing(m_coordinate_sequence.release()));
                    } catch (geos::util::GEOSException&) {
                        std::throw_with_nested(osmium::geos_geometry_error());
                    }
                }

                void multipolygon_inner_ring_start() {
                    try {
                        m_coordinate_sequence.reset(m_geos_factory.getCoordinateSequenceFactory()->create(static_cast<size_t>(0), 2));
                    } catch (geos::util::GEOSException&) {
                        std::throw_with_nested(osmium::geos_geometry_error());
                    }
                }

                void multipolygon_inner_ring_finish() {
                    try {
                        m_rings.emplace_back(m_geos_factory.createLinearRing(m_coordinate_sequence.release()));
                    } catch (geos::util::GEOSException&) {
                        std::throw_with_nested(osmium::geos_geometry_error());
                    }
                }

                void multipolygon_add_location(const osmium::geom::Coordinates& xy) {
                    try {
                        m_coordinate_sequence->add(geos::geom::Coordinate(xy.x, xy.y));
                    } catch (geos::util::GEOSException&) {
                        std::throw_with_nested(osmium::geos_geometry_error());
                    }
                }

                multipolygon_type multipolygon_finish() {
                    try {
                        auto polygons = new std::vector<geos::geom::Geometry*>;
                        std::transform(m_polygons.begin(), m_polygons.end(), std::back_inserter(*polygons), [](std::unique_ptr<geos::geom::Polygon>& p) {
                            return p.release();
                        });
                        m_polygons.clear();
                        return multipolygon_type(m_geos_factory.createMultiPolygon(polygons));
                    } catch (geos::util::GEOSException&) {
                        std::throw_with_nested(osmium::geos_geometry_error());
                    }
                }

            }; // class GEOSFactoryImpl

        } // namespace detail

        template <class TProjection = IdentityProjection>
        using GEOSFactory = GeometryFactory<osmium::geom::detail::GEOSFactoryImpl, TProjection>;

    } // namespace geom

} // namespace osmium

#endif // OSMIUM_GEOM_GEOS_HPP
