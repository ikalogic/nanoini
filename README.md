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
- Only 3 functions API (including the callback function).

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
 * There *must* be a new line after the last key/value pair.
 
 ## API Usage

Implement this callback in your code:

```c
static void nanoini_handler(void* user,const char* key, const char* value, bool ovf);
```

This will be called for every valid key/value par that is detected in the parsed string. `ovf` will be true whenever a key or a value cannot fit in the maximum length defined by:

```c
#define NANOINI_MAX_KEY_LEN 50 
#define NANOINI_MAX_VAL_LEN 50 
```
`user` is simply a pointer to your own data container.

Then, simply Call `nanoini_init(nanoini_parser_t *p, ini_handler handler, void *user_data)` before parsing a string.

And Call `nanoini_parse_bloc(nanoini_parser_t *p,char* data, size_t len)` for each bloc of string to be parsed.
 
 ## Usage examples
 
 In the example below, we read the content from a file (although that's not the purpose of this library), and we cut it in 50 bytes blocs, to mimic how FreeRTOS streambuffers may present data (or any other data streaming system).

 ```c
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../../nanoini.h"

#define BLOC_LEN    50

typedef struct
{
    uint8_t     key_a;
    bool        key_b;
    int32_t    key_c;
}my_cfg_t;

static char data[BLOC_LEN];

static void nanoini_handler(void* user,const char* key, const char* value, bool ovf)
{
    my_cfg_t* cfg = user;
    printf("%s=%s%s\n",key,value,ovf?"Warning, overflow occured!":"");
    if (strcmp(key,"KEY_A") == 0)
    {
        cfg->key_a = (uint8_t)strtol(value,NULL,0);
    }
    else if (strcmp(key,"KEY_B") == 0)
    {
        cfg->key_b = strtol(value,NULL,0);
    }
    else if (strcmp(key,"KEY_C") == 0)
    {
        cfg->key_c = (int32_t)strtol(value,NULL,0);
    }
}

int main()
{
    nanoini_parser_t nano_prs;
    FILE * pFile;
    my_cfg_t cfg;
    int c; // note: int, not char, required to handle EOF
    int i = 0;

    pFile = fopen("test.ini","r");
    if(!pFile) {
        perror("File opening failed");
        return -1;
    }

    nanoini_init(&nano_prs,nanoini_handler,&cfg);

    while ((c = fgetc(pFile)) != EOF)
    {
       data[i++] = (char)c;
       if (i >= BLOC_LEN)
       {
           nanoini_parse_bloc(&nano_prs,data,(size_t)i);
           i = 0;
       }
    }
    //parse last block (that will most probably be smaller than 50 bytes
    nanoini_parse_bloc(&nano_prs,data,(size_t)i);

    printf("cfg populated with: %d,%d,%d\n",cfg.key_a,cfg.key_b,cfg.key_c);
    return 0;
}

 ```
