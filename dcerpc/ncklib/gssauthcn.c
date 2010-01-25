/*
 * Portions Copyright 2010 Apple Inc. All rights reserved.
 */
/*
 *
 * (c) Copyright 1991 OPEN SOFTWARE FOUNDATION, INC.
 * (c) Copyright 1991 HEWLETT-PACKARD COMPANY
 * (c) Copyright 1991 DIGITAL EQUIPMENT CORPORATION
 * Portions Copyright (c) 2010 Apple Inc. All rights reserved
 * To anyone who acknowledges that this file is provided "AS IS"
 * without any express or implied warranty:
 *                 permission to use, copy, modify, and distribute this
 * file for any purpose is hereby granted without fee, provided that
 * the above copyright notices and this notice appears in all source
 * code copies, and that none of the names of Open Software
 * Foundation, Inc., Hewlett-Packard Company, or Digital Equipment
 * Corporation be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.  Neither Open Software Foundation, Inc., Hewlett-
 * Packard Company, nor Digital Equipment Corporation makes any
 * representations about the suitability of this software for any
 * purpose.
 *
 */
/*
 */
/*
**  NAME
**
**      gssauthcn.c
**
**  FACILITY:
**
**      Remote Procedure Call (RPC)
**
**  ABSTRACT:
**
**  The gssauth CN authentication module.
**
**
*/

#include <config.h>
#include <gssauth.h>
#include <gssauthcn.h>

#define RPC__GSSAUTH_CN_AUTH_PADDING		16

INTERNAL boolean32 rpc__gssauth_cn_three_way_winnt (void);	/* NTLM only */
INTERNAL boolean32 rpc__gssauth_cn_three_way_gss (void);

INTERNAL boolean32 rpc__gssauth_cn_context_valid (
	rpc_cn_sec_context_p_t		/* in */     /*sec*/,
	unsigned32			/* out */    * /*st*/
    );

INTERNAL void rpc__gssauth_negotiate_cn_create_info (
       rpc_authn_level_t		/* in */     /*authn_level*/,
       rpc_auth_info_p_t		/* out */    * /*auth_info*/,
       unsigned32			/* out */    * /*st*/
    );

INTERNAL void rpc__gssauth_mskrb_cn_create_info (
       rpc_authn_level_t		/* in */     /*authn_level*/,
       rpc_auth_info_p_t		/* out */    * /*auth_info*/,
       unsigned32			/* out */    * /*st*/
    );

INTERNAL void rpc__gssauth_winnt_cn_create_info (
       rpc_authn_level_t		/* in */     /*authn_level*/,
       rpc_auth_info_p_t		/* out */    * /*auth_info*/,
       unsigned32			/* out */    * /*st*/
    );

INTERNAL void rpc__gssauth_netlogon_cn_create_info (
       rpc_authn_level_t		/* in */     /*authn_level*/,
       rpc_auth_info_p_t		/* out */    * /*auth_info*/,
       unsigned32			/* out */    * /*st*/
    );
INTERNAL boolean32 rpc__gssauth_cn_cred_changed (
	rpc_cn_sec_context_p_t		/* in */     /*sec*/,
	unsigned32			/* out */    * /*st*/
    );

INTERNAL void rpc__gssauth_cn_cred_refresh (
	rpc_auth_info_p_t		/* in */     /*auth_info*/,
	unsigned32			/* out */    * /*st*/
    );

INTERNAL void rpc__gssauth_cn_fmt_client_req (
	rpc_cn_assoc_sec_context_p_t	/* in */     /*assoc_sec*/,
	rpc_cn_sec_context_p_t		/* in */     /*sec*/,
	pointer_t			/* in/out */ /*auth_value*/,
	unsigned32			/* in/out */ * /*auth_value_len*/,
	pointer_t			/* in/out */ * /*last_auth_pos*/,
	unsigned32			/* out */    * /*auth_len_remain*/,
	unsigned32			/* in */     /*old_server*/,
	unsigned32			/* out */    * /*st*/
    );

INTERNAL void rpc__gssauth_cn_fmt_srvr_resp (
	unsigned32			/* in */     /*verify_st*/,
	rpc_cn_assoc_sec_context_p_t	/* in */     /*assoc_sec*/,
	rpc_cn_sec_context_p_t		/* in */     /*sec*/,
	pointer_t			/* in */     /*req_auth_value*/,
	unsigned32			/* in */     /*req_auth_value_len*/,
	pointer_t			/* in/out */ /*auth_value*/,
	unsigned32			/* in/out */ * /*auth_value_len*/
    );

INTERNAL void rpc__gssauth_cn_free_prot_info (
	rpc_auth_info_p_t		/* in */     /*info*/,
	rpc_cn_auth_info_p_t		/* in/out */ * /*cn_info*/
    );

INTERNAL void rpc__gssauth_cn_get_prot_info (
	rpc_auth_info_p_t		/* in */     /*info*/,
	rpc_cn_auth_info_p_t		/* out */    * /*cn_info*/,
	unsigned32			/* out */    * /*st*/
    );

INTERNAL void rpc__gssauth_cn_pre_call (
	rpc_cn_assoc_sec_context_p_t	/* in */     /*assoc_sec*/,
	rpc_cn_sec_context_p_t		/* in */     /*sec*/,
	pointer_t			/* in/out */ /*auth_value*/,
	unsigned32			/* in/out */ * /*auth_value_len*/,
	unsigned32			/* in/out */ * /*st*/
    );

INTERNAL void rpc__gssauth_cn_pre_send (
	rpc_cn_assoc_sec_context_p_t	/* in */     /*assoc_sec*/,
	rpc_cn_sec_context_p_t		/* in */     /*sec*/,
	rpc_socket_iovec_p_t		/* in/out */ /*iov*/,
	unsigned32			/* in */     /*iovlen*/,
	rpc_socket_iovec_p_t		/* out */    /*out_iov*/,
	unsigned32			/* out */    * /*st*/
    );

INTERNAL void rpc__gssauth_cn_recv_check (
	rpc_cn_assoc_sec_context_p_t	/* in */     /*assoc_sec*/,
	rpc_cn_sec_context_p_t		/* in */     /*sec*/,
	rpc_cn_common_hdr_p_t		/* in */     /*pdu*/,
	unsigned32			/* in */     /*pdu_len*/,
	unsigned32			/* in */     /*cred_len*/,
	rpc_cn_auth_tlr_p_t		/* in */     /*auth_tlr*/,
	boolean32			/* in */     /*unpack_ints*/,
	unsigned32			/* out */    * /*st*/
    );

INTERNAL void rpc__gssauth_cn_tlr_uuid_crc (
	pointer_t			/* in */     /*auth_value*/,
	unsigned32			/* in */     /*auth_value_len*/,
	unsigned32			/* out */    * /*uuid_crc*/
    );

INTERNAL void rpc__gssauth_cn_tlr_unpack (
	rpc_cn_packet_p_t		/* in */     /*pkt_p*/,
	unsigned32			/* in */     /*auth_value_len*/,
	unsigned8			/* out */    * /*packed_drep*/
    );

INTERNAL void rpc__gssauth_cn_vfy_client_req (
	rpc_cn_assoc_sec_context_p_t	/* in */     /*assoc_sec*/,
	rpc_cn_sec_context_p_t		/* in */     /*sec*/,
	pointer_t			/* in */     /*auth_value*/,
	unsigned32			/* in */     /*auth_value_len*/,
	unsigned32			/* in */     /*old_client*/,
	unsigned32			/* out */    * /*st*/
    );

INTERNAL void rpc__gssauth_cn_vfy_srvr_resp (
	rpc_cn_assoc_sec_context_p_t	/* in */     /*assoc_sec*/,
	rpc_cn_sec_context_p_t		/* in */     /*sec*/,
	pointer_t			/* in/out */ /*auth_value*/,
	unsigned32			/* in */     /*auth_value_len*/,
	unsigned32			/* out */    * /*st*/
    );

INTERNAL rpc_cn_auth_epv_t rpc_g_gssauth_negotiate_cn_epv =
{
	rpc__gssauth_cn_three_way_gss,
	rpc__gssauth_cn_context_valid,
	rpc__gssauth_negotiate_cn_create_info,
	rpc__gssauth_cn_cred_changed,
	rpc__gssauth_cn_cred_refresh,
	rpc__gssauth_cn_fmt_client_req,
	rpc__gssauth_cn_fmt_srvr_resp,
	rpc__gssauth_cn_free_prot_info,
	rpc__gssauth_cn_get_prot_info,
	rpc__gssauth_cn_pre_call,
	rpc__gssauth_cn_pre_send,
	rpc__gssauth_cn_recv_check,
	rpc__gssauth_cn_tlr_uuid_crc,
	rpc__gssauth_cn_tlr_unpack,
	rpc__gssauth_cn_vfy_client_req,
	rpc__gssauth_cn_vfy_srvr_resp
};

INTERNAL rpc_cn_auth_epv_t rpc_g_gssauth_mskrb_cn_epv =
{
	rpc__gssauth_cn_three_way_gss,
	rpc__gssauth_cn_context_valid,
	rpc__gssauth_mskrb_cn_create_info,
	rpc__gssauth_cn_cred_changed,
	rpc__gssauth_cn_cred_refresh,
	rpc__gssauth_cn_fmt_client_req,
	rpc__gssauth_cn_fmt_srvr_resp,
	rpc__gssauth_cn_free_prot_info,
	rpc__gssauth_cn_get_prot_info,
	rpc__gssauth_cn_pre_call,
	rpc__gssauth_cn_pre_send,
	rpc__gssauth_cn_recv_check,
	rpc__gssauth_cn_tlr_uuid_crc,
	rpc__gssauth_cn_tlr_unpack,
	rpc__gssauth_cn_vfy_client_req,
	rpc__gssauth_cn_vfy_srvr_resp
};

INTERNAL rpc_cn_auth_epv_t rpc_g_gssauth_winnt_cn_epv =
{
	rpc__gssauth_cn_three_way_winnt,
	rpc__gssauth_cn_context_valid,
	rpc__gssauth_winnt_cn_create_info,
	rpc__gssauth_cn_cred_changed,
	rpc__gssauth_cn_cred_refresh,
	rpc__gssauth_cn_fmt_client_req,
	rpc__gssauth_cn_fmt_srvr_resp,
	rpc__gssauth_cn_free_prot_info,
	rpc__gssauth_cn_get_prot_info,
	rpc__gssauth_cn_pre_call,
	rpc__gssauth_cn_pre_send,
	rpc__gssauth_cn_recv_check,
	rpc__gssauth_cn_tlr_uuid_crc,
	rpc__gssauth_cn_tlr_unpack,
	rpc__gssauth_cn_vfy_client_req,
	rpc__gssauth_cn_vfy_srvr_resp
};

INTERNAL rpc_cn_auth_epv_t rpc_g_gssauth_netlogon_cn_epv =
{
	rpc__gssauth_cn_three_way_gss,
	rpc__gssauth_cn_context_valid,
	rpc__gssauth_netlogon_cn_create_info,
	rpc__gssauth_cn_cred_changed,
	rpc__gssauth_cn_cred_refresh,
	rpc__gssauth_cn_fmt_client_req,
	rpc__gssauth_cn_fmt_srvr_resp,
	rpc__gssauth_cn_free_prot_info,
	rpc__gssauth_cn_get_prot_info,
	rpc__gssauth_cn_pre_call,
	rpc__gssauth_cn_pre_send,
	rpc__gssauth_cn_recv_check,
	rpc__gssauth_cn_tlr_uuid_crc,
	rpc__gssauth_cn_tlr_unpack,
	rpc__gssauth_cn_vfy_client_req,
	rpc__gssauth_cn_vfy_srvr_resp
};

/*****************************************************************************/
/*
**++
**
**  ROUTINE NAME:       rpc__gssauth_cn_three_way
**
**  SCOPE:              INTERNAL - declared locally
**
**  DESCRIPTION:
**
**      Determine whether the authentication protocol requires a
**      3-way authentication handshake. If true the client is expected to
**      provide an rpc_auth3 PDU before the security context is fully
**      established and a call can be made.
**
**  INPUTS:             none
**
**  INPUTS/OUTPUTS:     none
**
**  OUTPUTS:            none
**
**  IMPLICIT INPUTS:    none
**
**  IMPLICIT OUTPUTS:   none
**
**  FUNCTION VALUE:     boolean32
**
**      True if the authentication protocol requires a 3-way
**      authentication handshake.
**
**  SIDE EFFECTS:       none
**
**--
**/

INTERNAL boolean32 rpc__gssauth_cn_three_way_winnt (void)
{
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_cn_three_way)\n"));

	return true;
}

INTERNAL boolean32 rpc__gssauth_cn_three_way_gss (void)
{
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_cn_three_way)\n"));

	return false;
}

/*****************************************************************************/
/*
**++
**
**  ROUTINE NAME:       rpc__gssauth_cn_context_valid
**
**  SCOPE:              INTERNAL - declared locally
**
**  DESCRIPTION:
**
**      Determine whether the established security context will be
**      valid (i. e. timely) for the next 300 seconds. If
**      not this routine will try to renew the context.
**      If it cannot be renewed false is returned. This is
**      called from the client side.
**
**  INPUTS:
**
**      sec             A pointer to security context element which includes
**                      the key ID, auth information rep and RPC auth
**                      information rep.
**
**  INPUTS/OUTPUTS:     none
**
**  OUTPUTS:
**
**      st              The return status of this routine.
**
**  IMPLICIT INPUTS:    none
**
**  IMPLICIT OUTPUTS:   none
**
**  FUNCTION VALUE:     boolean32
**
**      True if security context identified by the auth
**      information rep and RPC auth information rep will
**      still be valid in 300 seconds, false if not.
**
**  SIDE EFFECTS:
**
**      The context may be renewed.
**
**--
**/

INTERNAL boolean32 rpc__gssauth_cn_context_valid
(
	rpc_cn_sec_context_p_t	sec,
	unsigned32		*st
)
{
	CODING_ERROR (st);
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_cn_context_valid)\n"));

	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_PKT,
		("(rpc__gssauth_cn_context_valid) prot->%x level->%x key_id->%x\n",
		sec->sec_info->authn_protocol,
		sec->sec_info->authn_level,
		sec->sec_key_id));

#ifdef DEBUG
	if (RPC_DBG_EXACT(rpc_es_dbg_cn_errors, RPC_C_CN_DBG_AUTH_CONTEXT_VALID)) {
		*st = RPC_S_CN_DBG_AUTH_FAILURE;
		return false;
	}
#endif

	*st = rpc_s_ok;
	return true;
}

/*****************************************************************************/
/*
**++
**
**  ROUTINE NAME:       rpc__gssauth_cn_create_info
**
**  SCOPE:              INTERNAL - declared locally
**
**  DESCRIPTION:
**
**      Create an auth information rep data structure with and
**      add a reference to it. This is called on the server
**      side. The fields will be initialized to NULL values.
**      The caller should fill them in based on information
**      decrypted from the authenticator passed in the bind
**      request.
**
**  INPUTS:
**
**      authn_level     The authentication level to be applied over this
**                      security context.
**
**  INPUTS/OUTPUTS:     none
**
**  OUTPUTS:
**
**      info            A pointer to the auth information rep structure
**                      containing RPC protocol indenpendent information.
**      st              The return status of this routine.
**
**  IMPLICIT INPUTS:    none
**
**  IMPLICIT OUTPUTS:   none
**
**  FUNCTION VALUE:     none
**
**  SIDE EFFECTS:
**
**      The newly create auth info will have a reference count of 1.
**
**--
**/

INTERNAL void rpc__gssauth_cn_create_info
(
	rpc_authn_protocol_id_t authn_protocol,
	rpc_authn_level_t	authn_level,
	rpc_auth_info_p_t	*auth_info,
	unsigned32		*st
)
{
	rpc_gssauth_info_p_t gssauth_info;

	CODING_ERROR(st);
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_cn_create_info)\n"));

	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_PKT,
		("(rpc__gssauth_cn_create_info) prot->%x level->%x\n",
		authn_protocol,
		authn_level));

#ifdef DEBUG
	if (RPC_DBG_EXACT(rpc_es_dbg_cn_errors, RPC_C_CN_DBG_AUTH_CREATE_INFO)) {
		*st = RPC_S_CN_DBG_AUTH_FAILURE;
		return;
	}
#endif

	if ((authn_level != rpc_c_authn_level_connect) &&
	    (authn_level != rpc_c_authn_level_pkt_integrity) &&
	    (authn_level != rpc_c_authn_level_pkt_privacy)) {
		*st = rpc_s_unsupported_authn_level;
		return;
	}

	/*
	 * Allocate storage for a gssauth info structure from heap.
	 */
	RPC_MEM_ALLOC(gssauth_info,
		      rpc_gssauth_info_p_t,
		      sizeof (rpc_gssauth_info_t),
		      RPC_C_MEM_GSSAUTH_INFO,
		      RPC_C_MEM_WAITOK);
	memset (gssauth_info, 0, sizeof(rpc_gssauth_info_t));

	/*
	 * Initialize the common auth_info stuff.
	 */
	gssauth_info->auth_info.refcount = 1;
	gssauth_info->auth_info.server_princ_name = '\0';
	gssauth_info->auth_info.authn_level = authn_level;
	gssauth_info->auth_info.authn_protocol = authn_protocol;
	gssauth_info->auth_info.authz_protocol = rpc_c_authz_name;
	gssauth_info->auth_info.is_server = true;

	*auth_info = &gssauth_info->auth_info;
	*st = rpc_s_ok;
}

INTERNAL void rpc__gssauth_negotiate_cn_create_info
(
	rpc_authn_level_t	authn_level,
	rpc_auth_info_p_t	*auth_info,
	unsigned32		*st
)
{
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_negotiate_cn_create_info)\n"));

	rpc__gssauth_cn_create_info(rpc_c_authn_gss_negotiate,
				    authn_level,
				    auth_info,
				    st);
}

INTERNAL void rpc__gssauth_mskrb_cn_create_info
(
	rpc_authn_level_t	authn_level,
	rpc_auth_info_p_t	*auth_info,
	unsigned32		*st
)
{
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_mskrb_cn_create_info)\n"));

	rpc__gssauth_cn_create_info(rpc_c_authn_gss_mskrb,
				    authn_level,
				    auth_info,
				    st);
}

INTERNAL void rpc__gssauth_winnt_cn_create_info
(
	rpc_authn_level_t	authn_level,
	rpc_auth_info_p_t	*auth_info,
	unsigned32		*st
)
{
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_winnt_cn_create_info)\n"));

	rpc__gssauth_cn_create_info(rpc_c_authn_winnt,
				    authn_level,
				    auth_info,
				    st);
}

INTERNAL void rpc__gssauth_netlogon_cn_create_info
(
	rpc_authn_level_t	authn_level,
	rpc_auth_info_p_t	*auth_info,
	unsigned32		*st
)
{
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_netlogon_cn_create_info)\n"));

	rpc__gssauth_cn_create_info(rpc_c_authn_netlogon,
				    authn_level,
				    auth_info,
				    st);
}

/*****************************************************************************/
/*
**++
**
**  ROUTINE NAME:       rpc__gssauth_cn_cred_changed
**
**  SCOPE:              INTERNAL - declared locally
**
**  DESCRIPTION:
**
**      Determine whether the client's credentials stored in the
**      security context are different from those in the auth info.
**      If they are not the same return true, else false.
**
**  INPUTS:
**
**      sec             A pointer to security context element which includes
**                      the key ID, auth information rep and RPC auth
**                      information rep.
**
**  INPUTS/OUTPUTS:     none
**
**  OUTPUTS:
**
**      st              The return status of this routine.
**
**  IMPLICIT INPUTS:    none
**
**  IMPLICIT OUTPUTS:   none
**
**  FUNCTION VALUE:     boolean32
**
**      True if the credentials identified by the auth
**      information rep and RPC auth information rep are different,
**      false if not.
**
**      The md5 checksum algorithm requires the use of the session key
**      to encrypt the CRC(assoc_uuid).  Since the session key will
**      change when the credential changes, this routine sets the flag
**      indicating that a (potentially) valid encrypted crc is now
**      invalid, forcing a recomputation.
**
**  SIDE EFFECTS:       none
**
**--
**/

INTERNAL boolean32 rpc__gssauth_cn_cred_changed
(
	rpc_cn_sec_context_p_t	sec,
	unsigned32		*st
)
{
	CODING_ERROR(st);
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_cn_cred_changed)\n"));

	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_PKT,
		("(rpc__gssauth_cn_cred_changed) prot->%x level->%x key_id->%x\n",
		sec->sec_info->authn_protocol,
		sec->sec_info->authn_level,
		sec->sec_key_id));

#ifdef DEBUG
	if (RPC_DBG_EXACT(rpc_es_dbg_cn_errors, RPC_C_CN_DBG_AUTH_CRED_CHANGED)) {
		*st = RPC_S_CN_DBG_AUTH_FAILURE;
		return false;
	}
#endif

	/*
	 * Assume that cred is already valid.
	 */
	*st = rpc_s_ok;
	return false;
}

/*****************************************************************************/
/*
**++
**
**  ROUTINE NAME:       rpc__gssauth_cn_cred_refresh
**
**  SCOPE:              INTERNAL - declared locally
**
**  DESCRIPTION:
**
**      Determine whether the client's credentials are still
**      valid. If not this routine will try to renew the credentials.
**      If they cannot be renewed an error is returned. This routine
**      is called from the client side.
**
**  INPUTS:
**
**      auth_info       A pointer to the auth information rep
**
**  INPUTS/OUTPUTS:     none
**
**  OUTPUTS:
**
**      st              The return status of this routine.
**
**  IMPLICIT INPUTS:    none
**
**  IMPLICIT OUTPUTS:   none
**
**  FUNCTION VALUE:     none
**
**  SIDE EFFECTS:       none
**
**--
**/

INTERNAL void rpc__gssauth_cn_cred_refresh
(
	rpc_auth_info_p_t	auth_info,
	unsigned32		*st
)
{
	CODING_ERROR(st);
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_cn_cred_refresh)\n"));

	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_PKT,
		("(rpc__gssauth_cn_cred_refresh) prot->%x level->%x\n",
		auth_info->authn_protocol,
		auth_info->authn_level));

#ifdef DEBUG
	if (RPC_DBG_EXACT(rpc_es_dbg_cn_errors, RPC_C_CN_DBG_AUTH_CRED_REFRESH)) {
		*st = RPC_S_CN_DBG_AUTH_FAILURE;
		return;
	}
#endif

	/*
	 * Assume that cred is already valid.
	 */
	*st = rpc_s_ok;
}

PRIVATE const char *rpc__gssauth_error_map
(
	int			major_status,
	OM_uint32		min_stat,
	const gss_OID		mech,
	char			*message_buffer,
	unsigned32		message_length,
	unsigned32		*st
)
{
	switch (min_stat) {
	case KRB5KRB_AP_ERR_BAD_INTEGRITY:
		*st = rpc_s_auth_bad_integrity;
		break;
	case KRB5KRB_AP_ERR_TKT_EXPIRED:
		*st = rpc_s_auth_tkt_expired;
		break;
	case KRB5KRB_AP_ERR_TKT_NYV:
		*st = rpc_s_auth_tkt_nyv;
		break;
	case KRB5KRB_AP_ERR_REPEAT:
		*st = rpc_s_auth_repeat;
		break;
	case KRB5KRB_AP_ERR_NOT_US:
		*st = rpc_s_auth_not_us;
		break;
	case KRB5KRB_AP_ERR_BADMATCH:
		*st = rpc_s_auth_badmatch;
		break;
	case KRB5KRB_AP_ERR_SKEW:
		*st = rpc_s_auth_skew;
		break;
	case KRB5KRB_AP_ERR_BADADDR:
		*st = rpc_s_auth_badaddr;
		break;
	case KRB5KRB_AP_ERR_BADVERSION:
		*st = rpc_s_auth_badversion;
		break;
	case KRB5KRB_AP_ERR_MSG_TYPE:
		*st = rpc_s_auth_msg_type;
		break;
	case KRB5KRB_AP_ERR_MODIFIED:
		*st = rpc_s_auth_modified;
		break;
	case KRB5KRB_AP_ERR_BADORDER:
		*st = rpc_s_auth_badorder;
		break;
	case KRB5KRB_AP_ERR_BADKEYVER:
		*st = rpc_s_auth_badkeyver;
		break;
	case KRB5KRB_AP_ERR_NOKEY:
		*st = rpc_s_auth_nokey;
		break;
	case KRB5KRB_AP_ERR_MUT_FAIL:
		*st = rpc_s_auth_mut_fail;
		break;
	case KRB5KRB_AP_ERR_BADDIRECTION:
		*st = rpc_s_auth_baddirection;
		break;
	case KRB5KRB_AP_ERR_METHOD:
		*st = rpc_s_auth_method;
		break;
	case KRB5KRB_AP_ERR_BADSEQ:
		*st = rpc_s_auth_badseq;
		break;
	case KRB5KRB_AP_ERR_INAPP_CKSUM:
		*st = rpc_s_auth_inapp_cksum;
		break;
	default:
		if (major_status == GSS_S_CONTINUE_NEEDED) {
		    *st = rpc_s_partial_credentials;
		    break;
		}
		*st = rpc_s_auth_method;
		break;
	}

	if (message_buffer != NULL && message_length != 0) {
		OM_uint32 min2;
		gss_buffer_desc major_msg;
		gss_buffer_desc minor_msg;
		OM_uint32 msg_ctx = 0;

		major_msg.value = NULL;
		major_msg.length = 0;
		minor_msg.value = NULL;
		minor_msg.length = 0;

		gss_display_status(&min2, major_status, GSS_C_GSS_CODE,
				   NULL, &msg_ctx, &major_msg);
		gss_display_status(&min2, min_stat, GSS_C_MECH_CODE,
				   mech, &msg_ctx, &minor_msg);

		snprintf((char *)message_buffer, message_length - 1,
			 "major: %*s (%u) minor: %*s (%u) => dcerpc: (0x%08x)\n",
			 (int)major_msg.length, (char *)major_msg.value,
			 major_status, (int)minor_msg.length,
			 (char *)minor_msg.value, min_stat,
			 (unsigned int)*st);
		message_buffer[message_length - 1] = '\0';

		gss_release_buffer(&min2, &major_msg);
		gss_release_buffer(&min2, &minor_msg);

		return (const char *)message_buffer;
	}

	return NULL;
}

#define	__GSS_MECH_SPNEGO_OID_LENGTH 6
#define	__GSS_MECH_SPNEGO_OID "\053\006\001\005\005\002"
INTERNAL const gss_OID_desc rpc__gssauth_spnego_oid =
    {__GSS_MECH_SPNEGO_OID_LENGTH, (void *)__GSS_MECH_SPNEGO_OID};

#define __GSS_MECH_KRB5_OID_LENGTH 9
#define __GSS_MECH_KRB5_OID "\052\206\110\206\367\022\001\002\002"
INTERNAL const gss_OID_desc rpc__gssauth_krb5_oid =
    {__GSS_MECH_KRB5_OID_LENGTH, (void *)__GSS_MECH_KRB5_OID};

#define __GSS_MECH_NTLM_OID_LENGTH 10
#define __GSS_MECH_NTLM_OID "\x2b\x06\x01\x04\x01\x82\x37\x02\x02\x0a"
INTERNAL const gss_OID_desc rpc__gssauth_ntlm_oid =
    {__GSS_MECH_NTLM_OID_LENGTH, (void *)__GSS_MECH_NTLM_OID};

#define __GSS_MECH_NETLOGON_OID_LENGTH 6
#define __GSS_MECH_NETLOGON_OID "\x2a\x85\x70\x2b\x0e\x02"
INTERNAL const gss_OID_desc rpc__gssauth_netlogon_oid =
    {__GSS_MECH_NETLOGON_OID_LENGTH, (void *)__GSS_MECH_NETLOGON_OID};

INTERNAL int rpc__gssauth_select_mech
(
	OM_uint32		*min_stat,
	rpc_authn_protocol_id_t	authn_protocol,
	gss_OID			*req_mech
)
{
	gss_OID selected_mech = GSS_C_NO_OID;

	*min_stat = 0;

	switch (authn_protocol) {
	case rpc_c_authn_gss_negotiate:
		selected_mech = (gss_OID)&rpc__gssauth_spnego_oid;
		break;
	case rpc_c_authn_gss_mskrb:
		selected_mech = (gss_OID)&rpc__gssauth_krb5_oid;
		break;
	case rpc_c_authn_winnt:
		selected_mech = (gss_OID)&rpc__gssauth_ntlm_oid;
		break;
	case rpc_c_authn_netlogon:
		selected_mech = (gss_OID)&rpc__gssauth_netlogon_oid;
		break;
	}

	*req_mech = selected_mech;
	return GSS_S_COMPLETE;
}

INTERNAL int rpc__gssauth_select_flags
(
	OM_uint32		*min_stat,
	rpc_authn_level_t	authn_level,
	OM_uint32		*req_flags
)
{
	OM_uint32 flags = GSS_C_MUTUAL_FLAG | GSS_C_REPLAY_FLAG |
                          GSS_C_SEQUENCE_FLAG | GSS_C_DCE_STYLE;
	*min_stat = 0;

	switch (authn_level) {
	case rpc_c_authn_level_connect:
		break;
	case rpc_c_protect_level_pkt_integ:
		flags |= GSS_C_INTEG_FLAG;
		break;
	case rpc_c_protect_level_pkt_privacy:
		flags |= GSS_C_INTEG_FLAG | GSS_C_CONF_FLAG;
		break;
	}

	*req_flags = flags;
	return GSS_S_COMPLETE;
}

INTERNAL int rpc__gssauth_create_client_token
(
	OM_uint32			*min_stat,
	const rpc_cn_sec_context_p_t	sec,
	const gss_cred_id_t		gss_creds,
	const gss_name_t		gss_server_name,
	gss_ctx_id_t			*gss_ctx,
	gss_buffer_desc			*output_token
)
{
	OM_uint32 maj_stat;
	gss_OID req_mech = GSS_C_NO_OID;
	OM_uint32 req_flags;
	gss_buffer_desc input_token;

	maj_stat = rpc__gssauth_select_mech(min_stat,
					    sec->sec_info->authn_protocol,
					    &req_mech);
	if (maj_stat != GSS_S_COMPLETE) {
		return maj_stat;
	}

	maj_stat = rpc__gssauth_select_flags(min_stat,
					     sec->sec_info->authn_level,
					     &req_flags);
	if (maj_stat != GSS_S_COMPLETE) {
		return maj_stat;
	}

	input_token.value = NULL;
	input_token.length = 0;

	maj_stat = gss_init_sec_context(min_stat,
				      gss_creds,
				      gss_ctx,
				      gss_server_name,
				      req_mech,
				      req_flags,
				      0,
				      NULL,
				      &input_token,
				      NULL,
				      output_token,
				      NULL,
				      NULL);
	return maj_stat;
}

INTERNAL int rpc__gssauth_verify_server_token
(
	OM_uint32			*min_stat,
	const rpc_cn_sec_context_p_t	sec,
	const gss_cred_id_t		gss_creds,
	const gss_name_t		gss_server_name,
	gss_ctx_id_t			gss_ctx,
	gss_buffer_desc			input_token,
	gss_buffer_desc			*output_token
)
{
	OM_uint32 maj_stat;
	gss_OID req_mech = GSS_C_NO_OID;
	OM_uint32 req_flags;

	assert(gss_ctx != GSS_C_NO_CONTEXT);

	maj_stat = rpc__gssauth_select_mech(min_stat,
					    sec->sec_info->authn_protocol,
					    &req_mech);
	if (maj_stat != GSS_S_COMPLETE) {
		return maj_stat;
	}

	maj_stat = rpc__gssauth_select_flags(min_stat,
					     sec->sec_info->authn_protocol,
					     &req_flags);
	if (maj_stat != GSS_S_COMPLETE) {
		return maj_stat;
	}

	maj_stat = gss_init_sec_context(min_stat,
				        gss_creds,
				        &gss_ctx,
				        gss_server_name,
				        req_mech,
				        req_flags,
				        0,
				        NULL,
				        &input_token,
				        NULL,
				        output_token,
				        NULL,
				        NULL);

	return maj_stat;
}

/*****************************************************************************/
/*
**++
**
**  ROUTINE NAME:       rpc__gssauth_cn_fmt_client_req
**
**  SCOPE:              INTERNAL - declared locally
**
**  DESCRIPTION:
**
**      This routine will format the auth_value field of
**      either an rpc_bind or rpc_alter_context PDU. This is
**      called from the client side association state machine.
**
**  INPUTS:
**
**      assoc_sec       A pointer to per-association security context
**                      including association UUID CRC and sequence numbers.
**      sec             A pointer to security context element which includes
**                      the key ID, auth information rep and RPC auth
**                      information rep.
**      auth_value      A pointer to the auth_value field in the rpc_bind or
**                      rpc_alter_context PDU authentication trailer.
**
**  INPUTS/OUTPUTS:
**
**      auth_value_len  On input, the lenght, in bytes of the available space
**                      for the auth_value field. On output, the lenght in
**                      bytes used in encoding the auth_value field. Zero if
**                      an error status is returned.
**
**  OUTPUTS:
**
**      st              The return status of this routine.
**
**  IMPLICIT INPUTS:    none
**
**  IMPLICIT OUTPUTS:   none
**
**  FUNCTION VALUE:     none
**
**  SIDE EFFECTS:       none
**
**--
**/

INTERNAL void rpc__gssauth_cn_fmt_client_req
(
	rpc_cn_assoc_sec_context_p_t	assoc_sec ATTRIBUTE_UNUSED,
	rpc_cn_sec_context_p_t		sec,
	pointer_t			auth_value,
	unsigned32			*auth_value_len,
	pointer_t			*last_auth_pos ATTRIBUTE_UNUSED,
	unsigned32			*auth_len_remain,
	unsigned32			old_server ATTRIBUTE_UNUSED,
	unsigned32			*st
)
{
	rpc_gssauth_info_p_t gssauth_info = (rpc_gssauth_info_p_t)sec->sec_info;
	rpc_gssauth_cn_info_p_t gssauth_cn_info = (rpc_gssauth_cn_info_p_t)sec->sec_cn_info;
	int maj_stat;
	OM_uint32 min_stat;
	gss_buffer_desc output_token = GSS_C_EMPTY_BUFFER;

	CODING_ERROR(st);
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_cn_fmt_client_req)\n"));

	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_PKT,
		("(rpc__gssauth_cn_fmt_client_req) prot->%x level->%x key_id->%x assoc_uuid_crc->%x xmit_seq->%x recv_seq->%x\n",
		sec->sec_info->authn_protocol,
		sec->sec_info->authn_level,
		sec->sec_key_id,
		assoc_sec->assoc_uuid_crc,
		assoc_sec->assoc_next_snd_seq,
		assoc_sec->assoc_next_rcv_seq));

#ifdef DEBUG
	if (RPC_DBG_EXACT(rpc_es_dbg_cn_errors, RPC_C_CN_DBG_AUTH_FMT_CLIENT_REQ)) {
		*st = RPC_S_CN_DBG_AUTH_FAILURE;
		return;
	}
#endif

	if (gssauth_cn_info->gss_ctx != GSS_C_NO_CONTEXT) {
		/*
		 * transfer the output of rpc__gssauth_verify_server_token()
		 */
		output_token.length = assoc_sec->krb_message.length;
		output_token.value = assoc_sec->krb_message.data;
		assoc_sec->krb_message.length = 0;
		assoc_sec->krb_message.data = NULL;
	} else {
		maj_stat = rpc__gssauth_create_client_token(&min_stat,
							    sec,
							    gssauth_info->gss_creds,
							    gssauth_info->gss_server_name,
							    &gssauth_cn_info->gss_ctx,
							    &output_token);
		if (GSS_ERROR(maj_stat)) {
			char msg[BUFSIZ];
			rpc__gssauth_error_map(maj_stat, min_stat,
					       (gss_OID)&rpc__gssauth_krb5_oid,
					       msg, sizeof(msg), st);
			RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_GENERAL,
				("(rpc__gssauth_cn_fmt_client_req): %s\n", msg));
			return;
		}
		gssauth_cn_info->gss_stat = maj_stat;
	}

	if (*auth_value_len < output_token.length) {
		assoc_sec->krb_message.length = output_token.length;
		assoc_sec->krb_message.data = output_token.value;

		*auth_value_len = output_token.length;
		memset((unsigned_char_p_t)auth_value, 0xAF, *auth_value_len);
	} else {
		*auth_value_len = output_token.length;
		*auth_len_remain = 0;

		memcpy((unsigned_char_p_t)auth_value,
		       output_token.value,
		       output_token.length);

		gss_release_buffer(&min_stat, &output_token);
	}

	*st = rpc_s_ok;
}

/*****************************************************************************/
/*
**++
**
**  ROUTINE NAME:       rpc__gssauth_cn_fmt_srvr_resp
**
**  SCOPE:              INTERNAL - declared locally
**
**  DESCRIPTION:
**
**      This routine will format the auth_value field of
**      either an rpc_bind_ack or rpc_alter_context_response
**      PDU. The authentication protocol encoding in the
**      credentials field of auth_value should correspond
**      to the status returned from rpc__auth_cn_vfy_
**      client_req()  routine. This credentials field, when
**      decoded by rpc__auth_cn_vfy_srvr_resp(),  should
**      result in the same error code being returned. If the
**      memory provided is not large enough an authentication
**      protocol specific error message will be encoded in
**      the credentials field indicating this error. This is
**      called from the server side association state machine.
**
**  INPUTS:
**
**      verify_st       The status code returned by rpc__auth_cn_verify_
**                      client_req().
**      assoc_sec       A pointer to per-association security context
**                      including association UUID CRC and sequence numbers.
**      sec             A pointer to security context element which includes
**                      the key ID, auth information rep and RPC auth
**                      information rep.
**      req_auth_value  A pointer to the auth_value field in the
**                      rpc_bind or rpc_alter_context PDU authentication trailer.
**      req_auth_value_len The length, in bytes, of the
**                      req_auth_value field.
**      auth_value      A pointer to the auth_value field in the rpc_bind or
**                      rpc_alter_context PDU authentication trailer.
**
**  INPUTS/OUTPUTS:
**
**      auth_value_len  On input, the length, in bytes of the available space
**                      for the auth_value field. On output, the length in
**                      bytes used in encoding the auth_value field. Zero if
**                      an error status is returned.
**
**  OUTPUTS:            none
**
**  IMPLICIT INPUTS:    none
**
**  IMPLICIT OUTPUTS:   none
**
**  FUNCTION VALUE:     none
**
**  SIDE EFFECTS:       none
**
**--
**/

INTERNAL void rpc__gssauth_cn_fmt_srvr_resp
(
	unsigned32			verify_st,
	rpc_cn_assoc_sec_context_p_t    assoc_sec,
	rpc_cn_sec_context_p_t		sec,
	pointer_t			req_auth_value ATTRIBUTE_UNUSED /*TODO*/,
	unsigned32			req_auth_value_len ATTRIBUTE_UNUSED /*TODO*/,
	pointer_t			auth_value,
	unsigned32			*auth_value_len
)
{
	OM_uint32 min_stat;
	gss_buffer_desc output_token = GSS_C_EMPTY_BUFFER;

	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_cn_fmt_srvr_resp)\n"));

	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_PKT,
		("(rpc__gssauth_cn_fmt_srvr_resp) prot->%x level->%x key_id->%x assoc_uuid_crc->%x xmit_seq->%x recv_seq->%x, vfy_client_st->%x\n",
		sec->sec_info->authn_protocol,
		sec->sec_info->authn_level,
		sec->sec_key_id,
		assoc_sec->assoc_uuid_crc,
		assoc_sec->assoc_next_snd_seq,
		assoc_sec->assoc_next_rcv_seq,
		verify_st));

#ifdef DEBUG
	if (RPC_DBG_EXACT(rpc_es_dbg_cn_errors, RPC_C_CN_DBG_AUTH_FMT_SERVER_RESP)) {
		verify_st = RPC_S_CN_DBG_AUTH_FAILURE;
	}
#endif

	/*
	 * transfer the output of rpc__gssauth_cn_vfy_client_req()
	 */
	output_token.length = assoc_sec->krb_message.length;
	output_token.value = assoc_sec->krb_message.data;
	assoc_sec->krb_message.length = 0;
	assoc_sec->krb_message.data = NULL;

	if (*auth_value_len < output_token.length) {
		gss_release_buffer(&min_stat, &output_token);
		*auth_value_len = 0;
		return;
	}

	*auth_value_len = output_token.length;

	memcpy((unsigned_char_p_t)auth_value,
	       output_token.value,
	       output_token.length);

	gss_release_buffer(&min_stat, &output_token);
}

/*****************************************************************************/
/*
**++
**
**  ROUTINE NAME:       rpc__gssauth_cn_free_prot_info
**
**  SCOPE:              INTERNAL - declared locally
**
**  DESCRIPTION:
**
**      This routine will free an NCA Connection RPC auth
**      information rep.
**
**  INPUTS:
**
**      info            A pointer to the auth information rep structure
**                      containing RPC protocol indenpendent information.
**
**  INPUTS/OUTPUTS:
**
**      cn_info         A pointer to the RPC auth information rep structure
**                      containing NCA Connection specific
**                      information. NULL on output.
**
**  OUTPUTS:            none
**
**  IMPLICIT INPUTS:    none
**
**  IMPLICIT OUTPUTS:   none
**
**  FUNCTION VALUE:     none
**
**  SIDE EFFECTS:       none
**
**--
**/

INTERNAL void rpc__gssauth_cn_free_prot_info
(
	rpc_auth_info_p_t	info,
	rpc_cn_auth_info_p_t	*cn_info
)
{
	rpc_gssauth_cn_info_p_t gssauth_cn_info = (rpc_gssauth_cn_info_p_t)*cn_info;
	OM_uint32 min_stat;

	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_cn_free_prot_info)\n"));

	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_PKT,
		("(rpc__gssauth_cn_free_prot_info) prot->%x level->%x \n",
		info->authn_protocol,
		info->authn_level));

	if (gssauth_cn_info->gss_ctx != GSS_C_NO_CONTEXT) {
		gss_delete_sec_context(&min_stat,
				       &gssauth_cn_info->gss_ctx,
				       GSS_C_NO_BUFFER);
		gssauth_cn_info->gss_ctx = GSS_C_NO_CONTEXT;
	}

#ifdef DEBUG
	memset (gssauth_cn_info, 0, sizeof (rpc_gssauth_cn_info_p_t));
#endif

	RPC_MEM_FREE (gssauth_cn_info, RPC_C_MEM_GSSAUTH_CN_INFO);
	*cn_info = NULL;
}

/*****************************************************************************/
/*
**++
**
**  ROUTINE NAME:       rpc__gssauth_cn_get_prot_info
**
**  SCOPE:              INTERNAL - declared locally
**
**  DESCRIPTION:
**
**      This routine will create and return an NCA Connection
**      RPC auth information rep.
**
**  INPUTS:
**
**      info            A pointer to the auth information rep structure
**                      containing RPC protocol indenpendent information.
**
**  INPUTS/OUTPUTS:     none
**
**  OUTPUTS:
**
**      cn_info         A pointer to the RPC auth information rep structure
**                      containing NCA Connection specific information.
**      st              The return status of this routine.
**
**  IMPLICIT INPUTS:    none
**
**  IMPLICIT OUTPUTS:   none
**
**  FUNCTION VALUE:     none
**
**  SIDE EFFECTS:       none
**
**--
**/

INTERNAL void rpc__gssauth_cn_get_prot_info
(
	rpc_auth_info_p_t		info,
	rpc_cn_auth_info_p_t		*cn_info,
	unsigned32			*st
)
{
        rpc_gssauth_cn_info_t   *gssauth_cn_info = NULL;
	rpc_gssauth_info_t      *gssauth_info = (rpc_gssauth_info_p_t)info;

	CODING_ERROR(st);
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_cn_get_prot_info)\n"));

	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_PKT,
		("(rpc__gssauth_cn_get_prot_info) prot->%x level->%x \n",
		info->authn_protocol,
		info->authn_level));

#ifdef DEBUG
	if (RPC_DBG_EXACT(rpc_es_dbg_cn_errors, RPC_C_CN_DBG_AUTH_GET_PROT_INFO)) {
		*st = RPC_S_CN_DBG_AUTH_FAILURE;
		return;
	}
#endif

	/*
	 * Allocate storage for a gssauth cn info structure from heap.
	 */
	RPC_MEM_ALLOC(gssauth_cn_info,
		      rpc_gssauth_cn_info_p_t,
		      sizeof (rpc_gssauth_cn_info_t),
		      RPC_C_MEM_GSSAUTH_CN_INFO,
		      RPC_C_MEM_WAITOK);

	/*
	 * Initialize it.
	 */
	memset (gssauth_cn_info, 0, sizeof(rpc_gssauth_cn_info_t));

	/*
	 * Set pointer to connection specific auth info if we're
	 * on the server side.
	 */
	if (info->is_server) {
		gssauth_info->cn_info = gssauth_cn_info;
	}

	*cn_info = &gssauth_cn_info->cn_info;
	*st = rpc_s_ok;
}

/*****************************************************************************/
/*
**++
**
**  ROUTINE NAME:       rpc__gssauth_cn_pre_call
**
**  SCOPE:              INTERNAL - declared locally
**
**  DESCRIPTION:
**
**      This routine will format the auth_value field of
**      a call level PDU, namely an rpc_request, rpc_response
**      rpc_fault, rpc_remote_alert or rpc_orphaned PDU. It will
**      also format the auth_value field of the association level
**      rpc_shutdown PDU. This does
**      not include generating any checksums in the auth_value_field
**      or encrypting of data corresponding to the authentication
**      level. That will be done in the rpc__cn_auth_pre_send
**      routine. This is called on both client and server when a the
**      data structures and header template for a call is being set up.
**
**  INPUTS:
**
**      assoc_sec       A pointer to per-association security context
**                      including association UUID CRC and sequence numbers.
**      sec             A pointer to security context element which includes
**                      the key ID, auth information rep and RPC auth
**                      information rep.
**      auth_value      A pointer to the auth_value field in the rpc_bind or
**                      rpc_alter_context PDU authentication trailer.
**
**  INPUTS/OUTPUTS:
**
**      auth_value_len  On input, the lenght, in bytes of the available space
**                      for the auth_value field. On output, the lenght in
**                      bytes used in encoding the auth_value field. Zero if
**                      an error status is returned.
**
**  OUTPUTS:
**
**      st              The return status of this routine.
**
**  IMPLICIT INPUTS:    none
**
**  IMPLICIT OUTPUTS:   none
**
**  FUNCTION VALUE:     none
**
**  SIDE EFFECTS:       none
**
**--
**/

INTERNAL void rpc__gssauth_cn_pre_call
(
	rpc_cn_assoc_sec_context_p_t	assoc_sec,
	rpc_cn_sec_context_p_t		sec,
	pointer_t			auth_value,
	unsigned32			*auth_value_len,
	unsigned32			*st
)
{
	rpc_gssauth_cn_info_p_t gssauth_cn_info = (rpc_gssauth_cn_info_p_t)sec->sec_cn_info;
	OM_uint32 maj_stat;
	OM_uint32 min_stat;
	gss_iov_buffer_desc iov[2];
	int conf_req_flag = 0;

	CODING_ERROR(st);

	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_cn_pre_call)\n"));

	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_PKT,
		("(rpc__gssauth_cn_pre_call) prot->%x level->%x key_id->%x assoc_uuid_crc->%x xmit_seq->%x recv_seq->%x\n",
		sec->sec_info->authn_protocol,
		sec->sec_info->authn_level,
		sec->sec_key_id,
		assoc_sec->assoc_uuid_crc,
		assoc_sec->assoc_next_snd_seq,
		assoc_sec->assoc_next_rcv_seq));

#ifdef DEBUG
	if (RPC_DBG_EXACT(rpc_es_dbg_cn_errors, RPC_C_CN_DBG_AUTH_PRE_CALL)) {
		*st = RPC_S_CN_DBG_AUTH_FAILURE;
		return;
	}
#endif

	switch (sec->sec_info->authn_level) {
	case rpc_c_protect_level_pkt_privacy:
		conf_req_flag = 1;
		/* fallthrough */
	case rpc_c_protect_level_pkt_integ:
		iov[0].type = GSS_IOV_BUFFER_TYPE_DATA;
		iov[0].buffer.length = RPC__GSSAUTH_CN_AUTH_PADDING;

		iov[1].type = GSS_IOV_BUFFER_TYPE_HEADER;
		iov[1].buffer.length = 0;

		maj_stat = gss_wrap_iov_length(&min_stat,
					       gssauth_cn_info->gss_ctx,
					       conf_req_flag,
					       GSS_C_QOP_DEFAULT,
					       NULL, /* conf_state */
					       iov,
					       sizeof(iov)/sizeof(iov[0]));
		if (GSS_ERROR(maj_stat)) {
			*st = rpc_s_auth_method;
			return;
		}
		*auth_value_len = iov[1].buffer.length;
		memset((unsigned_char_p_t)auth_value, 0xAF, *auth_value_len);
		break;
	case rpc_c_authn_level_connect:
	default:
		*auth_value_len = 0;
		break;
	}

	*st = rpc_s_ok;
}

INTERNAL void rpc__gssauth_cn_wrap_packet
(
	rpc_cn_sec_context_p_t		sec,
	const char			*comment,
	unsigned8			header_size,
	rpc_socket_iovec_p_t		iov,
	unsigned32			iovlen,
	boolean                         conf_req_flag,
	rpc_socket_iovec_p_t		out_iov,
	unsigned32			*st
)
{
	rpc_gssauth_cn_info_p_t gssauth_cn_info = (rpc_gssauth_cn_info_p_t)sec->sec_cn_info;
	rpc_cn_common_hdr_p_t pdu;
	rpc_cn_auth_tlr_p_t auth_tlr;
	unsigned32 i;
	unsigned32 payload_len;
	unsigned8 pad_len;
	OM_uint32 maj_stat;
	OM_uint32 min_stat;
	int conf_state;
	gss_iov_buffer_desc gss_iov[4];
	unsigned_char_p_t pdu_buf;
	size_t pdu_buflen;
	boolean header_sign = gssauth_cn_info->header_sign;

	CODING_ERROR(st);
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_cn_wrap_packet)\n"));

	if (iovlen < 2) {
		RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_GENERAL,
			("(rpc__gssauth_cn_wrap_packet): %s: iovlen[%u]\n",
			comment, (unsigned int)iovlen));
		*st = rpc_m_no_stub_data;
		return;
	}
	if (iov[0].iov_len < header_size) {
		RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_GENERAL,
			("(rpc__gssauth_cn_wrap_packet): %s: iov[0].iov_len[%u] < header_size[%u]\n",
			comment, (unsigned int)iov[0].iov_len,(unsigned int)header_size));
		*st = rpc_m_no_stub_data;
		return;
	}
	if (iov[iovlen - 1].iov_len < RPC_CN_PKT_SIZEOF_COM_AUTH_TLR) {
		RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_GENERAL,
			("(rpc__gssauth_cn_wrap_packet): %s: iov[%u].iov_len[%u] != %u\n",
			comment,(unsigned int)iovlen - 1,
			(unsigned int)iov[iovlen - 1].iov_len,
			RPC_CN_PKT_SIZEOF_COM_AUTH_TLR));
		*st = rpc_s_auth_field_toolong;
		return;
	}

	for (i = 0, payload_len = 0; i < iovlen - 1; i++) {
		payload_len += iov[i].iov_len;
	}

	payload_len -= header_size;
	pad_len = RPC__GSSAUTH_CN_AUTH_PADDING -
		  (payload_len % RPC__GSSAUTH_CN_AUTH_PADDING);
	if (pad_len == RPC__GSSAUTH_CN_AUTH_PADDING)
		pad_len = 0;

	pdu_buflen = header_size + payload_len + pad_len +
		     iov[iovlen - 1].iov_len;

	RPC_MEM_ALLOC(pdu_buf,
		      unsigned_char_p_t,
		      pdu_buflen,
		      RPC_C_MEM_CN_ENCRYPT_BUF,
		      RPC_C_MEM_WAITOK);

	pdu = (rpc_cn_common_hdr_p_t)pdu_buf;

	for (i = 0, pdu_buflen = 0; i < iovlen - 1; i++) {
		memcpy(&pdu_buf[pdu_buflen],
		       iov[i].iov_base,
		       iov[i].iov_len);
		pdu_buflen += iov[i].iov_len;
	}

	memset(&pdu_buf[pdu_buflen], 0, pad_len);
	pdu_buflen += pad_len;

	gss_iov[0].type = header_sign ? GSS_IOV_BUFFER_TYPE_SIGN_ONLY :
					GSS_IOV_BUFFER_TYPE_EMPTY;
	gss_iov[0].buffer.value = pdu_buf;
	gss_iov[0].buffer.length = header_size;

	gss_iov[1].type = GSS_IOV_BUFFER_TYPE_DATA;
	gss_iov[1].buffer.value = &pdu_buf[header_size];
	gss_iov[1].buffer.length = payload_len + pad_len;

	gss_iov[2].type = header_sign ? GSS_IOV_BUFFER_TYPE_SIGN_ONLY :
					GSS_IOV_BUFFER_TYPE_EMPTY;
	gss_iov[2].buffer.value = &pdu_buf[header_size + pad_len];
	gss_iov[2].buffer.length = RPC_CN_PKT_SIZEOF_COM_AUTH_TLR;

	gss_iov[3].type = GSS_IOV_BUFFER_TYPE_HEADER;
	gss_iov[3].buffer.value = &pdu_buf[header_size + pad_len +
					     RPC_CN_PKT_SIZEOF_COM_AUTH_TLR];
	gss_iov[3].buffer.length = iov[iovlen - 1].iov_len -
				   RPC_CN_PKT_SIZEOF_COM_AUTH_TLR;

	if (conf_req_flag) {
		maj_stat = gss_wrap_iov(&min_stat,
					gssauth_cn_info->gss_ctx,
					conf_req_flag,
					GSS_C_QOP_DEFAULT,
					&conf_state,
					gss_iov,
					sizeof(gss_iov)/sizeof(gss_iov[0]));
		if (GSS_ERROR(maj_stat))
			goto cleanup;
	} else {
		gss_buffer_desc input_token;
		gss_buffer_desc mic_token;

		conf_state = 0;

		if (header_sign) {
			input_token = gss_iov[0].buffer;
			input_token.length += gss_iov[1].buffer.length + gss_iov[2].buffer.length;
		} else
			input_token = gss_iov[1].buffer;

		maj_stat = gss_get_mic(&min_stat,
				       gssauth_cn_info->gss_ctx,
				       GSS_C_QOP_DEFAULT,
				       &input_token,
				       &mic_token);
		if (GSS_ERROR(maj_stat))
			goto cleanup;

		assert(mic_token.length <=
		       gss_iov[3].buffer.length);
		memcpy(gss_iov[3].buffer.value,
		       mic_token.value, mic_token.length);
		gss_release_buffer(&min_stat, &mic_token);
	}

	if (conf_req_flag != conf_state) {
		RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_GENERAL,
			("(rpc__gssauth_cn_wrap_packet): %s: conf_req[%u] != conf_state[%u]\n",
			comment, (unsigned int)conf_req_flag, (unsigned int)conf_state));
		*st = rpc_s_auth_method;
		goto cleanup;
	}

	pdu->frag_len        = gss_iov[0].buffer.length +
			       gss_iov[1].buffer.length +
			       gss_iov[2].buffer.length +
			       gss_iov[3].buffer.length;

	pdu->auth_len        = gss_iov[3].buffer.length;
	auth_tlr->stub_pad_length = pad_len;

	out_iov->iov_base = pdu_buf;
	out_iov->iov_len  = pdu->frag_len;

	*st = rpc_s_ok;

cleanup:
	if (maj_stat != GSS_S_COMPLETE) {
		char msg[BUFSIZ];
		rpc__gssauth_error_map(maj_stat, min_stat,
				       (gss_OID)&rpc__gssauth_krb5_oid,
				       msg, sizeof(msg), st);
		RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_GENERAL,
			("(rpc__gssauth_cn_wrap_packet): %s: %s\n", comment, msg));
	}

	return;
}

INTERNAL void rpc__gssauth_cn_create_large_frag
(
	rpc_cn_assoc_sec_context_p_t	assoc_sec,
	const char			*comment,
	unsigned8			header_size,
	rpc_socket_iovec_p_t		iov,
	unsigned32			iovlen,
	rpc_socket_iovec_p_t		out_iov,
	unsigned32			*st
)
{
	rpc_cn_common_hdr_p_t pdu;
	rpc_cn_packet_p_t pkt = (rpc_cn_packet_p_t)pdu;
	unsigned32 i;
	unsigned32 pdu_buflen;
	unsigned_char_p_t pdu_buf, pdu_bufp;
	OM_uint32 min_stat;
	gss_buffer_desc output_token;

	CODING_ERROR(st);
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_cn_create_large_frag)\n"));

	output_token.length = assoc_sec->krb_message.length;
	output_token.value = assoc_sec->krb_message.data;
	assoc_sec->krb_message.length = 0;
	assoc_sec->krb_message.data = NULL;

#if 0
	RPC_CN_PKT_FLAGS(pkt) |= RPC_C_CN_FLAGS_SUPPORT_HEADER_SIGN;
#endif

	if (iovlen < 1) {
		RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_GENERAL,
			("(rpc__gssauth_cn_create_large_frag): %s: iovlen[%u]\n",
			comment, (unsigned int)iovlen));
		gss_release_buffer(&min_stat, &output_token);
		*st = rpc_m_no_stub_data;
		return;
	}
	if (iov[0].iov_len < header_size) {
		RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_GENERAL,
			("(rpc__gssauth_cn_create_large_frag): %s: iov[0].iov_len[%u] < header_size[%u]\n",
			comment, (unsigned int)iov[0].iov_len,(unsigned int)header_size));
		gss_release_buffer(&min_stat, &output_token);
		*st = rpc_m_no_stub_data;
		return;
	}

	for (i = 0, pdu_buflen = 0; i < iovlen; i++) {
		pdu_buflen += iov[i].iov_len;
	}

	if (pdu_buflen < header_size + RPC_CN_PKT_SIZEOF_COM_AUTH_TLR + output_token.length) {
		gss_release_buffer(&min_stat, &output_token);
		RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_GENERAL,
			("(rpc__gssauth_cn_create_large_frag): %s: pdu_buflen[%u] < min_len[%u]\n",
			comment, (unsigned int)pdu_buflen,
			(unsigned int)header_size +
				RPC_CN_PKT_SIZEOF_COM_AUTH_TLR + output_token.length));
		*st = rpc_m_no_stub_data;
		return;
	}

	pdu_buflen += output_token.length;

	RPC_MEM_ALLOC(pdu_buf,
		      unsigned_char_p_t,
		      pdu_buflen,
		      RPC_C_MEM_CN_ENCRYPT_BUF,
		      RPC_C_MEM_WAITOK);

	pdu_bufp = pdu_buf;
	pdu = (rpc_cn_common_hdr_p_t)pdu_bufp;
	for (i = 0; i < iovlen; i++) {
		memcpy(pdu_bufp, iov[i].iov_base, iov[i].iov_len);
		pdu_bufp += iov[i].iov_len;
	}

	memcpy(pdu_bufp, output_token.value, output_token.length);
	pdu_bufp += output_token.length;

	pdu->frag_len = (pdu_bufp - pdu_buf);
	pdu->auth_len = output_token.length;

	out_iov->iov_base = pdu_buf;
	out_iov->iov_len = pdu_buflen;

	assert(out_iov->iov_len == pdu->frag_len);

	gss_release_buffer(&min_stat, &output_token);

	*st = rpc_s_ok;
}

/*****************************************************************************/
/*
**++
**
**  ROUTINE NAME:       rpc__gssauth_cn_pre_send
**
**  SCOPE:              INTERNAL - declared locally
**
**  DESCRIPTION:
**
**      This routine will perform per-packet security
**      processing on a packet before it is sent. This
**      includes checksumming and encryption.
**
**      Note that in some cases, the data is copied to
**      a contiguous buffer for checksumming and
**      encryption.  In these cases, the contiguous
**      iov element should be used instead of the original
**      iovector.
**
**  INPUTS:
**
**      assoc_sec       A pointer to per-association security context
**                      including association UUID CRC and sequence numbers.
**      sec             A pointer to security context element which includes
**                      the key ID, auth information rep and RPC auth
**                      information rep.
**      iov             A pointer to the iovector containing the PDU
**                      about to be sent. The appropriate per-packet security
**                      services will be applied to it.
**      iovlen          The length, in bytes, of the PDU.
**      out_iov         An iovector element.  This iovector element
**                      will describe packet if the original iov
**                      had to be copied.  If the original iov was
**                      copied, out_iov->iov_base will be non-NULL.
**
**  INPUTS/OUTPUTS:     none
**
**  OUTPUTS:
**
**      st              The return status of this routine.
**
**  IMPLICIT INPUTS:    none
**
**  IMPLICIT OUTPUTS:   none
**
**  FUNCTION VALUE:     none
**
**  SIDE EFFECTS:       none
**
**--
**/

INTERNAL void rpc__gssauth_cn_pre_send
(
	rpc_cn_assoc_sec_context_p_t	assoc_sec,
	rpc_cn_sec_context_p_t		sec,
	rpc_socket_iovec_p_t		iov,
	unsigned32			iovlen,
	rpc_socket_iovec_p_t		out_iov,
	unsigned32			*st
)
{
	rpc_gssauth_cn_info_p_t gssauth_cn_info = (rpc_gssauth_cn_info_p_t)sec->sec_cn_info;
	rpc_cn_common_hdr_p_t pdu;
	unsigned32 ptype;
	boolean conf_req_flag = false;
	OM_uint32 maj_stat;
	OM_uint32 min_stat;

	CODING_ERROR(st);
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_cn_pre_send)\n"));

	pdu = (rpc_cn_common_hdr_p_t)iov[0].iov_base;
	rpc_cn_packet_p_t pkt = (rpc_cn_packet_p_t)pdu;
	ptype = pdu->ptype;
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_GENERAL,
		("(rpc__gssauth_cn_pre_send) authn level->%x packet type->%x\n",
		sec->sec_info->authn_level, ptype));

	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_PKT,
		("(rpc__gssauth_cn_pre_send) prot->%x level->%x key_id->%x assoc_uuid_crc->%x xmit_seq->%x recv_seq->%x ptype->%x\n",
		sec->sec_info->authn_protocol,
		sec->sec_info->authn_level,
		sec->sec_key_id,
		assoc_sec->assoc_uuid_crc,
		assoc_sec->assoc_next_snd_seq,
		assoc_sec->assoc_next_rcv_seq,
		ptype));

#ifdef DEBUG
	if (RPC_DBG_EXACT(rpc_es_dbg_cn_errors, RPC_C_CN_DBG_AUTH_PRE_SEND)) {
		*st = RPC_S_CN_DBG_AUTH_FAILURE;
		return;
	}
#endif

	out_iov->iov_base = NULL;

	switch (ptype) {
	case RPC_C_CN_PKT_REQUEST:
	case RPC_C_CN_PKT_RESPONSE:
		switch (sec->sec_info->authn_level) {
		case rpc_c_authn_level_connect:
			*st = rpc_s_ok;
			break;
		case rpc_c_authn_level_pkt_privacy:
			conf_req_flag = true;
			/* fallthrough */
		case rpc_c_authn_level_pkt_integrity:
			rpc__gssauth_cn_wrap_packet(sec,
				ptype == RPC_C_CN_PKT_REQUEST ? "request" : "response",
				ptype == RPC_C_CN_PKT_REQUEST ? RPC_CN_PKT_SIZEOF_RQST_HDR :
								RPC_CN_PKT_SIZEOF_RESP_HDR,
				iov, iovlen, conf_req_flag, out_iov, st);
			break;
		default:
			break;
		}
	case RPC_C_CN_PKT_BIND:
	case RPC_C_CN_PKT_ALTER_CONTEXT:
		if (assoc_sec->krb_message.length == 0) {
			*st = rpc_s_ok;
		} else {
			rpc__gssauth_cn_create_large_frag(assoc_sec,
				(ptype == RPC_C_CN_PKT_BIND) ? "bind" : "alter_context",
				RPC_CN_PKT_SIZEOF_BIND_HDR,
				iov, iovlen, out_iov, st);
		}
		break;
	case RPC_C_CN_PKT_BIND_ACK:
	case RPC_C_CN_PKT_BIND_NAK:
	case RPC_C_CN_PKT_ALTER_CONTEXT_RESP:
		if (gssauth_cn_info->header_sign)
			RPC_CN_PKT_FLAGS(pkt) |= RPC_C_CN_FLAGS_SUPPORT_HEADER_SIGN;
		break;
	case RPC_C_CN_PKT_AUTH3: {
		int locally_initiated = 0;
		gss_buffer_desc input_token, output_token;
		rpc_cn_auth_tlr_p_t auth_tlr;

		maj_stat = gss_inquire_context(&min_stat,
					       gssauth_cn_info->gss_ctx,
					       NULL,
					       NULL,
					       NULL,
					       NULL,
					       NULL,
					       &locally_initiated,
					       NULL);
		if (GSS_ERROR(maj_stat) || !locally_initiated)
			break;

		assert(iovlen == 1);

		auth_tlr = RPC_CN_PKT_AUTH_TLR(pkt, iov[0].iov_len);
		input_token.value = auth_tlr->auth_value;
		input_token.length = RPC_CN_PKT_AUTH_LEN(pkt);

		maj_stat = rpc__gssauth_verify_server_token(&min_stat,
							    sec,
							    GSS_C_NO_CREDENTIAL,
							    GSS_C_NO_NAME,
							    gssauth_cn_info->gss_ctx,
							    input_token,
							    &output_token);
		if (output_token.length) {
			*st = rpc_s_credentials_too_large;
			gss_release_buffer(&min_stat, &output_token);
			return;
		}
		break;
	}
	case RPC_C_CN_PKT_FAULT:
	case RPC_C_CN_PKT_SHUTDOWN:
	case RPC_C_CN_PKT_REMOTE_ALERT:
	case RPC_C_CN_PKT_ORPHANED:
	default:
		break;
	}
}

INTERNAL void rpc__gssauth_cn_unwrap_packet
(
	rpc_cn_sec_context_p_t		sec,
	const char			*comment,
	unsigned8			header_size,
	rpc_cn_common_hdr_p_t		pdu,
	unsigned32			pdu_len,
	unsigned32			cred_len,
	rpc_cn_auth_tlr_p_t		auth_tlr,
	boolean                         conf_req_flag,
	boolean32			unpack_ints,
	unsigned32			*st
)
{
	rpc_gssauth_cn_info_p_t gssauth_cn_info = (rpc_gssauth_cn_info_p_t)sec->sec_cn_info;
	unsigned32 payload_len;
	OM_uint32 maj_stat;
	OM_uint32 min_stat;
	int conf_state;
	gss_qop_t qop_state;
	unsigned16 auth_len;
	gss_iov_buffer_desc gss_iov[4];
	boolean header_sign = gssauth_cn_info->header_sign;

	CODING_ERROR(st);
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_cn_unwrap_packet)\n"));

	auth_len = pdu->auth_len;
	if (unpack_ints) {
		SWAB_INPLACE_16(auth_len);
	}

	gss_iov[0].type = header_sign ? GSS_IOV_BUFFER_TYPE_SIGN_ONLY :
					GSS_IOV_BUFFER_TYPE_EMPTY;
	gss_iov[0].buffer.value = (unsigned_char_p_t)pdu;
	gss_iov[0].buffer.length = header_size;

	gss_iov[1].type = GSS_IOV_BUFFER_TYPE_DATA;
	gss_iov[1].buffer.value = (unsigned_char_p_t)pdu + header_size;
	gss_iov[1].buffer.length = pdu_len -
		(header_size + RPC_CN_PKT_SIZEOF_COM_AUTH_TLR + auth_len);

	gss_iov[2].type = header_sign ? GSS_IOV_BUFFER_TYPE_SIGN_ONLY :
					GSS_IOV_BUFFER_TYPE_EMPTY;
	gss_iov[2].buffer.value = auth_tlr;
	gss_iov[2].buffer.length = RPC_CN_PKT_SIZEOF_COM_AUTH_TLR;

	gss_iov[3].type = GSS_IOV_BUFFER_TYPE_HEADER;
	gss_iov[3].buffer.value = (unsigned_char_p_t)auth_tlr +
		RPC_CN_PKT_SIZEOF_COM_AUTH_TLR;
	gss_iov[3].buffer.length = auth_len;

	if (conf_req_flag) {
		maj_stat = gss_unwrap_iov(&min_stat,
					  gssauth_cn_info->gss_ctx,
					  &conf_state,
					  &qop_state,
					  gss_iov,
					  sizeof(gss_iov)/sizeof(gss_iov[0]));
	} else {
		gss_buffer_desc input_token;
		gss_buffer_desc mic_token;

		conf_state = 0;

		if (header_sign) {
			input_token = gss_iov[0].buffer;
			input_token.length += gss_iov[1].buffer.length + gss_iov[2].buffer.length;
		} else
			input_token = gss_iov[1].buffer;

		maj_stat = gss_verify_mic(&min_stat,
					  gssauth_cn_info->gss_ctx,
					  &input_token,
					  &mic_token,
					  &qop_state);
	}

	if (GSS_ERROR(maj_stat)) {
		char msg[BUFSIZ];
		rpc__gssauth_error_map(maj_stat, min_stat,
				       (gss_OID)&rpc__gssauth_krb5_oid,
				       msg, sizeof(msg), st);
		RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_GENERAL,
			("(rpc__gssauth_cn_unwrap_packet): %s: %s\n",
			comment, msg));
		/* *st is already filled */
	} else if (conf_state != conf_req_flag) {
		RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_GENERAL,
			("(rpc__gssauth_cn_unwrap_packet): %s: packet privacy mismatch\n",
			comment));
		*st = rpc_s_auth_bad_integrity;
	} else {
		*st = rpc_s_ok;
	}

	return;
}

/*****************************************************************************/
/*
**++
**
**  ROUTINE NAME:       rpc__gssauth_cn_recv_check
**
**  SCOPE:              INTERNAL - declared locally
**
**  DESCRIPTION:
**
**      This routine will perform per-packet security
**      processing on a packet after it is received. This
**      includes decryption and verification of checksums.
**
**  INPUTS:
**
**      assoc_sec       A pointer to per-association security context
**                      including association UUID CRC and sequence numbers.
**      sec             A pointer to security context element which includes
**                      the key ID, auth information rep and RPC auth
**                      information rep.
**      pdu             A pointer to the PDU about to be sent. The appropriate
**                      per-packet security services will be applied to it.
**      pdu_len         The length, in bytes, of the PDU.
**      cred_len        The length, in bytes, of the credentials.
**      auth_tlr        A pointer to the auth trailer.
**      unpack_ints     A boolean indicating whether the integer rep
**                      of fields in the pdu need to be adjusted for
**                      endian differences.
**
**  INPUTS/OUTPUTS:     none
**
**  OUTPUTS:
**
**      st              The return status of this routine.
**
**  IMPLICIT INPUTS:    none
**
**  IMPLICIT OUTPUTS:   none
**
**  FUNCTION VALUE:     none
**
**  SIDE EFFECTS:       none
**
**--
**/

INTERNAL void rpc__gssauth_cn_recv_check
(
	rpc_cn_assoc_sec_context_p_t    assoc_sec,
	rpc_cn_sec_context_p_t		sec,
	rpc_cn_common_hdr_p_t		pdu,
	unsigned32			pdu_len,
	unsigned32			cred_len,
	rpc_cn_auth_tlr_p_t		auth_tlr,
	boolean32			unpack_ints,
	unsigned32			*st
)
{
	rpc_gssauth_cn_info_p_t gssauth_cn_info = (rpc_gssauth_cn_info_p_t)sec->sec_cn_info;
	boolean conf_req_flag = false;
	rpc_cn_packet_p_t pkt = (rpc_cn_packet_p_t)pdu;

	CODING_ERROR(st);
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_cn_recv_check)\n"));

	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_GENERAL,
		("(rpc__gssauth_cn_recv_check) authn level->%x packet type->%x\n",
		sec->sec_info->authn_level,
		pdu->ptype));

	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_PKT,
		("(rpc__gssauth_cn_recv_check) prot->%x level->%x key_id->%x assoc_uuid_crc->%x xmit_seq->%x recv_seq->%x ptype->%x\n",
		sec->sec_info->authn_protocol,
		sec->sec_info->authn_level,
		sec->sec_key_id,
		assoc_sec->assoc_uuid_crc,
		assoc_sec->assoc_next_snd_seq,
		assoc_sec->assoc_next_rcv_seq,
		pdu->ptype));

#ifdef DEBUG
	if (RPC_DBG_EXACT(rpc_es_dbg_cn_errors, RPC_C_CN_DBG_AUTH_RECV_CHECK)) {
		*st = RPC_S_CN_DBG_AUTH_FAILURE;
		return;
	}
#endif

	switch (pdu->ptype) {
	case RPC_C_CN_PKT_REQUEST:
	case RPC_C_CN_PKT_RESPONSE:
		switch (sec->sec_info->authn_level) {
		case rpc_c_authn_level_connect:
			*st = rpc_s_ok;
			break;
		case rpc_c_authn_level_pkt_privacy:
			conf_req_flag = true;
			/* fallthrough */
		case rpc_c_authn_level_pkt_integrity:
			rpc__gssauth_cn_unwrap_packet(sec,
				(pdu->ptype == RPC_C_CN_PKT_REQUEST) ?  "request" : "response",
				(pdu->ptype == RPC_C_CN_PKT_REQUEST) ?  RPC_CN_PKT_SIZEOF_RQST_HDR : RPC_CN_PKT_SIZEOF_RESP_HDR,
				pdu, pdu_len, cred_len, auth_tlr, conf_req_flag,
				unpack_ints, st);
			break;
		}
		break;
	case RPC_C_CN_PKT_AUTH3:
	case RPC_C_CN_PKT_BIND:
	case RPC_C_CN_PKT_BIND_ACK:
	case RPC_C_CN_PKT_BIND_NAK:
	case RPC_C_CN_PKT_ALTER_CONTEXT:
	case RPC_C_CN_PKT_ALTER_CONTEXT_RESP:
		if (RPC_CN_PKT_FLAGS(pkt) & RPC_C_CN_FLAGS_SUPPORT_HEADER_SIGN)
			gssauth_cn_info->header_sign = true;
		break;
	case RPC_C_CN_PKT_SHUTDOWN:
	case RPC_C_CN_PKT_REMOTE_ALERT:
	case RPC_C_CN_PKT_ORPHANED:
	case RPC_C_CN_PKT_FAULT:
	default:
		break;
	}

	*st = rpc_s_ok;
}

/*****************************************************************************/
/*
**++
**
**  ROUTINE NAME:       rpc__gssauth_cn_tlr_uuid_crc
**
**  SCOPE:              INTERNAL - declared locally
**
**  DESCRIPTION:
**
**      This routine will locate and return the association
**      UUID CRC contained in the auth_value field of an
**      authentication trailer of an rpc_bind, rpc_bind_ack,
**      rpc_alter_context or rpc_alter_context_response PDU.
**
**  INPUTS:
**
**      auth_value      A pointer to the auth_value field in an authentication
**                      trailer.
**      auth_value_len  The length, in bytes, of the auth_value field.
**
**  INPUTS/OUTPUTS:     none
**
**  OUTPUTS:
**
**      assoc_uuid_crc  The association UUID CRC contained in the auth_value
**                      field.
**
**  IMPLICIT INPUTS:    none
**
**  IMPLICIT OUTPUTS:   none
**
**  FUNCTION VALUE:     none
**
**  SIDE EFFECTS:       none
**
**--
**/

INTERNAL void rpc__gssauth_cn_tlr_uuid_crc
(
	pointer_t		auth_value ATTRIBUTE_UNUSED /*TODO*/,
	unsigned32		auth_value_len ATTRIBUTE_UNUSED /*TODO*/,
	unsigned32		*uuid_crc
)
{
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_cn_tlr_uuid_crc)\n"));

	*uuid_crc = 0;

	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_GENERAL,
		("(rpc__gssauth_cn_tlr_uuid_crc) assoc_uuid_crc->%x\n", *uuid_crc));
}

/*****************************************************************************/
/*
**++
**
**  ROUTINE NAME:       rpc__gssauth_cn_tlr_unpack
**
**  SCOPE:              INTERNAL - declared locally
**
**  DESCRIPTION:
**
**      This routine will byte swap all the appropriate fields
**      of the the auth_value field of an authentication
**      trailer. It will also convert any characters from
**      the remote representation into the local, for example,
**      ASCII to EBCDIC.
**
**  INPUTS:
**
**      auth_value_len  The length, in bytes, of the auth_value field.
**      packed_drep     The packed Networ Data Representation, (see NCA RPC
**                      RunTime Extensions Specification Version OSF TX1.0.9
**                      pre 1003 for details), of the remote machine.
**
**  INPUTS/OUTPUTS:
**
**      pkt_p           A pointer to the entire packet.
**
**  OUTPUTS:            none
**
**  IMPLICIT INPUTS:    none
**
**  IMPLICIT OUTPUTS:   none
**
**  FUNCTION VALUE:     none
**
**  SIDE EFFECTS:       none
**
**--
**/

INTERNAL void rpc__gssauth_cn_tlr_unpack
(
	rpc_cn_packet_p_t	pkt_p ATTRIBUTE_UNUSED /*TODO*/,
	unsigned32		auth_value_len ATTRIBUTE_UNUSED /*TODO*/,
	unsigned8		*packed_drep ATTRIBUTE_UNUSED /*TODO*/
)
{
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_cn_tlr_unpack)\n"));
}

/*****************************************************************************/
/*
**++
**
**  ROUTINE NAME:       rpc__gssauth_cn_vfy_client_req
**
**  SCOPE:              INTERNAL - declared locally
**
**  DESCRIPTION:
**
**      This routine will decode the auth_value field of
**      either an rpc_bind or rpc_alter_context PDU. Any
**      error encountered while authenticating the client
**      will result in an error status return. The contents
**      of the credentials field includes the authorization
**      data. This is called from the server side association
**      state machine. Note that upon successful return
**      the auth information rep will contain the client's
**      authorization protocol and data.
**
**  INPUTS:
**
**      assoc_sec       A pointer to per-association security context
**                      including association UUID CRC and sequence numbers.
**      sec             A pointer to security context element which includes
**                      the key ID, auth information rep and RPC auth
**                      information rep.
**      auth_value      A pointer to the auth_value field in the rpc_bind or
**                      rpc_alter_context PDU authentication trailer.
**      auth_value_len  The length, in bytes, of auth_value.
**
**  INPUTS/OUTPUTS:     none
**
**  OUTPUTS:
**
**      st              The return status of this routine.
**
**  IMPLICIT INPUTS:    none
**
**  IMPLICIT OUTPUTS:   none
**
**  FUNCTION VALUE:     none
**
**  SIDE EFFECTS:       none
**
**--
**/

INTERNAL void rpc__gssauth_cn_vfy_client_req
(
	rpc_cn_assoc_sec_context_p_t	assoc_sec,
	rpc_cn_sec_context_p_t		sec,
	pointer_t			auth_value,
	unsigned32			auth_value_len,
	unsigned32			old_client ATTRIBUTE_UNUSED /*TODO*/,
	unsigned32			*st
)
{
	rpc_gssauth_cn_info_p_t gssauth_cn_info = (rpc_gssauth_cn_info_p_t)sec->sec_cn_info;
	int maj_stat;
	OM_uint32 min_stat = 0;
	gss_buffer_desc input_token, output_token = GSS_C_EMPTY_BUFFER;

	CODING_ERROR(st);
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_cn_vfy_client_req)\n"));

	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_PKT,
		("(rpc__gssauth_cn_vfy_client_req) prot->%x level->%x key_id->%x assoc_uuid_crc->%x xmit_seq->%x recv_seq->%x\n",
		sec->sec_info->authn_protocol,
		sec->sec_info->authn_level,
		sec->sec_key_id,
		assoc_sec->assoc_uuid_crc,
		assoc_sec->assoc_next_snd_seq,
		assoc_sec->assoc_next_rcv_seq));

#ifdef DEBUG
	if (RPC_DBG_EXACT(rpc_es_dbg_cn_errors, RPC_C_CN_DBG_AUTH_VFY_CLIENT_REQ)) {
		*st = RPC_S_CN_DBG_AUTH_FAILURE;
		return;
	}
#endif

	input_token.value = auth_value;
	input_token.length = auth_value_len;
	maj_stat = gss_accept_sec_context(&min_stat,
					  &gssauth_cn_info->gss_ctx,
					  NULL,
					  &input_token,
					  NULL,
					  NULL,
					  NULL,
					  &output_token,
					  NULL,
					  NULL,
					  NULL);
        gssauth_cn_info->gss_stat = maj_stat;
        if (maj_stat == GSS_S_CONTINUE_NEEDED) {
                char msg[BUFSIZ];
                rpc__gssauth_error_map(maj_stat, min_stat,
                                       (gss_OID)&rpc__gssauth_krb5_oid,
                                       msg, sizeof(msg), st);
                RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_GENERAL,
                        ("(rpc__gssauth_cn_vfy_client_req): %s: output_token.length[%u]\n",
                        msg, (unsigned int)output_token.length));
                /*
                 * we still transfer the buffer to the client
                 * but fail the auth in rpc__gssauth_cn_fmt_srvr_resp()
                 */
        } else if (maj_stat != GSS_S_COMPLETE) {
                char msg[BUFSIZ];
                rpc__gssauth_error_map(maj_stat, min_stat,
                                       (gss_OID)&rpc__gssauth_krb5_oid,
                                       msg, sizeof(msg), st);
                RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_GENERAL,
                        ("(rpc__gssauth_cn_vfy_client_req): %s\n", msg));
		return;
        } else {
                *st = rpc_s_ok;
        }

	assoc_sec->krb_message.length = output_token.length;
	assoc_sec->krb_message.data = output_token.value;
}

/*****************************************************************************/
/*
**++
**
**  ROUTINE NAME:       rpc__gssauth_cn_vfy_srvr_resp
**
**  SCOPE:              INTERNAL - declared locally
**
**  DESCRIPTION:
**
**      This routine will decode auth_value field either an
**      rpc_bind_ack or rpc_alter_context_response PDU. If the
**      credentials field of the auth_value field contains an
**      authentication protocol specific encoding of an error
**      this will be returned as an error status code. This is
**      called from the client side association state machine.
**      Note that upon successful return the auth information
**      rep will contain the client's authorization protocol
**      and data.
**
**  INPUTS:
**
**      assoc_sec       A pointer to per-association security context
**                      including association UUID CRC and sequence numbers.
**      sec             A pointer to security context element which includes
**                      the key ID, auth information rep and RPC auth
**                      information rep.
**      auth_value      A pointer to the auth_value field in the rpc_bind or
**                      rpc_alter_context PDU authentication trailer.
**      auth_value_len  The length, in bytes, of auth_value.
**
**  INPUTS/OUTPUTS:     none
**
**  OUTPUTS:
**
**      st              The return status of this routine.
**
**  IMPLICIT INPUTS:    none
**
**  IMPLICIT OUTPUTS:   none
**
**  FUNCTION VALUE:     none
**
**  SIDE EFFECTS:       none
**
**--
**/

INTERNAL void rpc__gssauth_cn_vfy_srvr_resp
(
	rpc_cn_assoc_sec_context_p_t	assoc_sec,
	rpc_cn_sec_context_p_t		sec,
	pointer_t			auth_value,
	unsigned32			auth_value_len,
	unsigned32			*st
)
{
	rpc_gssauth_info_p_t gssauth_info = (rpc_gssauth_info_p_t)sec->sec_info;
	rpc_gssauth_cn_info_p_t gssauth_cn_info = (rpc_gssauth_cn_info_p_t)sec->sec_cn_info;
	OM_uint32 maj_stat;
	OM_uint32 min_stat;
	gss_buffer_desc input_token, output_token = GSS_C_EMPTY_BUFFER;

	CODING_ERROR(st);
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_cn_vfy_srvr_resp)\n"));

	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_PKT,
		("(rpc__gssauth_cn_vfy_server_resp) prot->%x level->%x key_id->%x assoc_uuid_crc->%x xmit_seq->%x recv_seq->%x\n",
		sec->sec_info->authn_protocol,
		sec->sec_info->authn_level,
		sec->sec_key_id,
		assoc_sec->assoc_uuid_crc,
		assoc_sec->assoc_next_snd_seq,
		assoc_sec->assoc_next_rcv_seq));

#ifdef DEBUG
	if (RPC_DBG_EXACT(rpc_es_dbg_cn_errors, RPC_C_CN_DBG_AUTH_VFY_SERVER_RESP)) {
		*st = RPC_S_CN_DBG_AUTH_FAILURE;
		return;
	}
#endif

	if (gssauth_cn_info->gss_ctx != GSS_C_NO_CONTEXT &&
	    gssauth_cn_info->gss_stat == GSS_S_COMPLETE) {
		/* todo only when ready */
		goto done;
	}

	input_token.value = auth_value;
	input_token.length = auth_value_len;
	maj_stat = rpc__gssauth_verify_server_token(&min_stat,
						    sec,
						    gssauth_info->gss_creds,
						    gssauth_info->gss_server_name,
						    gssauth_cn_info->gss_ctx,
						    input_token,
						    &output_token);
	gssauth_cn_info->gss_stat = maj_stat;
	if (maj_stat == GSS_S_CONTINUE_NEEDED) {
		char msg[BUFSIZ];
		rpc__gssauth_error_map(maj_stat, min_stat,
				       (gss_OID)&rpc__gssauth_krb5_oid,
				       msg, sizeof(msg), st);
		RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_GENERAL,
			("(rpc__gssauth_cn_vfy_srvr_resp): %s: output_token.length[%u]\n",
			msg, (unsigned int)output_token.length));
		/*
		 * we still transfer the buffer to the client
		 * but fail the auth in rpc__gssauth_cn_fmt_srvr_resp()
		 */
	} else if (maj_stat != GSS_S_COMPLETE) {
		char msg[BUFSIZ];
		rpc__gssauth_error_map(maj_stat, min_stat,
				       (gss_OID)&rpc__gssauth_krb5_oid,
				       msg, sizeof(msg), st);
		RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_GENERAL,
			("(rpc__gssauth_cn_vfy_srvr_resp): %s\n", msg));
		return;
	}

done:
	assoc_sec->krb_message.length = output_token.length;
	assoc_sec->krb_message.data = output_token.value;

	*st = rpc_s_ok;
}

PRIVATE rpc_protocol_id_t rpc__gssauth_negotiate_cn_init
(
	rpc_auth_rpc_prot_epv_p_t	*epv,
	unsigned32			*st
)
{
	CODING_ERROR(st);
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_negotiate_cn_init)\n"));

	*epv = (rpc_auth_rpc_prot_epv_p_t)&rpc_g_gssauth_negotiate_cn_epv;
	*st = rpc_s_ok;
	return RPC_C_PROTOCOL_ID_NCACN;
}

PRIVATE rpc_protocol_id_t rpc__gssauth_mskrb_cn_init
(
	rpc_auth_rpc_prot_epv_p_t	*epv,
	unsigned32			*st
)
{
	CODING_ERROR(st);
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_mskrb_cn_init)\n"));

	*epv = (rpc_auth_rpc_prot_epv_p_t)&rpc_g_gssauth_mskrb_cn_epv;
	*st = rpc_s_ok;
	return RPC_C_PROTOCOL_ID_NCACN;
}

PRIVATE rpc_protocol_id_t rpc__gssauth_winnt_cn_init
(
	rpc_auth_rpc_prot_epv_p_t	*epv,
	unsigned32			*st
)
{
	CODING_ERROR(st);
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_winnt_cn_init)\n"));

	*epv = (rpc_auth_rpc_prot_epv_p_t)&rpc_g_gssauth_winnt_cn_epv;
	*st = rpc_s_ok;
	return RPC_C_PROTOCOL_ID_NCACN;
}

PRIVATE rpc_protocol_id_t rpc__gssauth_netlogon_cn_init
(
	rpc_auth_rpc_prot_epv_p_t	*epv,
	unsigned32			*st
)
{
	CODING_ERROR(st);
	RPC_DBG_PRINTF(rpc_e_dbg_auth, RPC_C_CN_DBG_AUTH_ROUTINE_TRACE,
		("(rpc__gssauth_netlogon_cn_init)\n"));

	*epv = (rpc_auth_rpc_prot_epv_p_t)&rpc_g_gssauth_netlogon_cn_epv;
	*st = rpc_s_ok;
	return RPC_C_PROTOCOL_ID_NCACN;
}
