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

enum class NodeType {
    // Request types
    GET_REQUEST,
    ADD_REQUEST,
    CHANGE_TO_REQUEST,
    DELETE_REQUEST,

    // Conditions
    CONDITION,
    AND,
    OR,
    EQUAL,
    NEQUAL,

    // Assignments
    ASSIGNMENT,
    ASSIGN_OP,

    // Leaves with identifiers and values
    TOKEN
};

// Factory function that forwards arguments to the appropriate constructor
template<typename... Args>
NodePtr makeNodePtr(Args&&... args)
{
    return std::make_shared<Node>(std::forward<Args>(args)...);
}

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

/** Format of resulting .dot file is as follows
    ```
    digraph AST {
        node0 [label="EQUAL"];
        node1 [label="TOKEN: typing"];
        node2 [label="TOKEN: abc"];

        node0 -> node1;
        node0 -> node2;
    }
    ```
 */
void dumpToGraphviz(const std::string& filename, const NodePtr& node);

}  // namespace ast
