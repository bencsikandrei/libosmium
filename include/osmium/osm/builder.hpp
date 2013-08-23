#ifndef OSMIUM_OSM_BUILDER_HPP
#define OSMIUM_OSM_BUILDER_HPP

/*

This file is part of Osmium (http://osmcode.org/osmium).

Copyright 2013 Jochen Topf <jochen@topf.org> and others (see README).

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

#include <osmium/memory/builder.hpp>
#include <osmium/osm.hpp>

namespace osmium {

    namespace memory {

        typedef ObjectBuilder<Node> NodeBuilder;
        typedef ObjectBuilder<Way> WayBuilder;
        typedef ObjectBuilder<Relation> RelationBuilder;

        class TagListBuilder : public Builder {

        public:

            TagListBuilder(Buffer& buffer, Builder* parent=nullptr) :
                Builder(buffer, parent, sizeof(TagList), item_type::tag_list) {
            }

            TagList& object() {
                return static_cast<TagList&>(item());
            }

            void add_tag(const char* key, const char* value) {
                add_size(append(key) + append(value));
            }

        }; // class TagListBuilder

        class WayNodeListBuilder : public Builder {

        public:

            WayNodeListBuilder(Buffer& buffer, Builder* parent=nullptr) :
                Builder(buffer, parent, sizeof(WayNodeList), item_type::way_node_list) {
            }

            WayNodeList& object() {
                return static_cast<WayNodeList&>(item());
            }

            void add_way_node(const WayNode& way_node) {
                new (reserve_space_for<osmium::WayNode>()) osmium::WayNode(way_node);
                add_size(sizeof(osmium::WayNode));
            }

            void add_way_node(const object_id_type ref, const Location location=Location()) {
                add_way_node(WayNode(ref, location));
            }

        }; // class WayNodeListBuilder

        class RelationMemberListBuilder : public Builder {

        public:

            RelationMemberListBuilder(Buffer& buffer, Builder* parent=nullptr) :
                Builder(buffer, parent, sizeof(RelationMemberList), item_type::relation_member_list) {
            }

            RelationMemberList& object() {
                return static_cast<RelationMemberList&>(item());
            }

            void add_member(osmium::item_type type, object_id_type ref, const char* role, const osmium::Object* full_member = nullptr) {
                new (reserve_space_for<osmium::RelationMember>()) osmium::RelationMember(ref, type, full_member != nullptr);
                add_size(sizeof(RelationMember));
                add_string(role);
                if (full_member) {
                    add_item(full_member);
                }
            }

        }; // class RelationMemberListBuilder

    } // namespace memory

} // namespace osmium

#endif // OSMIUM_OSM_BUILDER_HPP
