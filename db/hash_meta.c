/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1999
 *	Sleepycat Software.  All rights reserved.
 */

#include "db_config.h"

#ifndef lint
static const char sccsid[] = "@(#)hash_meta.c	11.3 (Sleepycat) 9/14/99";
#endif /* not lint */

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>
#include <errno.h>
#endif

#include "db_int.h"
#include "db_page.h"
#include "hash.h"
#include "db_shash.h"
#include "lock.h"
#include "txn.h"

/*
 * Acquire the meta-data page.
 *
 * PUBLIC: int CDB___ham_get_meta __P((DBC *));
 */
int
CDB___ham_get_meta(dbc)
	DBC *dbc;
{
	HASH_CURSOR *hcp;
	HASH *hashp;
	DB *dbp;
	int ret;

	hcp = dbc->internal;
	dbp = dbc->dbp;
	hashp = dbp->h_internal;

	if (dbp->dbenv != NULL && F_ISSET(dbp->dbenv, DB_ENV_LOCKING) &&
	    !F_ISSET(dbc, DBC_RECOVER)) {
		dbc->lock.pgno = hashp->meta_pgno;
		if ((ret = CDB_lock_get(dbp->dbenv, dbc->locker,
		    DB_NONBLOCK(dbc) ? DB_LOCK_NOWAIT : 0,
		    &dbc->lock_dbt, DB_LOCK_READ, &hcp->hlock)) != 0)
			return (ret);
	}

	if ((ret = CDB___ham_get_page(dbc->dbp,
	    hashp->meta_pgno, (PAGE **)&(hcp->hdr))) != 0 &&
	    hcp->hlock.off != LOCK_INVALID) {
		(void)CDB_lock_put(dbc->dbp->dbenv, &hcp->hlock);
		hcp->hlock.off = LOCK_INVALID;
	}

	return (ret);
}

/*
 * Release the meta-data page.
 *
 * PUBLIC: int CDB___ham_release_meta __P((DBC *));
 */
int
CDB___ham_release_meta(dbc)
	DBC *dbc;
{
	HASH_CURSOR *hcp;

	hcp = dbc->internal;

	if (hcp->hdr)
		(void)CDB___ham_put_page(dbc->dbp, (PAGE *)hcp->hdr,
		    F_ISSET(hcp, H_DIRTY) ? 1 : 0);
	hcp->hdr = NULL;
	if (!F_ISSET(dbc, DBC_RECOVER) &&
	    dbc->txn == NULL && hcp->hlock.off != LOCK_INVALID)
		(void)CDB_lock_put(dbc->dbp->dbenv, &hcp->hlock);
	hcp->hlock.off = LOCK_INVALID;
	F_CLR(hcp, H_DIRTY);

	return (0);
}

/*
 * Mark the meta-data page dirty.
 *
 * PUBLIC: int CDB___ham_dirty_meta __P((DBC *));
 */
int
CDB___ham_dirty_meta(dbc)
	DBC *dbc;
{
	DB *dbp;
	DB_LOCK _tmp;
	HASH *hashp;
	HASH_CURSOR *hcp;
	int ret;

	dbp = dbc->dbp;
	hashp = dbp->h_internal;
	hcp = dbc->internal;

	ret = 0;
	if (F_ISSET(dbp->dbenv, DB_ENV_LOCKING) && !F_ISSET(dbc, DBC_RECOVER)) {
		dbc->lock.pgno = hashp->meta_pgno;
		if ((ret = CDB_lock_get(dbp->dbenv, dbc->locker,
		    DB_NONBLOCK(dbc) ? DB_LOCK_NOWAIT : 0,
		    &dbc->lock_dbt, DB_LOCK_WRITE, &_tmp)) == 0) {
			ret = CDB_lock_put(dbp->dbenv, &hcp->hlock);
			hcp->hlock = _tmp;
		}
	}

	if (ret == 0)
		F_SET(hcp, H_DIRTY);
	return (ret);
}
