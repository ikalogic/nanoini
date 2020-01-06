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
