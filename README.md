# Bytefinder

This is a small utility program which a file for a pattern of bytes. It was created for searching for non-ASCII strings in ROMs.

The search works by forming a numerical difference pattern based on the integer values of the string characters relative to the first character in the string. Byte sequences in the file are then compared to this pattern. The address of each matching byte sequence is returned.

## Building

	g++ bytefinder.cpp -o bytefinder

## Usage

	bytefinder <file> <string>

The wildcard character `*` is supported and may be escaped as `\*`. Search strings must contain at least two non-wildcard characters.

## License

This program is licensed under the GNU General Public License, version 3. See [COPYING](COPYING) for details.
