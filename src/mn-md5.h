/*
 * mn-md5.h - RSA Data Security, Inc., MD5 message-digest algorithm
 *
 * Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
 * rights reserved.
 *
 * License to copy and use this software is granted provided that it
 * is identified as the "RSA Data Security, Inc. MD5 Message-Digest
 * Algorithm" in all material mentioning or referencing this software
 * or this function.
 *
 * License is also granted to make and use derivative works provided
 * that such works are identified as "derived from the RSA Data
 * Security, Inc. MD5 Message-Digest Algorithm" in all material
 * mentioning or referencing the derived work.
 *
 * RSA Data Security, Inc. makes no representations concerning either
 * the merchantability of this software or the suitability of this
 * software for any particular purpose. It is provided "as is"
 * without express or implied warranty of any kind.
 *
 * These notices must be retained in any copies of any part of this
 * documentation and/or software.
 *
 * This code is the same as the code published by RSA Inc.  It has been
 * edited for clarity and style only.
 */

#ifndef _MN_MD5_H
#define _MN_MD5_H

#include <stdint.h>

typedef struct
{
  uint32_t state[4];		/* state (ABCD) */
  uint32_t count[2];		/* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];	/* input buffer */
} MNMD5Context;

void mn_md5_init (MNMD5Context *context);
void mn_md5_update (MNMD5Context *context,
		    const unsigned char *buf,
		    unsigned int len);
void mn_md5_pad (MNMD5Context *context);
void mn_md5_final (unsigned char digest[16], MNMD5Context *context);
char *mn_md5_end (MNMD5Context *context, char *buf);

#endif /* _MN_MD5_H */
