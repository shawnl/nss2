/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/* Copyright(c) 2019, Shawn Landden. */

/* Wrapper functions for PPC optimized implementation of AES-GCM */

#ifdef USE_PPC_GCM

#if !defined(__powerpc__) && !defined(__powerpc64__)
#error This file is only for powerpc.
#endif

#ifdef FREEBL_NO_DEPEND
#include "stubs.h"
#endif

#include "blapii.h"
#include "blapit.h"
#include "gcm.h"
#include "ctr.h"
#include "secerr.h"
#include "prtypes.h"
#include "pkcs11t.h"

#include <limits.h>
#include <stdint.h>

#include "rijndael.h"

// These are not uint128_t so this can build on ppc32
void gcm_init_p8(uint8_t Htable[16 * 16], const uint64_t H[2]);
void gcm_gmult_p8(uint64_t Xi[2], const uint8_t Htable[16 * 16]);
void gcm_ghash_p8(uint64_t Xi[2], const uint8_t Htable[16 * 16], const uint8_t *inp,
                  size_t len);

SECStatus gcm_HashMult_hw(gcmHashContext *ghash, const unsigned char *buf,
                          unsigned int count) {
    gcm_ghash_p8(ghash->shash, ghash->table, buf, count * 16);
    return SECSuccess;
}

SECStatus gcm_HashInit_hw(gcmHashContext *ghash) {
    memset(ghash->table, 0, sizeof(ghash->table));
    gcm_init_p8(ghash->table, (const uint64_t *) ghash->H);
    ghash->ghash_mul = gcm_HashMult_hw;
    ghash->hw = PR_TRUE;
    ghash->bytes = 0;
    memset(ghash->shash, 0, 16);
    return SECSuccess;
}

SECStatus gcm_HashWrite_hw(gcmHashContext *ghash, unsigned char *outbuf) {
    memcpy(outbuf, ghash->shash, 16);
    return SECSuccess;
}

SECStatus gcm_HashZeroX_hw(gcmHashContext *ghash) {
    ghash->shash[0] = 0;
    ghash->shash[1] = 0;
    return SECSuccess;
}

#endif /* USE_PPC_GSM */
