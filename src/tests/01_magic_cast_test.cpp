#include <string>
#include <stdexcept>
#include "config/MagicCast.hpp"

void	magic_cast_test(void)
{
	std::string		str("Hello world!");
	try { cfg::magic_cast<std::string>(str); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<unsigned int>(str); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::vector<std::string> >(str); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::vector<unsigned int> >(str); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::map<unsigned int, std::string> >(str); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::map<unsigned int, std::vector<std::string> > >(str); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	std::cout << std::endl;

	unsigned int	uintv = 42;
	try { cfg::magic_cast<std::string>(uintv); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<unsigned int>(uintv); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::vector<std::string> >(uintv); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::vector<unsigned int> >(uintv); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::map<unsigned int, std::string> >(uintv); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::map<unsigned int, std::vector<std::string> > >(uintv); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	std::cout << std::endl;

	std::vector<std::string>	vector_str;
	vector_str.push_back("Hello");
	vector_str.push_back("world!");
	try { cfg::magic_cast<std::string>(vector_str); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<unsigned int>(vector_str); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::vector<std::string> >(vector_str); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::vector<unsigned int> >(vector_str); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::map<unsigned int, std::string> >(vector_str); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::map<unsigned int, std::vector<std::string> > >(vector_str); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	std::cout << std::endl;

	std::vector<unsigned int>	vector_uintv;
	vector_uintv.push_back(40);
	vector_uintv.push_back(2);
	try { cfg::magic_cast<std::string>(vector_uintv); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<unsigned int>(vector_uintv); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::vector<std::string> >(vector_uintv); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::vector<unsigned int> >(vector_uintv); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::map<unsigned int, std::string> >(vector_uintv); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::map<unsigned int, std::vector<std::string> > >(vector_uintv); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	std::cout << std::endl;

	std::map<unsigned int, std::string>	map1;
	map1.insert(std::make_pair(404, str));
	try { cfg::magic_cast<std::string>(map1); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<unsigned int>(map1); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::vector<std::string> >(map1); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::vector<unsigned int> >(map1); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::map<unsigned int, std::string> >(map1); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::map<unsigned int, std::vector<std::string> > >(map1); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	std::cout << std::endl;

	std::map<unsigned int, std::vector<std::string> >	map2;
	map2.insert(std::make_pair(404, vector_str));
	try { cfg::magic_cast<std::string>(map2); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<unsigned int>(map2); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::vector<std::string> >(map2); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::vector<unsigned int> >(map2); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::map<unsigned int, std::string> >(map2); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::map<unsigned int, std::vector<std::string> > >(map2); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }
}