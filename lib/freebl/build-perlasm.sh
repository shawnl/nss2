#!/bin/sh
#/* This Source Code Form is subject to the terms of the Mozilla Public
# * License, v. 2.0. If a copy of the MPL was not distributed with this
# * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
perl ghashp8-ppc.pl linux64le ppc8-ghash.S
perl ghashp8-ppc.pl linux64 ppc8-ghash-be.S
perl ghashp8-ppc.pl linux32 ppc8-ghash-32.S
perl aesp8-ppc.pl linux64le ppc8-aes.S
perl aesp8-ppc.pl linux64 ppc8-aes-be.S
perl aesp8-ppc.pl linux32 ppc8-aes-32.S

