#include <boost/parser/parser.hpp>

#include "node.h"

namespace {

using namespace ast;

namespace bp = boost::parser;

// Requirements for variable name
auto const letters_and_underscore = bp::char_(
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "_"
);
auto const identifier = bp::lexeme[
    letters_and_underscore
    > *(letters_and_underscore | bp::char_('0', '9'))
];

// Lexemes to match value of variables
auto const raw_string = bp::lexeme[ +( bp::char_ - bp::blank ) ];
auto const value = bp::quoted_string | raw_string;

bp::symbols<NodeType> const cmp_op = {
    {"==",  NodeType::EQUAL},
    {"!=",  NodeType::NEQUAL}
};

bp::symbols<NodeType> const logical_op = {
    {"AND", NodeType::AND},
    {"OR",  NodeType::OR}
};

bp::symbols<NodeType> const assign_op = {
    {"=", NodeType::ASSIGN_OP}
};

bp::rule<class base_condition, NodePtr> const base_condition = "base_condition";
bp::rule<class base_assignment, NodePtr> const base_assignment = "base_assignment";
bp::rule<class assignment, NodePtr> const assignment = "assignment";
bp::rule<class condition, NodePtr> const condition = "condition";
bp::rule<class command, NodePtr> const command = "command";

auto const base_condition_def =
    (identifier > cmp_op > value)
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
        NodePtr conditionNode = makeNodePtr(NodeType::CONDITION);
        conditionNode->children.emplace_back(_attr(ctx));
        _val(ctx) = conditionNode;
    })];

auto const base_assignment_def =
    (identifier > assign_op > value)
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
        (bp::lit("GET") > condition)
        [([](auto& ctx){
            NodePtr node = makeNodePtr(NodeType::GET_REQUEST);
            node->children.emplace_back(_attr(ctx));

            _val(ctx) = node;
        })]

    |   (bp::lit("ADD") > assignment)
        [([](auto& ctx){
            NodePtr node = makeNodePtr(NodeType::ADD_REQUEST);
            node->children.emplace_back(_attr(ctx));

            _val(ctx) = node;
        })]

    |   (bp::lit("CHANGE") > bp::lit("TO") > assignment > "IF" > condition)
        [([](auto& ctx){
            auto const & attr = _attr(ctx);

            NodePtr node = makeNodePtr(NodeType::CHANGE_TO_REQUEST);

            node->children.emplace_back(std::get<0>(attr));
            node->children.emplace_back(std::get<1>(attr));

            _val(ctx) = node;
        })]

    |   ("DELETE" >> condition)
        [([](auto& ctx){
            NodePtr node = makeNodePtr(NodeType::DELETE_REQUEST);
            node->children.emplace_back(_attr(ctx));

            _val(ctx) = node;
        })];

BOOST_PARSER_DEFINE_RULES(
    base_condition,
    condition,
    base_assignment,
    assignment,
    command
);

template <typename RuleType>
NodePtr _parse(const std::string& input, bp::rule<RuleType, NodePtr> rule)
{
    std::ostringstream error_stream;
    bp::stream_error_handler error_handler("input", error_stream);
    auto const result = bp::parse(
        input,
        bp::with_error_handler(rule, error_handler),
        bp::ws
    );

    if (!result) {
        throw std::runtime_error(error_stream.str());
    }

    return *result;
}

} // anonymous namespace

namespace ast::parser
{

NodePtr parse(const std::string& input)
{
    return _parse(input, command);
}

}  // namespace ast::parser
