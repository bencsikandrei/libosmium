#ifndef OSMIUM_BUILDER_BUILDER_HELPER_HPP
#define OSMIUM_BUILDER_BUILDER_HELPER_HPP

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

#include <cstddef>
#include <initializer_list>
#include <map>

#include <osmium/builder/osm_object_builder.hpp>
#include <osmium/memory/buffer.hpp>

namespace osmium {

    namespace builder {

        inline const osmium::WayNodeList& build_way_node_list(osmium::memory::Buffer& buffer, std::initializer_list<osmium::NodeRef> nodes) {
            size_t pos = buffer.committed();
            {
                osmium::builder::WayNodeListBuilder wnl_builder(buffer);
                for (const auto& node_ref : nodes) {
                    wnl_builder.add_node_ref(node_ref);
                }
            }
            buffer.commit();
            return buffer.get<const osmium::WayNodeList>(pos);
        }

        inline const osmium::TagList& build_tag_list(osmium::memory::Buffer& buffer, std::initializer_list<std::pair<const char*, const char*>> tags) {
            size_t pos = buffer.committed();
            {
                osmium::builder::TagListBuilder tl_builder(buffer);
                for (const auto& p : tags) {
                    tl_builder.add_tag(p.first, p.second);
                }
            }
            buffer.commit();
            return buffer.get<const osmium::TagList>(pos);
        }

        inline const osmium::TagList& build_tag_list(osmium::memory::Buffer& buffer, const std::map<const char*, const char*>& tags) {
            size_t pos = buffer.committed();
            {
                osmium::builder::TagListBuilder tl_builder(buffer);
                for (const auto& p : tags) {
                    tl_builder.add_tag(p.first, p.second);
                }
            }
            buffer.commit();
            return buffer.get<const osmium::TagList>(pos);
        }

        inline const osmium::TagList& build_tag_list(osmium::memory::Buffer& buffer, std::function<void(osmium::builder::TagListBuilder&)> func) {
            size_t pos = buffer.committed();
            {
                osmium::builder::TagListBuilder tl_builder(buffer);
                func(tl_builder);
            }
            buffer.commit();
            return buffer.get<const osmium::TagList>(pos);
        }

    } // namespace builder

} // namespace osmium

#endif // OSMIUM_BUILDER_BUILDER_HELPER_HPP
