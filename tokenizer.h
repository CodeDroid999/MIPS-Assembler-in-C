#ifndef __PARSE_UTILS_H_
#define __PARSE_UTILS_H_

/* 
 *Author:
 *Author: 
 *This file contains a thread-safe string tokenization function that can be
 * used for general purpose parsing. 
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

static inline char *parse_token(char *in_str, char *delim, char **out_str, char *delim_char)
{
  int len;
  char *ptr, *tptr, *token;

  /* Bypass leading whitespace delimiters */
  len = strspn(in_str, delim);
  ptr = (in_str + len);
  
  /* Get end of token */
  tptr = strpbrk(ptr, delim);
  if (tptr == NULL) return(NULL);
  len = tptr - ptr;

  if (delim_char != NULL) *delim_char = *tptr;
  
  /* Create output string */
  *out_str = tptr + 1; /* go past the delimiter */
  
  /* Create token */
  token = (char *) malloc(len + 1);
  if (token == NULL) return(NULL);
  memcpy(token, ptr, len);
  token[len] = (char) 0;
  return(token);
}


#endif 
