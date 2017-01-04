/*
Copyright (C) 2010-2016 Kristian Duske

This file is part of TrenchBroom.

TrenchBroom is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TrenchBroom is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
*/

#include "NodeCollection.h"

#include "Model/Brush.h"
#include "Model/Entity.h"
#include "Model/Group.h"
#include "Model/Layer.h"
#include "Model/Node.h"

#include <algorithm>
#include <cassert>

namespace TrenchBroom {
	namespace Model {
		class NodeCollection::AddNode : public NodeVisitor {
		private:
			NodeCollection& m_collection;
		public:
			AddNode(NodeCollection& collection) :
				m_collection(collection) {}
		private:
			void doVisit(World* world) {}
			void doVisit(Layer* layer) { m_collection.m_nodes.push_back(layer);  m_collection.m_layers.push_back(layer); }
			void doVisit(Group* group) { m_collection.m_nodes.push_back(group);  m_collection.m_groups.push_back(group); }
			void doVisit(Entity* entity) { m_collection.m_nodes.push_back(entity); m_collection.m_entities.push_back(entity); }
			void doVisit(Brush* brush) { m_collection.m_nodes.push_back(brush);  m_collection.m_brushes.push_back(brush); }
		};

		class NodeCollection::RemoveNode : public NodeVisitor {
		private:
			NodeCollection& m_collection;
			NodeArray::iterator m_nodeRem;
			LayerArray::iterator m_layerRem;
			GroupArray::iterator m_groupRem;
			EntityArray::iterator m_entityRem;
			BrushArray::iterator m_brushRem;
		public:
			RemoveNode(NodeCollection& collection) :
				m_collection(collection),
				m_nodeRem(std::end(m_collection.m_nodes)),
				m_layerRem(std::end(m_collection.m_layers)),
				m_groupRem(std::end(m_collection.m_groups)),
				m_entityRem(std::end(m_collection.m_entities)),
				m_brushRem(std::end(m_collection.m_brushes)) {}

			~RemoveNode() {
				m_collection.m_nodes.erase(m_nodeRem, std::end(m_collection.m_nodes));
				m_collection.m_layers.erase(m_layerRem, std::end(m_collection.m_layers));
				m_collection.m_groups.erase(m_groupRem, std::end(m_collection.m_groups));
				m_collection.m_entities.erase(m_entityRem, std::end(m_collection.m_entities));
				m_collection.m_brushes.erase(m_brushRem, std::end(m_collection.m_brushes));
			}
		private:
			void doVisit(World* world) {}
			void doVisit(Layer* layer) { remove(m_collection.m_nodes, m_nodeRem, layer);  remove(m_collection.m_layers, m_layerRem, layer); }
			void doVisit(Group* group) { remove(m_collection.m_nodes, m_nodeRem, group);  remove(m_collection.m_groups, m_groupRem, group); }
			void doVisit(Entity* entity) { remove(m_collection.m_nodes, m_nodeRem, entity); remove(m_collection.m_entities, m_entityRem, entity); }
			void doVisit(Brush* brush) { remove(m_collection.m_nodes, m_nodeRem, brush);  remove(m_collection.m_brushes, m_brushRem, brush); }

			template <typename V, typename E>
			void remove(V& collection, typename V::iterator& rem, E& elem) {
				rem = std::remove(std::begin(collection), rem, elem);
			}
		};

		bool NodeCollection::empty() const {
			return m_nodes.empty();
		}

		size_t NodeCollection::nodeCount() const {
			return m_nodes.size();
		}

		size_t NodeCollection::layerCount() const {
			return m_layers.size();
		}

		size_t NodeCollection::groupCount() const {
			return m_groups.size();
		}

		size_t NodeCollection::entityCount() const {
			return m_entities.size();
		}

		size_t NodeCollection::brushCount() const {
			return m_brushes.size();
		}

		bool NodeCollection::hasLayers() const {
			return !m_layers.empty();
		}

		bool NodeCollection::hasOnlyLayers() const {
			return !empty() && nodeCount() == layerCount();
		}

		bool NodeCollection::hasGroups() const {
			return !m_groups.empty();
		}

		bool NodeCollection::hasOnlyGroups() const {
			return !empty() && nodeCount() == groupCount();
		}

		bool NodeCollection::hasEntities() const {
			return !m_entities.empty();
		}

		bool NodeCollection::hasOnlyEntities() const {
			return !empty() && nodeCount() == entityCount();
		}

		bool NodeCollection::hasBrushes() const {
			return !m_brushes.empty();
		}

		bool NodeCollection::hasOnlyBrushes() const {
			return !empty() && nodeCount() == brushCount();
		}

		NodeArray::iterator NodeCollection::begin() {
			return std::begin(m_nodes);
		}

		NodeArray::iterator NodeCollection::end() {
			return std::end(m_nodes);
		}

		NodeArray::const_iterator NodeCollection::begin() const {
			return std::begin(m_nodes);
		}

		NodeArray::const_iterator NodeCollection::end() const {
			return std::end(m_nodes);
		}

		const NodeArray& NodeCollection::nodes() const {
			return m_nodes;
		}

		const LayerArray& NodeCollection::layers() const {
			return m_layers;
		}

		const GroupArray& NodeCollection::groups() const {
			return m_groups;
		}

		const EntityArray& NodeCollection::entities() const {
			return m_entities;
		}

		const BrushArray& NodeCollection::brushes() const {
			return m_brushes;
		}

		void NodeCollection::addNodes(const NodeArray& nodes) {
			AddNode visitor(*this);
			Node::accept(std::begin(nodes), std::end(nodes), visitor);
		}

		void NodeCollection::addNode(Node* node) {
			ensure(node != NULL, "node is null");
			AddNode visitor(*this);
			node->accept(visitor);
		}

		void NodeCollection::removeNodes(const NodeArray& nodes) {
			RemoveNode visitor(*this);
			Node::accept(std::begin(nodes), std::end(nodes), visitor);
		}

		void NodeCollection::removeNode(Node* node) {
			ensure(node != NULL, "node is null");
			RemoveNode visitor(*this);
			node->accept(visitor);
		}

		void NodeCollection::clear() {
			m_nodes.clear();
			m_layers.clear();
			m_groups.clear();
			m_entities.clear();
			m_brushes.clear();
		}
	}
}
