#ifndef RPMPERL_H
#define RPMPERL_H

/** \ingroup rpmio
 * \file rpmio/rpmperl.h
 */

#include <rpmiotypes.h>
#include <rpmio.h>

typedef /*@abstract@*/ struct rpmperl_s * rpmperl;

/*@unchecked@*/
extern int _rpmperl_debug;

#if defined(_RPMPERL_INTERNAL)
#if defined(WITH_PERLEMBED)
#include <EXTERN.h>
#include <perl.h>
#endif

struct rpmperl_s {
    struct rpmioItem_s _item;	/*!< usage mutex and pool identifier. */
    const char * fn;
    int flags;
    void * I;
};
#endif /* _RPMPERL_INTERNAL */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Unreference a perl interpreter instance.
 * @param perl		perl interpreter
 * @return		NULL on last dereference
 */
/*@unused@*/ /*@null@*/
rpmperl rpmperlUnlink (/*@killref@*/ /*@only@*/ /*@null@*/ rpmperl perl)
	/*@modifies perl @*/;
#define	rpmperlUnlink(_ds)	\
    ((rpmperl)rpmioUnlinkPoolItem((rpmioItem)(_perl), __FUNCTION__, __FILE__, __LINE__))

/**
 * Reference a perl interpreter instance.
 * @param perl		perl interpreter
 * @return		new perl interpreter reference
 */
/*@unused@*/ /*@newref@*/ /*@null@*/
rpmperl rpmperlLink (/*@null@*/ rpmperl perl)
	/*@modifies perl @*/;
#define	rpmperlLink(_perl)	\
    ((rpmperl)rpmioLinkPoolItem((rpmioItem)(_perl), __FUNCTION__, __FILE__, __LINE__))

/**
 * Destroy a perl interpreter.
 * @param perl		perl interpreter
 * @return		NULL on last dereference
 */
/*@null@*/
rpmperl rpmperlFree(/*@killref@*/ /*@null@*/rpmperl perl)
	/*@globals fileSystem @*/
	/*@modifies perl, fileSystem @*/;
#define	rpmperlFree(_perl)	\
    ((rpmperl)rpmioFreePoolItem((rpmioItem)(_perl), __FUNCTION__, __FILE__, __LINE__))

/**
 * Create and load a perl interpreter.
 * @param fn		(unimplemented) perl file to load (or NULL)
 * @param flags		(unimplemented) perl interpreter flags
 * @return		new perl interpreter
 */
/*@newref@*/ /*@null@*/
rpmperl rpmperlNew(/*@null@*/ const char * fn, int flags)
	/*@globals fileSystem, internalState @*/
	/*@modifies fileSystem, internalState @*/;

/**
 * Execute perl string.
 * @param perl		perl interpreter
 * @param str		perl string to execute (or NULL)
 * @param *resultp	perl exec result
 * @return		RPMRC_OK on success
 */
rpmRC rpmperlRun(rpmperl perl, /*@null@*/ const char * str,
		/*@null@*/ const char ** resultp)
	/*@globals fileSystem, internalState @*/
	/*@modifies perl, *resultp, fileSystem, internalState @*/;

#ifdef __cplusplus
}
#endif

#endif /* RPMPERL_H */