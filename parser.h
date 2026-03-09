#pragma once

#include <boost/parser/parser.hpp>

#include "node.h"

namespace ast::parser {

// using namespace ast;
namespace bp = boost::parser;

// Requirements like for variable name
auto const alpha = bp::lexeme[ +(bp::char_ - bp::blank - bp::control - bp::punct) ];
auto const identifier = alpha;

// <TODO>: Add continuous string as an argument
auto const value = bp::quoted_string;

bp::symbols<NodeType> const cmp_op = {
    {"==", NodeType::EQUAL},
    {"!=", NodeType::NEQUAL}
};

bp::symbols<NodeType> const assign_op = {
    {"=", NodeType::ASSIGN_OP}
};

bp::rule<class base_condition, NodePtr> const base_condition = "base_condition";
bp::rule<class base_assignment, std::shared_ptr<Node>> const base_assignment = "base_assignment";
bp::rule<class assignment, std::shared_ptr<Node>> const assignment = "assignment";
bp::rule<class condition, std::shared_ptr<Node>> const condition = "condition";
bp::rule<class command, std::shared_ptr<Node>> const command = "command";

auto const base_condition_def =
    (identifier >> cmp_op >> value)
    [([](auto & ctx){
        auto const & attr = _attr(ctx);

        NodePtr node = makeNodePtr(std::get<1>(attr));
        node->children.emplace_back(makeNodePtr(NodeType::TOKEN, std::get<0>(attr)));
        node->children.emplace_back(makeNodePtr(NodeType::TOKEN, std::get<2>(attr)));

        _val(ctx) = node;
    })];

auto const condition_def =
    (base_condition)
    [([](auto & ctx){
        // auto const & attr = _attr(ctx);

        NodePtr node = makeNodePtr(NodeType::CONDITION);
        node->children.emplace_back(_attr(ctx));

        _val(ctx) = node;
    })];

auto const base_assignment_def =
    (identifier >> assign_op >> value)
    [([](auto & ctx){
        auto const & attr = _attr(ctx);

        NodePtr node = makeNodePtr(std::get<1>(attr));
        node->children.emplace_back(makeNodePtr(NodeType::TOKEN, std::get<0>(attr)));
        node->children.emplace_back(makeNodePtr(NodeType::TOKEN, std::get<2>(attr)));

        _val(ctx) = node;
    })];

auto const assignment_def =
    (base_assignment % ",")
    [([](auto & ctx){
        auto const & attr = _attr(ctx);

        NodePtr assignmentNode = makeNodePtr(NodeType::ASSIGNMENT);

        for (auto node : attr) {
            assignmentNode->children.emplace_back(node);
        }

        _val(ctx) = assignmentNode;
    })];

auto const command_def =
    // Maybe need to rewrite all GET, ADD, etc. literals to bp::symbols
        (bp::lit("GET") >> condition)
        [([](auto& ctx){
            NodePtr node = makeNodePtr(NodeType::GET_REQUEST);
            node->children.emplace_back(_attr(ctx));

            _val(ctx) = node;
        })]

    |   (bp::lit("ADD") >> assignment)
        [([](auto& ctx){
            NodePtr node = makeNodePtr(NodeType::ADD_REQUEST);
            node->children.emplace_back(_attr(ctx));

            _val(ctx) = node;
        })]

    |   (bp::lit("CHANGE") >> bp::lit("TO") >> assignment >> "IF" >> condition)
        [([](auto& ctx){
            auto const & attr = _attr(ctx);

            NodePtr node = makeNodePtr(NodeType::CHANGE_TO_REQUEST);

            node->children.emplace_back(std::get<0>(attr));
            node->children.emplace_back(std::get<1>(attr));

            _val(ctx) = node;
        })]

    |   ("DELETE" >> condition)
        [([](auto& ctx){
            NodePtr node = makeNodePtr(NodeType::GET_REQUEST);
            node->children.emplace_back(_attr(ctx));

            _val(ctx) = node;
        })];

BOOST_PARSER_DEFINE_RULES(base_condition, condition, base_assignment, assignment, command);

NodePtr parse(const std::string& input)
{
    auto const result = bp::parse(
        input,
        command,
        bp::ws
    );

    if (!result) {
        throw std::runtime_error("Parsing failed!");
    }

    return *result;
}

}  // namespace ast::parser
