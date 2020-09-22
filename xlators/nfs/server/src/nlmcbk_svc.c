/*
  Copyright (c) 2012 Gluster, Inc. <http://www.gluster.com>
  This file is part of GlusterFS.

  This file is licensed to you under your choice of the GNU Lesser
  General Public License, version 3 or any later version (LGPLv3 or
  later), or the GNU General Public License, version 2 (GPLv2), in all
  cases as published by the Free Software Foundation.
*/

/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "nlm4.h"
#include <glusterfs/logging.h>
#include "nfs-messages.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef SIG_PF
#define SIG_PF void (*)(int)
#endif

void
nlm4svc_sm_notify(struct nlm_sm_status *status);

void *
nlmcbk_sm_notify_0_svc(struct nlm_sm_status *status, struct svc_req *req)
{
    nlm4svc_sm_notify(status);
    return NULL;
}

static void
nlmcbk_program_0(struct svc_req *rqstp, register SVCXPRT *transp)
{
    union {
        struct nlm_sm_status nlmcbk_sm_notify_0_arg;
    } argument;
    char *result;
    xdrproc_t _xdr_argument, _xdr_result;
    char *(*local)(char *, struct svc_req *);

    switch (rqstp->rq_proc) {
        case NULLPROC:
            (void)svc_sendreply(transp, (xdrproc_t)xdr_void, (char *)NULL);
            return;

        case NLMCBK_SM_NOTIFY:
            _xdr_argument = (xdrproc_t)xdr_nlm_sm_status;
            _xdr_result = (xdrproc_t)xdr_void;
            local = (char *(*)(char *, struct svc_req *))nlmcbk_sm_notify_0_svc;
            break;

        default:
            svcerr_noproc(transp);
            return;
    }
    memset((char *)&argument, 0, sizeof(argument));
    if (!svc_getargs(transp, (xdrproc_t)_xdr_argument, (caddr_t)&argument)) {
        svcerr_decode(transp);
        return;
    }
    result = (*local)((char *)&argument, rqstp);
    if (!svc_sendreply(transp, (xdrproc_t)_xdr_result, result)) {
        svcerr_systemerr(transp);
    }

    if (!svc_freeargs(transp, (xdrproc_t)_xdr_argument, (caddr_t)&argument)) {
        gf_msg(GF_NLM, GF_LOG_ERROR, 0, NFS_MSG_ARG_FREE_FAIL,
               "unable to free arguments");
        return;
    }
    return;
}

void *
nsm_thread(void *argv)
{
    xlator_t *nfsx = argv;
    register SVCXPRT *transp;
    int ret = 0;

    GF_ASSERT(nfsx);

    THIS = nfsx;

    ret = pmap_unset(NLMCBK_PROGRAM, NLMCBK_V1);
    if (ret == 0) {
        gf_msg(GF_NLM, GF_LOG_ERROR, 0, NFS_MSG_PMAP_UNSET_FAIL,
               "pmap_unset failed");
        return NULL;
    }
    transp = svcudp_create(RPC_ANYSOCK);
    if (transp == NULL) {
        gf_msg(GF_NLM, GF_LOG_ERROR, errno, NFS_MSG_UDP_SERV_FAIL,
               "cannot create udp service.");
        return NULL;
    }
    if (!svc_register(transp, NLMCBK_PROGRAM, NLMCBK_V1, nlmcbk_program_0,
                      IPPROTO_UDP)) {
        gf_msg(GF_NLM, GF_LOG_ERROR, 0, NFS_MSG_REG_NLMCBK_FAIL,
               "unable to register (NLMCBK_PROGRAM, "
               "NLMCBK_V0, udp).");
        return NULL;
    }

    transp = svctcp_create(RPC_ANYSOCK, 0, 0);
    if (transp == NULL) {
        gf_msg(GF_NLM, GF_LOG_ERROR, errno, NFS_MSG_TCP_SERV_FAIL,
               "cannot create tcp service.");
        return NULL;
    }
    if (!svc_register(transp, NLMCBK_PROGRAM, NLMCBK_V1, nlmcbk_program_0,
                      IPPROTO_TCP)) {
        gf_msg(GF_NLM, GF_LOG_ERROR, 0, NFS_MSG_REG_NLMCBK_FAIL,
               "unable to register (NLMCBK_PROGRAM, "
               "NLMCBK_V0, tcp).");
        return NULL;
    }

    svc_run();
    gf_msg(GF_NLM, GF_LOG_ERROR, 0, NFS_MSG_SVC_RUN_RETURNED,
           "svc_run returned");
    return NULL;
    /* NOTREACHED */
}
