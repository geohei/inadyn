/* Plugin for DNS Stock
 *
 * Copyright (C) 2016  Georges Heinesch <georges@geohei.lu>
 * Copyright (C) 2010-2014  Joachim Nilsson <troglobit@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, visit the Free Software Foundation
 * website at http://www.gnu.org/licenses/gpl-2.0.html or write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#include "plugin.h"

/* dns-stock.com specific update request format */
#define DNSSTOCK_UPDATE_IP_REQUEST					\
	"GET %s"							\
	"hostname=%s&"							\
	"password=%s&"							\
	"ipaddr=%s&"							\
	"updatetimeout=0 "						\
	"HTTP/1.0\r\n"							\
	"Host: %s\r\n"							\
	"User-Agent: " AGENT_NAME " " SUPPORT_ADDR "\r\n\r\n"

static int request  (dnsstock_t   *ctx,   dnsstock_info_t *info, dnsstock_alias_t *alias);
static int response (http_trans_t *trans, dnsstock_info_t *info, dnsstock_alias_t *alias);

static dnsstock_system_t plugin = {
	.name         = "default@dnsstock.com",

	.request      = (req_fn_t)request,
	.response     = (rsp_fn_t)response,

	.checkip_name = DYNDNS_MY_IP_SERVER,
	.checkip_url  = DYNDNS_MY_CHECKIP_URL,

	.server_name  = "www.dns-stock.com",
	.server_url   =  "/?"
};

static int request(dnsstock_t *ctx, dnsstock_info_t *info, dnsstock_alias_t *alias)
{
	return snprintf(ctx->request_buf, ctx->request_buflen,
			DNSSTOCK_UPDATE_IP_REQUEST,
			info->server_url,
			alias->name,
			info->creds.password,
			alias->address,
			info->server_name.name);
}

static int response(http_trans_t *trans, dnsstock_info_t *UNUSED(info), dnsstock_alias_t *alias)
{
	char *rsp = trans->p_rsp_body;

	DO(http_status_valid(trans->status));

	if (strstr(rsp, alias->address))
		return 0;

	return RC_DYNDNS_RSP_NOTOK;
}

PLUGIN_INIT(plugin_init)
{
	plugin_register(&plugin);
}

PLUGIN_EXIT(dnsstock_exit)
{
	dnsstock_unregister(&plugin);
}

/**
 * Local Variables:
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
