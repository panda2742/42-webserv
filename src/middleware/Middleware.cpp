#include "middleware/Middleware.hpp"

namespace mdw
{
// #########################################################

MiddlewareException::MiddlewareException(const std::string& msg): std::runtime_error(msg) {}

Middleware::Middleware(cfg::HttpConfig& conf): conf_(conf) {}

Middleware::~Middleware(void) {}

bool	Middleware::checkFormat(const std::vector<cfg::Lexer::TokenNode>& nodes) const throw(MiddlewareException)
{
	int	open_brackets = 0;
	for (std::vector<cfg::Lexer::TokenNode>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		const cfg::Lexer::TokenNode&	node = (*it);
		switch (node.type)
		{
			case cfg::Lexer::TokenSymbolOpen:
				++open_brackets;
				break;
			case cfg::Lexer::TokenSymbolClose:
				if (open_brackets == 0)
					throw MiddlewareException("unexpected token '}'");
				--open_brackets;
			default:
				break;
		}
	}
	if (open_brackets < 0)
		throw MiddlewareException("there is too much closing brackets");
	else if (open_brackets > 0)
		throw MiddlewareException("there is too much opening brackets");
	return true;
}

// #########################################################
};