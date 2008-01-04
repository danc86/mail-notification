/*
 * MD5 message digest implementation, taken from glibc and edited for
 * style.
 *
 * The GNU C Library,
 * Copyright (C) 1995,1996,1997,1999,2000,2001,2005
 * Free Software Foundation, Inc.
 *
 * Mail Notification
 * Copyright (C) 2003-2008 Jean-Yves Lefort <jylefort@brutele.be>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _MN_MD5_H
#define _MN_MD5_H

#include <stdint.h>

typedef struct
{
  uint32_t A;
  uint32_t B;
  uint32_t C;
  uint32_t D;

  uint32_t total[2];
  uint32_t buflen;
  char buffer[128]
#ifdef __GNUC__
  __attribute__ ((__aligned__ (__alignof__ (uint32_t))))
#endif
    ;
} MNMD5Context;

/* Initialize structure containing state of computation.
   (RFC 1321, 3.3: Step 3)  */
void mn_md5_init_ctx (MNMD5Context *ctx);

/* Starting with the result of former calls of this function (or the
   initialization function update the context for the next LEN bytes
   starting at BUFFER.
   It is necessary that LEN is a multiple of 64!!! */
void mn_md5_process_block (MNMD5Context *ctx, const void *buffer, size_t len);

/* Starting with the result of former calls of this function (or the
   initialization function update the context for the next LEN bytes
   starting at BUFFER.
   It is NOT required that LEN is a multiple of 64.  */
void mn_md5_process_bytes (MNMD5Context *ctx, const void *buffer, size_t len);

/* Process the remaining bytes in the buffer and put result from CTX
   in first 16 bytes following RESBUF.  The result is always in little
   endian byte order, so that a byte-wise output yields to the wanted
   ASCII representation of the message digest.

   IMPORTANT: On some systems it is required that RESBUF is correctly
   aligned for a 32 bits value.  */
void *mn_md5_finish_ctx (MNMD5Context *ctx, unsigned char resbuf[16]);

/*
 * Stores the nul-terminated hexadecimal representation of @resbuf
 * (which must be the result buffer filled in by mn_md5_finish_ctx())
 * into @hexbuf.
 */
void mn_md5_to_hex (const unsigned char resbuf[16], char hexbuf[33]);

#endif /* _MN_MD5_H */
