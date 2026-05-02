#pragma once

#include "node.h"

using NodePtr = ast::NodePtr;

namespace ast::parser
{

NodePtr parse(const std::string& input);

}
