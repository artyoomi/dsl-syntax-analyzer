#pragma once

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <queue>
#include <format>

#include <magic_enum/magic_enum.hpp>

namespace ast {

struct Node;

using NodePtr = std::shared_ptr<Node>;

// Factory function that forwards arguments to the appropriate constructor
template<typename... Args>
NodePtr makeNodePtr(Args&&... args)
{
    return std::make_shared<Node>(std::forward<Args>(args)...);
}

enum class NodeType {
    // Request types
    GET_REQUEST,
    ADD_REQUEST,
    CHANGE_TO_REQUEST,
    DELETE_REQUEST,

    // Conditions
    CONDITION,
    EQUAL,
    NEQUAL,

    // Assignments
    ASSIGNMENT,
    ASSIGN_OP,

    // Leaves with identifiers and values
    TOKEN
};

struct Node {
    unsigned                   id;
    NodeType                   type;
    std::optional<std::string> token;
    std::vector<NodePtr>       children;

    Node()                  = delete;
    Node(const Node& other) = delete;
    Node(Node&& other)      = delete;

    explicit Node(NodeType type)
    : id{Node::generateId()}, type{type}, token{std::nullopt}
    {
    }

    explicit Node(NodeType type, const std::string & token)
    : id{Node::generateId()}, type{type}, token{token}
    {
    }

    // Generates and assigns unique id for each new node
    static unsigned generateId()
    {
        static unsigned id = 0;
        return ++id;
    }

    // friend std::ostream& operator<<(std::ostream& os, const NodePtr& obj);
};

template<
    typename Preprocess,
    typename ProcessNode,
    typename OnChildEnqueue,
    typename Postprocess>
static void nodeTraverse(
    const NodePtr& root,
    Preprocess preprocess,
    ProcessNode processNode,
    OnChildEnqueue onChildEnqueue,
    Postprocess postprocess)
{
    std::queue<NodePtr> q;
    q.push(root);

    preprocess(root);

    while (!q.empty()) {
        NodePtr node = q.front();
        q.pop();

        processNode(node);

        for (auto const& child : node->children) {
            onChildEnqueue(node, child);
            q.push(child);
        }
    }

    postprocess(root);
}

// <TODO>: need to be rewritten with nodeTraverse function
// static void nodeToString(const NodePtr& root, std::ostream& out, int indentLevel = 0) {
//     static std::string indentSymbol = "  ";

//     for (int i = 0; i < indentLevel; ++i) {
//         out << indentSymbol;
//     }
//     out << magic_enum::enum_name(root->type);

//     if (root->token) {
//         out << std::format(": {}", *root->token);
//     }
//     out << '\n';

//     for (auto & child : root->children) {nodeToString(child, out, indentLevel + 1); }
// }

// std::ostream& operator<<(std::ostream& os, const NodePtr& obj) {
//     nodeToString(obj, os, 0);
//     return os;
// }

/** Format of resulting .dot file
 * digraph AST {
 *   node0 [label="EQUAL"];
 *   node1 [label="TOKEN: typing"];
 *   node2 [label="TOKEN: abc"];
 *
 *   node0 -> node1;
 *   node0 -> node2;
 * }
 */
static void toGraphviz(std::ostream& out, NodePtr root)
{
    const std::string graphName = "AST";

    nodeTraverse(
        root,

        [&](auto) {
            out << std::format("digraph {} {{\n", graphName);
        },

        [&](const NodePtr& node) {
            // Create graph node
            out << std::format("\tnode{} [label=\"{}", node->id, magic_enum::enum_name(node->type));
            if (node->token) {
                out << std::format(": {}", *node->token);
            }
            out << "\"];\n";
        },

        [&](const NodePtr& parent, const NodePtr& child) {
            out << std::format("\tnode{} -> node{};\n", parent->id, child->id);
        },

        [&](auto) {
            out << "}\n";
        }
    );
}

void dumpToGraphviz(const std::string& filename, NodePtr node)
{
    std::ofstream file;

    try {
        file.open(filename);
        toGraphviz(file, node);
    } catch (const std::ifstream::failure& e) {
        throw std::runtime_error("Failed to open file " + filename + ": " + e.what());
    } catch (const std::exception /* ex */) {
        throw std::runtime_error("Unhandled exception");
    }

    file.close();
}

}  // namespace ast
