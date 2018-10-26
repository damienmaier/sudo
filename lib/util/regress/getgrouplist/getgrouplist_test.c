/*
 * Copyright (c) 2018 Todd C. Miller <Todd.Miller@sudo.ws>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <config.h>

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_STRING_H
# include <string.h>
#endif /* HAVE_STRING_H */
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif /* HAVE_STRINGS_H */
#ifdef HAVE_STDBOOL_H
# include <stdbool.h>
#else
# include "compat/stdbool.h"
#endif
#include <pwd.h>
#include <grp.h>

#include "sudo_compat.h"
#include "sudo_fatal.h"
#include "sudo_util.h"

__dso_public int main(int argc, char *argv[]);

/*
 * Test that sudo_getgrouplist2() works as expected.
 */

int
main(int argc, char *argv[])
{
    GETGROUPS_T *groups = NULL;
    struct passwd *pw;
    struct group *grp;
    char *username;
    int i, j, errors = 0, ntests = 0;
    int ngroups;
    initprogname(argc > 0 ? argv[0] : "getgrouplist_test");

    if ((pw = getpwuid(0)) == NULL)
	sudo_fatal_nodebug("getpwuid(0)");
    if ((username = strdup(pw->pw_name)) == NULL)
	sudo_fatal_nodebug(NULL);

    if (sudo_getgrouplist2(pw->pw_name, pw->pw_gid, &groups, &ngroups) == -1)
	sudo_fatal_nodebug("sudo_getgroulist2");

    for (i = 0; i < ngroups; i++) {
	ntests++;

	/* Verify group ID exists. */
	if ((grp = getgrgid(groups[i])) == NULL) {
	    sudo_warnx_nodebug("unable to look up group ID %u",
		(unsigned int)groups[i]);
	    errors++;
	    continue;
	}

	/* Verify group membership. */
	for (j = 0; grp->gr_mem[j] != NULL; j++) {
	    if (strcmp(username, grp->gr_mem[j]) == 0) {
		/* match */
		break;
	    }
	}
	if (grp->gr_mem[j] == NULL) {
	    sudo_warnx_nodebug("unable to find %s in group %s",
		username, grp->gr_name);
	    errors++;
	    continue;
	}
    }
    if (errors != 0) {
	printf("%s: %d tests run, %d errors, %d%% success rate\n",
	    getprogname(), ntests, errors, (ntests - errors) * 100 / ntests);
    }
    exit(errors);
}