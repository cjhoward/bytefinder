/*
 * Copyright (C) 2015-2017  Christopher J. Howard
 *
 * This file is part of Bytefinder.
 *
 * Bytefinder is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Bytefinder is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Bytefinder.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#define WILDCARD 0x100

int main(int argc, const char* argv[])
{
	if (argc != 3)
	{
		std::cerr << "Usage: bytefinder <file> <string>" << std::endl;
		return EXIT_FAILURE;
	}

	std::ifstream file(argv[1], std::ios::binary);
	if (!file.is_open())
	{
		std::cerr << "Failed to open file \"" << argv[1] << "\"" << std::endl;
		return EXIT_FAILURE;
	}

	// Form search string
	std::string search;
	int prefixed_wildcards = 0;
	for (int i = 0; i < std::strlen(argv[2]); ++i)
	{
		if (argv[2][i] != '*')
		{
			search = &argv[2][i];
			break;
		}

		++prefixed_wildcards;
	}

	// Form difference table
	std::vector<int> differences;
	int known_differences = 0;
	int base = -1;
	bool escape = false;
	for (int i = 0; i < search.length(); ++i)
	{
		char c = search[i];
		int value = (int)((unsigned char)search[i]);
		int difference = value - base;

		if (!escape)
		{
			if (c == '\\' && i < search.length() - 1)
			{
				escape = true;
				continue;
			}
			else if (c == '*')
			{
				differences.push_back(WILDCARD);
				continue;
			}
		}

		if (base != -1)
		{
			differences.push_back(difference);
			++known_differences;
		}
		else
			base = value;
		escape = false;
	}

	// Check difference table for valid search string
	if (!known_differences)
	{
		std::cerr << "Invalid search string" << std::endl;
		return EXIT_FAILURE;
	}

	// Search
	char byte;
	int base_offset;
	int match_count;
	int found_count = 0;
	int file_offset = 0;
	while (file.get(byte))
	{
		// Find base
		base = (int)((unsigned char)byte);
		base_offset = file_offset;
		match_count = 0;

		// Compare following bytes against difference table
		while (file.get(byte))
		{
			int value = (int)((unsigned char)byte);
			int difference = value - base;

			if (difference != differences[match_count] && differences[match_count] != WILDCARD)
				break;

			if (++match_count == differences.size())
			{
				// Shift base offset by number of prefixed wildcards
				base_offset -= prefixed_wildcards;
				if (base_offset < 0)
					break;

				// Pattern found
				std::printf("0x%08x\n", base_offset);
				++found_count;
				break;
			}
		}

		file.clear();
		file.seekg(++file_offset, file.beg);
	}

	file.close();
	
	// Check if any patterns were found
	if (!found_count)
	{
		std::cerr << "No results found" << std::endl;
		return EXIT_FAILURE;
	}
	else
	{
		std::cout << "Found " << found_count;
		
		if (found_count == 1 )
		{
			std::cout << " match" << std::endl;
		}
		else
		{
			std::cout << " matches" << std::endl;
		}
	}

	return EXIT_SUCCESS;
}
