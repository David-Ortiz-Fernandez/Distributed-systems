/* soapClient.c
   Generated by gSOAP 2.8.35 for conecta4.h

gSOAP XML Web services tools
Copyright (C) 2000-2016, Robert van Engelen, Genivia Inc. All Rights Reserved.
The soapcpp2 tool and its generated software are released under the GPL.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
--------------------------------------------------------------------------------
A commercial use license is available from Genivia Inc., contact@genivia.com
--------------------------------------------------------------------------------
*/

#if defined(__BORLANDC__)
#pragma option push -w-8060
#pragma option push -w-8004
#endif
#include "soapH.h"
#ifdef __cplusplus
extern "C" {
#endif

SOAP_SOURCE_STAMP("@(#) soapClient.c ver 2.8.35 2016-11-29 09:10:30 GMT")


SOAP_FMAC5 int SOAP_FMAC6 soap_call_conecta4ns__register(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct tMessage playerName, int *code)
{	struct conecta4ns__register soap_tmp_conecta4ns__register;
	struct conecta4ns__registerResponse *soap_tmp_conecta4ns__registerResponse;
	if (soap_endpoint == NULL)
		soap_endpoint = "http//localhost:10000";
	soap_tmp_conecta4ns__register.playerName = playerName;
	soap_begin(soap);
	soap->encodingStyle = "";
	soap_serializeheader(soap);
	soap_serialize_conecta4ns__register(soap, &soap_tmp_conecta4ns__register);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_conecta4ns__register(soap, &soap_tmp_conecta4ns__register, "conecta4ns:register", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_conecta4ns__register(soap, &soap_tmp_conecta4ns__register, "conecta4ns:register", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!code)
		return soap_closesock(soap);
	soap_default_int(soap, code);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	if (soap_recv_fault(soap, 1))
		return soap->error;
	soap_tmp_conecta4ns__registerResponse = soap_get_conecta4ns__registerResponse(soap, NULL, "", NULL);
	if (!soap_tmp_conecta4ns__registerResponse || soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	if (code && soap_tmp_conecta4ns__registerResponse->code)
		*code = *soap_tmp_conecta4ns__registerResponse->code;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_call_conecta4ns__getStatus(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct tMessage playerName, struct tBlock *status)
{	struct conecta4ns__getStatus soap_tmp_conecta4ns__getStatus;
	struct conecta4ns__getStatusResponse *soap_tmp_conecta4ns__getStatusResponse;
	if (soap_endpoint == NULL)
		soap_endpoint = "http//localhost:10000";
	soap_tmp_conecta4ns__getStatus.playerName = playerName;
	soap_begin(soap);
	soap->encodingStyle = "";
	soap_serializeheader(soap);
	soap_serialize_conecta4ns__getStatus(soap, &soap_tmp_conecta4ns__getStatus);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_conecta4ns__getStatus(soap, &soap_tmp_conecta4ns__getStatus, "conecta4ns:getStatus", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_conecta4ns__getStatus(soap, &soap_tmp_conecta4ns__getStatus, "conecta4ns:getStatus", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!status)
		return soap_closesock(soap);
	soap_default_conecta4ns__tBlock(soap, status);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	if (soap_recv_fault(soap, 1))
		return soap->error;
	soap_tmp_conecta4ns__getStatusResponse = soap_get_conecta4ns__getStatusResponse(soap, NULL, "", NULL);
	if (!soap_tmp_conecta4ns__getStatusResponse || soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	if (status && soap_tmp_conecta4ns__getStatusResponse->status)
		*status = *soap_tmp_conecta4ns__getStatusResponse->status;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_call_conecta4ns__insertChip(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct tMessage playerName, int playerMove, struct tBlock *status)
{	struct conecta4ns__insertChip soap_tmp_conecta4ns__insertChip;
	struct conecta4ns__insertChipResponse *soap_tmp_conecta4ns__insertChipResponse;
	if (soap_endpoint == NULL)
		soap_endpoint = "http//localhost:10000";
	soap_tmp_conecta4ns__insertChip.playerName = playerName;
	soap_tmp_conecta4ns__insertChip.playerMove = playerMove;
	soap_begin(soap);
	soap->encodingStyle = "";
	soap_serializeheader(soap);
	soap_serialize_conecta4ns__insertChip(soap, &soap_tmp_conecta4ns__insertChip);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_conecta4ns__insertChip(soap, &soap_tmp_conecta4ns__insertChip, "conecta4ns:insertChip", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_conecta4ns__insertChip(soap, &soap_tmp_conecta4ns__insertChip, "conecta4ns:insertChip", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!status)
		return soap_closesock(soap);
	soap_default_conecta4ns__tBlock(soap, status);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	if (soap_recv_fault(soap, 1))
		return soap->error;
	soap_tmp_conecta4ns__insertChipResponse = soap_get_conecta4ns__insertChipResponse(soap, NULL, "", NULL);
	if (!soap_tmp_conecta4ns__insertChipResponse || soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	if (status && soap_tmp_conecta4ns__insertChipResponse->status)
		*status = *soap_tmp_conecta4ns__insertChipResponse->status;
	return soap_closesock(soap);
}

#ifdef __cplusplus
}
#endif

#if defined(__BORLANDC__)
#pragma option pop
#pragma option pop
#endif

/* End of soapClient.c */
