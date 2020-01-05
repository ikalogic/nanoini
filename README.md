# nanoini
This plain C library is meant to be an *extremly* minimalistic, lightweight and efficient tool to parse INI formated string on the fly, with absolute minimum memory footprint: It only needs just enough memory to store a few pointers the the characters strings for a single key-value pair.
Most importantnly: the memory usage is not related to maximum line size, unlike most (all?) libraries available. 

nanoini does not relies on actual files, nor was it meant to work with files; it's solely adapted to MCUs with limited memory. nanoini's ability to parse a stream of INI formated string *on-the-fly without buffering a full line* prior to parsing it, is what makes it different.

## Features:
- Parses chunks of an ini formatted string, whatever the chunk size, without impact on memory usage.
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
 
 ## API
`void nanoini_init(nanoini_parser_t *p);`
The init function need to be called before using the other functions. It will simply reset some working variables in the nanoini_parser_t structure. `p` must be allocated.

`void nanoini_feed_bloc(nanoini_parser_t *p, char* data, size_t len);`
As the name implies, use this function to feed a bloc of string to the parser. This is perfectly adapted to FreeRTOS StreamBuffers.

`nanoini_result_t nanoini_parse_bloc(nanoini_parser_t *p);`
This function actually does the job of parsing the bloc that was fed using the function above. it a returns this structure:
```c
typedef struct
{
    char key[NANOINI_MAX_KEY_LEN];
    char val[NANOINI_MAX_VAL_LEN];
    int idx; //Index for both key and val
    bool key_val_overflow;
    bool valid;
    bool more;  //function still needs to be called again.
}nanoini_result_t;
```
The function `nanoini_parse_bloc` needs to be called repeatedly as long as the returned structure has `more == true`. Also, if the returned structure has the member `valid == true`, it means a new valid key/value pair are available in the structure members `key[]` and  `val[]`.

If `key_val_overflow` is true, it means that at some point, either the key or the value or both were too large to fit in the corresponding buffers.

In that case, you may either increase those defines:
```c
#define NANOINI_MAX_KEY_LEN 50 
#define NANOINI_MAX_VAL_LEN 50 
```
Or you may change the structure of your INI data.


 
 ## Usage examples
 
 ```c
 #include "nanoini.h"
 
 int main () 
 {
 
  nanoini_result_t nret;
  nanoini_parser_t np;
  int i,len;

  nanoini_init(&np);

  for (i = 0; i < sizeof(test_data); i+=50)
  {
    len = sizeof(test_data)-i;
    if (len > 50)
    {
        len = 50;
    }
    nanoini_feed_bloc(&np,&test_data[i],len);
    nret.more = true;
    while(nret.more)
    {
        nret = nanoini_parse_bloc(&np);
        if (nret.valid)
        {
            printf("%s=%s\r\n", nret.key, nret.val);
        }
    }
  }
  
  return 0;
 }
 ```
