# nanoini
This lib is meant to be an *extremly* minimalistic, lightweight and efficient tool to parse INI formated string on the fly, with absolute minimum memory footprint: It only needs just enough memory to store a few pointers the the characters strings for a single key-value pair.
Most importantnly: the memory usage is not related to maximum line size, unlike most (all?) libraries available. 

nanoini does not relies on actual files, nor was it meant to work with files; it's solely adapted to MCUs with limited memory. nanoini's ability to parse a stream of INI formated string *on-the-fly without buffering a full line* prior to parsing it, is what makes it different.

## Features:
- Parses chunks of an ini file, whatever the chunk size, without impact on memory usage.
- Only needs to allocate enough memory for the key and the value strings
- Gracefully handles a key and/or value that does not fit in allocated space
- Doesn't need to allocate memory for a full line
- Works with lines ending with '\n', '\r' or any comnination of both.
- INI Comments are ignored without any impact on memory usage, regardless of its size
- Only 3 functions API.

 ## known (and wanted) limitations:
 nanoini is limited by design. It may evolve a bit in the future to implement more standard features like sections.
 ### Key without value is not allowed
 it's not allowed to have such a line:
 `key`
 this 'key' will simply be silently ignored.
 
### Delimiter and Comments characters cannot be used for values
Even if the values are re "encapsulated", they cannot contain an `=`, for example this line is not valid and may break the parsing:
`my_key = "my=value"`
In other words, quotation marks are not understood by the parser and are treated like any other character.
 
 ### Other limitations
 * Spaces in keys or values are simply ignored
 * Sections are not supported (simply ignored)
 
 ## Usage examples
 
 TODO
