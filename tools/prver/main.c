/* SCE DECOMPILED
 $PSLibId$
 */
/*
 *
 *      Copyright (C) 1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define VERSION "1.0"

static int opt_v = 0;

static void version(void)
{
    fprintf(stderr, "%s\n", "prver version "VERSION);
    fprintf(stderr, "%s\n",
        "Copyright (C) 1999 by Sony Computer Entertainment Inc. "
        "All rights Reserved.");
}

static void usage(void)
{
    fprintf(stderr, "usage: prver [-v] file1 [file2...]\n");
}

static void pkver(const char *fname)
{
    int i;
    long pos;
    FILE *fp;
    int ch, sig[4] = { 'P','s','I','I' };

    fp = fopen(fname, "rb");
    if (fp == NULL) {
        perror(fname);
        return;
    }

    while (1) {
        ch = fgetc(fp);
        if (ch == EOF) break;

        /* found signature? */
        if (ch == sig[0]) {
            pos = ftell(fp);

            /* verify signature */
            for (i=1; i < 4; i++) {
                ch = fgetc(fp);
                if (ch == EOF || ch != sig[i]) {
                    goto seek_next;
                }
            }
            /* print library name */
            for (i=0; i < 8; i++) {
                ch = fgetc(fp);
                if (ch == EOF || !isprint(ch)) {
                    goto seek_next;
                }
                fputc(ch, stdout);
            }
            printf(" :\t");
            /* print library version */
            for (i=0; i < 4; i++) {
                ch = fgetc(fp);
                if (ch == EOF) {
                    goto seek_next;
                }
                if (isprint(ch)) {
                    fputc(ch, stdout);
                } else {
                    printf("%x", ch);
                }
                if (i < 3) {
                    fputc('.', stdout);
                }
            }
            printf("\n");
        seek_next:
            /* continue scan */
            fseek(fp, pos, SEEK_SET);
        }
    }
    /* scan complete */
    fclose(fp);
}

int main(int argc, char *argv[])
{
    int i;

    /* check arguments */
    while (1) {
        argv++;
        if (--argc <= 0) {
            break;
        }
        if (strcmp(argv[0], "-v") == 0) {
            opt_v = 1;
        }
        else break;
    }
    /* print version info */
    if (opt_v) {
        version();
    }
    /* scan input files */
    if (argc > 0) {
        for (i=0; i < argc; i++) {
            pkver(argv[i]);
        }
        return 0;
    }
    /* no arguments */
    if (!opt_v) {
        version();
    }
    usage();
    return 1;
}
