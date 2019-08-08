/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* curve 25519 https://www.rfc-editor.org/rfc/rfc7748.txt */

#ifdef FREEBL_NO_DEPEND
#include "../stubs.h"
#endif

#include "ecl-priv.h"
#include "ecp.h"
#include "mpi.h"
#include "mplogic.h"
#include "mpi-priv.h"
#include "secmpi.h"
#include "secitem.h"
#include "secport.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * point validation is not necessary in general. But this checks a point (px)
 * against some known bad values.
 */
SECStatus
ec_Curve25519_pt_validate(const SECItem *px)
{
    PRUint8 *p;
    int i;
    PRUint8 forbiddenValues[12][32] = {
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
        { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
        { 0xe0, 0xeb, 0x7a, 0x7c, 0x3b, 0x41, 0xb8, 0xae,
          0x16, 0x56, 0xe3, 0xfa, 0xf1, 0x9f, 0xc4, 0x6a,
          0xda, 0x09, 0x8d, 0xeb, 0x9c, 0x32, 0xb1, 0xfd,
          0x86, 0x62, 0x05, 0x16, 0x5f, 0x49, 0xb8, 0x00 },
        { 0x5f, 0x9c, 0x95, 0xbc, 0xa3, 0x50, 0x8c, 0x24,
          0xb1, 0xd0, 0xb1, 0x55, 0x9c, 0x83, 0xef, 0x5b,
          0x04, 0x44, 0x5c, 0xc4, 0x58, 0x1c, 0x8e, 0x86,
          0xd8, 0x22, 0x4e, 0xdd, 0xd0, 0x9f, 0x11, 0x57 },
        { 0xec, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f },
        { 0xed, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f },
        { 0xee, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f },
        { 0xcd, 0xeb, 0x7a, 0x7c, 0x3b, 0x41, 0xb8, 0xae,
          0x16, 0x56, 0xe3, 0xfa, 0xf1, 0x9f, 0xc4, 0x6a,
          0xda, 0x09, 0x8d, 0xeb, 0x9c, 0x32, 0xb1, 0xfd,
          0x86, 0x62, 0x05, 0x16, 0x5f, 0x49, 0xb8, 0x80 },
        { 0x4c, 0x9c, 0x95, 0xbc, 0xa3, 0x50, 0x8c, 0x24,
          0xb1, 0xd0, 0xb1, 0x55, 0x9c, 0x83, 0xef, 0x5b,
          0x04, 0x44, 0x5c, 0xc4, 0x58, 0x1c, 0x8e, 0x86,
          0xd8, 0x22, 0x4e, 0xdd, 0xd0, 0x9f, 0x11, 0xd7 },
        { 0xd9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
        { 0xda, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
        { 0xdb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
    };

    if (px->len == 32) {
        p = px->data;
    } else {
        return SECFailure;
    }

    for (i = 0; i < PR_ARRAY_SIZE(forbiddenValues); ++i) {
        if (NSS_SecureMemcmp(p, forbiddenValues[i], px->len) == 0) {
            return SECFailure;
        }
    }

    return SECSuccess;
}

/*
 * Scalar multiplication for Curve25519.
 * If P == NULL, the base point is used.
 * Returns X = k*P
 */
SECStatus
ec_Curve25519_pt_mul(SECItem *X, SECItem *k, SECItem *P)
{
    PRUint8 *px;
    PRUint8 basePoint[32] = { 9 };

    if (!P) {
        px = basePoint;
    } else {
        PORT_Assert(P->len == 32);
        if (P->len != 32) {
            return SECFailure;
        }
        px = P->data;
    }
    if (k->len != 32) {
        return SECFailure;
    }

    SECStatus rv = ec_Curve25519_mul(X->data, k->data, px);
    if (NSS_SecureMemcmpZero(X->data, X->len) == 0) {
        return SECFailure;
    }
    return rv;
}
