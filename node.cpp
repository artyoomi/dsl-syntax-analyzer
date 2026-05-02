#include "node.h"

namespace ast
{

/**
* Generic function to make operations with graph in BFS manner
*/
template<
    typename Preprocess,
    typename ProcessNode,
    typename OnChildEnqueue,
    typename Postprocess>
static void bfsTraverse(
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


static void toGraphviz(std::ostream& out, const NodePtr& root)
{
    const std::string graphName = "AST";

    bfsTraverse(
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

void dumpToGraphviz(const std::string& filename, const NodePtr& node)
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
