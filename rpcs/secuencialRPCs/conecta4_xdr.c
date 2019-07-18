/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "conecta4.h"

bool_t
xdr_tBoard (XDR *xdrs, tBoard objp)
{
	register int32_t *buf;

	 if (!xdr_vector (xdrs, (char *)objp, BOARD_CELLS,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_tString (XDR *xdrs, tString objp)
{
	register int32_t *buf;

	 if (!xdr_vector (xdrs, (char *)objp, STRING_LENGTH,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_tPlayer (XDR *xdrs, tPlayer *objp)
{
	register int32_t *buf;

	 if (!xdr_enum (xdrs, (enum_t *) objp))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_tMove (XDR *xdrs, tMove *objp)
{
	register int32_t *buf;

	 if (!xdr_enum (xdrs, (enum_t *) objp))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_tMessage (XDR *xdrs, tMessage *objp)
{
	register int32_t *buf;

	 if (!xdr_tString (xdrs, objp->msg))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_tColumn (XDR *xdrs, tColumn *objp)
{
	register int32_t *buf;

	 if (!xdr_u_int (xdrs, &objp->column))
		 return FALSE;
	 if (!xdr_tString (xdrs, objp->player))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_tBlock (XDR *xdrs, tBlock *objp)
{
	register int32_t *buf;

	 if (!xdr_u_int (xdrs, &objp->code))
		 return FALSE;
	 if (!xdr_tString (xdrs, objp->msg))
		 return FALSE;
	 if (!xdr_tBoard (xdrs, objp->board))
		 return FALSE;
	return TRUE;
}
