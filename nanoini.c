/*
 * Copyright (C) S.A.S. IKALOGIC - All Rights Reserved
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
 * nanoini.c
 *
 *  Created on: 4 janv. 2020
 *      Author: I.KAMAL
 */

#ifndef NANOINI_C_
#define NANOINI_C_

#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"
#include "nanoini.h"

void nanoini_reset_result(nanoini_parser_t *p);

void nanoini_init(nanoini_parser_t *p)
{
    nanoini_reset_result(p);
}

void nanoini_feed_bloc(nanoini_parser_t *p,char* data, size_t len)
{
    p->s = NANOINI_PARSER_GET_KEY;
    p->data_bloc_ptr = data;
    p->data_bloc_end = data+len;
}

nanoini_result_t nanoini_parse_bloc(nanoini_parser_t *p)
{
    bool line_done = false;
    p->result.valid = false;
    char c;
    while((p->data_bloc_ptr < p->data_bloc_end) && (line_done == false))
    {
        c = *(p->data_bloc_ptr);
        switch (p->s)
        {
        case NANOINI_PARSER_GET_KEY:
            if  (c == NANOINI_COMMENT)
            {
                p->s = NANOINI_PARSER_WAIT_NEW_LINE;
            }
            else if ((c == '\r')||(c == '\n'))
            {
                nanoini_reset_result(p);
                //Skip those characters, simply.
            }
            else if ((c == NANOINI_DELIMITER))
            {
                p->s = NANOINI_PARSER_GET_VAL;
                p->result.idx = 0;
            }
            else if (p->result.idx == NANOINI_MAX_KEY_LEN)
            {
                p->result.key_val_overflow = true;
            }
            else if (c != ' ')    //valid character for key (ignore spaces)
            {
                p->result.key[p->result.idx++] = c;
            }
            break;
        case NANOINI_PARSER_GET_VAL:
            if  ((c == NANOINI_COMMENT)||(c == '\r')||(c == '\n'))
            {
                line_done = true;
                p->result.valid = true;
                p->s = NANOINI_PARSER_WAIT_NEW_LINE;
            }
            else if (p->result.idx == NANOINI_MAX_VAL_LEN)
            {
                p->result.key_val_overflow = true;
            }
            else if (c != ' ')    //valid character for value (ignore spaces)
            {
                p->result.val[p->result.idx++] = c;
            }
            break;
        case NANOINI_PARSER_WAIT_NEW_LINE:
            if  ((c == '\r')||(c == '\n'))
            {
                nanoini_reset_result(p);
                p->s = NANOINI_PARSER_GET_KEY;
            }
            break;    
        default:
            break;
        }
        p->data_bloc_ptr++;
    }

    if (p->data_bloc_ptr == p->data_bloc_end)
    {
        p->result.more = false;
    }
    else
    {
        p->result.more = true;
    }

    return p->result;
}

void nanoini_reset_result(nanoini_parser_t *p)
{
    p->result.idx = 0;
    p->result.key_val_overflow=false;
    memset(p->result.key,0,NANOINI_MAX_KEY_LEN);
    memset(p->result.val,0,NANOINI_MAX_VAL_LEN);
}

#endif /* NANOINI_C_ */


