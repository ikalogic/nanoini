/*
 * Copyright (C) S.A.S. IKALOGIC - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Ibrahim KAMAL <i.kamal@ikalogic.com>, May 2019
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/*
 * nanoini.h
 *
 *  Created on: 4 janv. 2020
 *      Author: I.KAMAL
 *
 *  This lib is meant to be *extremly* minimalistic, lightweight and efficient tool to
 *  parse INI formated string on the fly, with absolute minimum memory footprint:
 *  It only needs just enough memory to store a few pointers the the characters 
 *  strings for a single key-value pair.
 * 
 *  Most importantnly: the memory usage is not related to maximum line size, unlike
 *  most (all?) libraries available.
 *  Comments are ignored without any impact on memory usage, regardless of its size
 *  or quantity.
 *
 *  Features:
 *  - Parses chunks of an ini file, whatever the chunk size, without impact on memory usage.
 *  - Only needs to allocate enough memory for the key and the value strings
 *  - Gracefully handles a key and/or value that does not fit in allocated space
 *  - Doesn't need to allocate memory for a full line
 *  - Works with lines ending with '\n', '\r' or any comnination of both.
 *  - Only 3 functions interface
 * 
 * known limitations:
 * - Key without value is not allowed
 * - Delimiter and Comments character cannot be used for values,
 *   even if the values are re "encapsulated".
 * - Spaces in keys or values are simply ignored
 * - Sections are not supported (simply ignored)
 *
 */

#ifndef NANOINI_H_
#define NANOINI_H_

#include "stdbool.h"
#include <crtdefs.h>

#define NANOINI_MAX_KEY_LEN 50
#define NANOINI_MAX_VAL_LEN 50
#define NANOINI_COMMENT     ';'
#define NANOINI_DELIMITER   '='

typedef struct
{
    char key[NANOINI_MAX_KEY_LEN];
    char val[NANOINI_MAX_VAL_LEN];
    int idx; //Index for both key and val
    bool key_val_overflow;
}nanoini_result_t;

typedef enum
{
    NANOINI_PARSER_GET_KEY,
    NANOINI_PARSER_GET_VAL,
    NANOINI_PARSER_WAIT_NEW_LINE,
}nanoini_state_t;

typedef struct
{
    char* data_bloc_ptr;
    char* data_bloc_end;
    nanoini_result_t result;
    nanoini_state_t s;
}nanoini_parser_t;

typedef void (*ini_handler)(void* user,const char* key, const char* value, bool ovf);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief nanoini_init need to be called before using the library
 * @param p
 */
void nanoini_init(nanoini_parser_t *p, ini_handler handler, void *user_data);
void nanoini_parse_bloc(nanoini_parser_t *p,char* data, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* NANOINI_H_ */
