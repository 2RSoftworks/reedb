/* reedb - crypto/engine.c
 *
 *
 * (c) 2015 					Lonely Robot.
 * Authors:						Katharina 'spacekookie' Sabel
 *
 * This program and the accompanying materials
 * are made available under the terms of the GNU Lesser General Public License
 * (LGPL) version 3 which accompanies this distribution, and is available at
 * http://www.gnu.org/licenses/lgpl-3.html
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * -------------------------------------------
 *
 */

#include <gcrypt.h>
#include "engine.h"
#include "reedb/defs.h"

ree_err_t init_rdb_crypto(enum cryflgs_t flags[])
{
	/* Version check should be the very first call because it
	 makes sure that important subsystems are initialized. */
	if (!gcry_check_version(GCRYPT_VERSION))
	{
		fputs("Reedb crypto engine error: libgcrypt version mismatch!\n", stderr);
		exit(2);
	}

	/* We don't want to see any warnings, e.g. because we have not yet
		 parsed program options which might be used to suppress such
		 warnings. */
	gcry_control(GCRYCTL_SUSPEND_SECMEM_WARN);

	/* Allocate a pool of 32k secure memory.  This make the secure memory
		 available and also drops privileges where needed.  */
	gcry_control(GCRYCTL_INIT_SECMEM, 32768, 0);

	/* It is now okay to let Libgcrypt complain when there was/is
		 a problem with the secure memory. */
	gcry_control(GCRYCTL_RESUME_SECMEM_WARN);

	/* Tell Libgcrypt that initialization has completed. */
	gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);

	if (!gcry_control(GCRYCTL_INITIALIZATION_FINISHED_P))
	{
		fputs("Reedb was unable to initialise the crypto engine...\n", stderr);
		abort();
	}

	return SUCCESS;
}
