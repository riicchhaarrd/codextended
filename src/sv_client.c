/*
	This file is part of CoDExtended.

    CoDExtended is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CoDExtended is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CoDExtended.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "server.h"
#include <sys/time.h>

char **configstrings = (char**)0x8355678;

void (*MSG_Init)( msg_t *buf, byte *data, int length ) = (void (*)(msg_t*,byte*,int))0x807EEB8;
void (*MSG_WriteLong)(msg_t*,int) = (void(*)(msg_t*,int))0x807F0EC;
void (*MSG_WriteString)(msg_t*,const char*) = (void(*)(msg_t*,const char*))0x807A620;
void (*MSG_WriteByte)(msg_t*,int) = (void(*)(msg_t*,int))0x807F090;
void (*MSG_WriteShort)(msg_t*,int) = (void(*)(msg_t*,int))0x807F0BC;
void (*MSG_WriteBigString)(msg_t*,const char*) = (void(*)(msg_t*,const char*))0x807A758;
void (*SV_SendMessageToClient)(msg_t*,client_t*) = (void(*)(msg_t*,client_t*))0x808F680;
void (*SV_SendClientGameState)(client_t*) = (void(*)(client_t*))0x8085EEC;

int clientversion = 0;

x_client x_clients[64];
x_challenge x_challenges[MAX_CHALLENGES] = {0};

long long current_timestamp() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000;
    return milliseconds;
}

typedef struct {
	char        *name;
	void ( *func )( client_t *cl );
} ucmd_t;

//static ucmd_t* ucmds = (ucmd_t*)0x80E2F4C;

void SV_BeginDownload(client_t*);
void SV_CoDExtended_f(client_t*);

#ifdef xDEBUG
void sv_sprint( client_t *cl ) {
	int num = get_client_number( cl );
	xentities[ num ].sprinting ^= 1;
	
	
}
#endif

void ucmd_ascii( client_t *cl ) {
	
	#if 0
	#define MAX_CURSOR 50
	int i,j,cursor;

	cursor = 0;
	int prev = 0;
	
	char send[4096] = {0};
	char str[MAX_CURSOR] = {0};
	for(i = 0; i < 40; i++) {
		for(j = 0; j < MAX_CURSOR; j++) {
			if( (rand() % 2) == 1 && prev != '^')
			prev = str[j] = '^';
			else
			str[j] = '0' + ( rand() % ('Z' - '0') );
			cursor = (cursor + 1) % MAX_CURSOR;
		}
		str[j] = '\0';

		strcat(send, str);
		strcat(send, "\n");
	}
	/*
	for(char *i = msg; *i != '\0'; *i++)
		if(*i == '\'')
			*i = '"';
	*/
	NET_OutOfBandPrint(NS_SERVER, cl->netchan.remoteAddress, "print\n%s", send);
	#endif
}

void SV_DoneDownload_f(client_t*);
void SV_NextDownload_f(client_t *cl);

static ucmd_t ucmds[] = {
	{"userinfo", (void*)0x8087B28},
	{"disconnect", (void*)0x8087AF8},
	{"cp", (void*)0x808674C},//SV_VerifyPaks_f},
	{"vdr", (void*)0x8087B14},
	{"download", SV_BeginDownload},
	{"nextdl", (void*)0x8086168},
	{"stopdl", (void*)0x8087960},
	{"donedl", (void*)0x80879FC},
	{"retransdl", (void*)0x8087A2C},
	
	{"codextended", SV_CoDExtended_f},
	#ifdef xDEBUG
	{"asc", ucmd_ascii},
	{"sprint", sv_sprint},
	#endif
	{NULL, NULL}
};

client_t **clients = (client_t**)svsclients_ptr;

typedef void (*SV_SendClientSnapshot_t)(client_t*);

SV_SendClientSnapshot_t SV_SendClientSnapshot = (SV_SendClientSnapshot_t)0x808F844;

SV_StopDownload_f_t SV_StopDownload_f = (SV_StopDownload_f_t)0x8087960;
SV_BeginDownload_f_t SV_BeginDownload_f = (SV_BeginDownload_f_t)0x8087A64;
getuserinfo_t getuserinfo = (getuserinfo_t)0x808B25C;
setuserinfo_t setuserinfo = (setuserinfo_t)0x808B1D0;
SV_DropClient_t SV_DropClient = (SV_DropClient_t)0x8085CF4;

void (*SV_FreeClient_o)(client_t*) = (void(*)(client_t*))0x808D34C;

void SV_FreeClient(client_t *cl) {
	/*void (*free_script_stuff)(unsigned short) = (void(*)(unsigned short))0x80A3BE4;
	free_script_stuff(cl->scriptId);
	cl->scriptId = 0;
	*/
	
	SV_FreeClient_o(cl);
}

SV_FreeClientScriptId_t SV_FreeClientScriptId = (SV_FreeClientScriptId_t)0x808D34C;

challenge_t *challenges = (challenge_t*)0x83B67F8;

char	*ConcatArgs( int start ) {
	int		i, c, tlen;
	static char	line[MAX_STRING_CHARS];
	int		len;
	char	arg[MAX_STRING_CHARS];

	len = 0;
	c = Cmd_Argc();
	for ( i = start ; i < c ; i++ ) {
		Cmd_ArgvBuffer( i, arg, sizeof( arg ) );
		tlen = strlen( arg );
		//if ( len + tlen >= MAX_STRING_CHARS - 1 ) {
		if ( len + tlen >= 894 ) {
			break;
		}
		memcpy( line + len, arg, tlen );
		len += tlen;
		if ( i != c - 1 ) {
			line[len] = ' ';
			len++;
		}
	}

	line[len] = 0;

	return line;
}

void SanitizeString( char *in, char *out ) {
	while ( *in ) {
		if ( *in == 27 ) {
			in += 2;		// skip color code
			continue;
		}
		if ( *in < 32 ) {
			in++;
			continue;
		}
		*out++ = tolower( *in++ );
	}

	*out = 0;
}

bool is_good_string(char* str) {
	int i;
	for(i = 0; i < strlen(str); i++)
		if(str[i] < 32 || str[i] > 126)
			return 0;
	return 1;
}

void SV_XAuthorize(netadr_t from) {
	int challenge;
	int i;
	
	if(!NET_CompareBaseAdr(from, x_master))
		return; //not from xtnded master
	
	challenge = atoi(Cmd_Argv(1));
	
	for(i = 0; i < MAX_CHALLENGES; i++) {
		if(challenges[i].challenge == challenge)
			break;
	}
	
	if(i == MAX_CHALLENGES) //not found
		return;
		
	x_challenges[i].bAuthRequested = false;
		
	int allowed = atoi(Cmd_Argv(2));
	char *mUID = Cmd_Argv(3);
	
	if(allowed == 0) {
		x_challenges[i].bCanConnect = -1;
		return;
	}
	
	if(!*mUID)
		return;
	
	cprintf(PRINT_UNDERLINE | PRINT_GOOD, "mUID: %s\n", mUID);
	
	Q_strncpyz(x_challenges[i].mUID, mUID, sizeof(x_challenges[i].mUID));
	x_challenges[i].bCanConnect = 1;
}

void SV_AuthorizeIpPacket( netadr_t from ) {
	return; //auth srv nty
	
	int challenge;
	int i;
	char    *s;
	char    *r;
	int guid;
	char ret[1024];
	
	if ( !NET_CompareBaseAdr( from, authorizeAddress ) ) {
		Com_Printf( "SV_AuthorizeIpPacket: not from authorize server\n" );
		return;
	}

	challenge = atoi( Cmd_Argv( 1 ) );

	for ( i = 0 ; i < MAX_CHALLENGES ; i++ ) {
		if ( challenges[i].challenge == challenge ) {
			break;
		}
	}
	
	if ( i == MAX_CHALLENGES ) {
		Com_Printf( "SV_AuthorizeIpPacket: challenge not found\n" );
		return;
	}

	// send a packet back to the original client
	challenges[i].pingTime = svs_time;
	s = Cmd_Argv( 2 );
	r = Cmd_Argv( 3 );          // reason
	guid = atoi(Cmd_Argv(4));
	
	if ( !Q_stricmp( s, "demo" ) ) {
		/*if ( Cvar_VariableValue( "fs_restrict" ) ) {
			// a demo client connecting to a demo server
			NET_OutOfBandPrint( NS_SERVER, from,
								"challengeResponse %i", challenges[i].challenge );
			return;
		}*/
		// they are a demo client trying to connect to a real server
		NET_OutOfBandPrint( NS_SERVER, challenges[i].adr, "print\nEXE_ERR_NOT_A_DEMO_SERVER" );
		// clear the challenge record so it won't timeout and let them through
		clearchallenge(i);
		return;
	}
	if ( !Q_stricmp( s, "accept" ) || !x_authorize->integer) {
		x_challenges[i].guid = guid;
		
		if ( sv_onlyVisibleClients->integer ) {
			NET_OutOfBandPrint( NS_SERVER, challenges[i].adr,
								"challengeResponse %i %i", challenges[i].challenge, sv_onlyVisibleClients->integer );
		} else {
			NET_OutOfBandPrint( NS_SERVER, challenges[i].adr,
								"challengeResponse %i", challenges[i].challenge );
		}
		return;
	}
	
	if(!Q_stricmp(s, "deny")) {
		if(!strcmp(r, "CLIENT_UNKNOWN_TO_AUTH") || !strcmp(r, "BAD_CDKEY"))
			NET_OutOfBandPrint( NS_SERVER, challenges[i].adr, "needcdkey" );
		else
			NET_OutOfBandPrint( NS_SERVER, challenges[i].adr, "error\nCould not authorize with the authorize server.");
	}
	
	// clear the challenge record so it won't timeout and let them through
	clearchallenge(i);
}

static time_t connect_t = 0;

void repeat_annoy(client_t *cl) {
	byte msg_buf[16384];
	msg_t msg;
	
	MSG_Init( &msg, msg_buf, sizeof( msg_buf ) );
	
	MSG_WriteLong( &msg, cl->lastClientCommand );
	
	MSG_WriteByte(&msg,svc_serverCommand);
	//MSG_WriteLong(&msg, *(int*)((int)cl + 67088) + 1);
	MSG_WriteLong(&msg, 1);
	//SV_SendServerCommand(cl, 0, "hello aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	
	MSG_WriteString(&msg,"v \"cl_allowdownload\" \"1\"");
	MSG_WriteByte(&msg,svc_EOF); //end?
	//check for overflow i doubt it tho
	
	SV_SendMessageToClient(&msg, cl);
}

void SV_SendDownloadDone(client_t *cl) { //let's fake that the downloads are done so it'll reload the file system
	byte msg_buf[16384];
	msg_t msg;
	
	MSG_Init( &msg, msg_buf, sizeof( msg_buf ) );
	
	MSG_WriteLong( &msg, cl->lastClientCommand );
	
	MSG_WriteByte(&msg,svc_configstring);
	
	char systeminfo[BIG_INFO_STRING];
	strcpy(systeminfo, configstrings[1]);
	char tmp[1024];
	strcpy(tmp, Info_ValueForKey(systeminfo, "sv_paks"));
	
	char *i;
	for(i = tmp; *i && *i != ' '; i++);
	*i = 0;
	Info_SetValueForKey(systeminfo, "sv_paks", tmp);
	strcpy(tmp, Info_ValueForKey(systeminfo, "sv_pakNames"));
	for(i = tmp; *i && *i != ' '; i++);
	*i = 0;
	Info_SetValueForKey(systeminfo, "sv_pakNames", tmp);
	
	//MSG_WriteLong(&msg, *(int*)((int)cl + 67088) + 1);
	MSG_WriteShort(&msg, 1);
	MSG_WriteBigString(&msg, systeminfo);
	MSG_WriteByte(&msg,svc_EOF); //end?
	
	SV_SendMessageToClient(&msg, cl);
}

client_t *last_cl = NULL;

void Info_RemoveKey_Big( char *s, const char *key ) {
	char    *start;
	char pkey[BIG_INFO_KEY];
	char value[BIG_INFO_VALUE];
	char    *o;

	if ( strlen( s ) >= BIG_INFO_STRING ) {
		Com_Error( ERR_DROP, "Info_RemoveKey_Big: oversize infostring [%s] [%s]", s, key );
	}

	if ( strchr( key, '\\' ) ) {
		return;
	}

	while ( 1 )
	{
		start = s;
		if ( *s == '\\' ) {
			s++;
		}
		o = pkey;
		while ( *s != '\\' )
		{
			if ( !*s ) {
				return;
			}
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value;
		while ( *s != '\\' && *s )
		{
			if ( !*s ) {
				return;
			}
			*o++ = *s++;
		}
		*o = 0;

		if ( !Q_stricmp( key, pkey ) ) {
			strcpy( start, s );  // remove this part
			return;
		}

		if ( !*s ) {
			return;
		}
	}

}

void Info_SetValueForKey_Big( char *s, const char *key, const char *value ) {
	char newi[BIG_INFO_STRING];

	if ( strlen( s ) >= BIG_INFO_STRING ) {
		Com_Error( ERR_DROP, "Info_SetValueForKey: oversize infostring [%s] [%s] [%s]", s, key, value );
	}

	if ( strchr( key, '\\' ) || strchr( value, '\\' ) ) {
		Com_Printf( "Can't use keys or values with a \\\n" );
		return;
	}

	if ( strchr( key, ';' ) || strchr( value, ';' ) ) {
		Com_Printf( "Can't use keys or values with a semicolon\n" );
		return;
	}

	if ( strchr( key, '\"' ) || strchr( value, '\"' ) ) {
		Com_Printf( "Can't use keys or values with a \"\n" );
		return;
	}

	Info_RemoveKey_Big( s, key );
	if ( !value || !strlen( value ) ) {
		return;
	}

	Com_sprintf( newi, sizeof( newi ), "\\%s\\%s", key, value );

	if ( strlen( newi ) + strlen( s ) > BIG_INFO_STRING ) {
		Com_Printf( "BIG Info string length exceeded\n" );
		return;
	}

	strcat( s, newi );
}

void custom_SV_ExecuteClientMessage(client_t *cl, msg_t *msg)
{
		int* cl_serverId = (int*)((int)cl + 370936);
	int* sv_serverId = (int*)0x80e30c0;
	int* cl_messageAcknowledge = (int*)((int)cl + 67096);

	int (*MSG_ReadBitsCompress)(msg_t*, byte*, int) = (int(*)(msg_t*, byte*, int))0x807F23C;
	int (*MSG_ReadBits)(msg_t*, int) = (int(*)(msg_t*, int))0x807F18C;
	void (*SV_UserMove)(client_t*, msg_t*, qboolean) = (void(*)(client_t*, msg_t*, qboolean))0x8086fa4;

	byte msgBuf[16384];
	msg_t decompressMsg;
	int c;

	MSG_Init(&decompressMsg, msgBuf, sizeof(msgBuf));
	decompressMsg.cursize = MSG_ReadBitsCompress(&msg->data[msg->readcount], msgBuf, msg->cursize - msg->readcount);

	if (*cl_serverId == *sv_serverId || cl->downloadName[0])
	{
		while (1)
		{
			c = MSG_ReadBits(&decompressMsg, 2);
			if (c != 2)
			{
				break;
			}
			if (!SV_ClientCommand(cl, &decompressMsg) || cl->state == CS_ZOMBIE)
			{
				return;
			}
		}

		if (sv_pure->integer && cl->pureAuthentic == 2)
		{
			cl->nextSnapshotTime = -1;
			SV_DropClient(cl, "EXE_UNPURECLIENTDETECTED");
			cl->state = CS_ACTIVE;
			SV_SendClientSnapshot(cl);
			cl->state = CS_ZOMBIE;
		}

		if (c)
		{
			if (c == 1)
			{
				SV_UserMove(cl, &decompressMsg, 0);
			}
			else if (c != 3)
			{
				Com_DPrintf("WARNING: bad command byte %i for client %i\n", c, get_client_number(cl));
			}
		}
		else
		{
			SV_UserMove(cl, &decompressMsg, 1);
		}
	}
	else if ((*cl_serverId & 0xF0) == (*sv_serverId & 0xF0))
	{
		if ((*cl_messageAcknowledge > cl->gamestateMessageNum) && cl->state == CS_PRIMED)
		{
			cprintf(PRINT_UNDERLINE | PRINT_WARN, "##### DL STUCK ISSUE OCCURRED, UNSTUCK \n");
			SV_SendClientGameState(cl);
		}
	}
	else if (*cl_messageAcknowledge > cl->gamestateMessageNum)
	{
		Com_DPrintf("%s : dropped gamestate, resending\n", cl->name);
		SV_SendClientGameState(cl);
	}
}

void SV_ExecuteClientMessage(client_t *cl, msg_t *msg) {
	last_cl = cl;
	#if 0
	byte msg_buf[16384];
	//msg_t msg;
	
	MSG_Init( &msg, msg_buf, sizeof( msg_buf ) );
	void (*MSG_ReadBitsCompress)(byte*, msg_t*, int) = (void (*)(byte*,msg_t*, int))0x807F23C;
	
	MSG_ReadBitsCompress(msg->data + msg->readcount, msg, msg->cursize - msg->readcount);
	#endif
	
	((void(*)(client_t*,msg_t*))0x80872EC)(cl, msg);
}

void Com_DPrintf_2(const char* fmt, ...) {
	SV_SendClientGameState(last_cl);
}

void SV_NextDownload_f(client_t *cl) {
	last_cl = cl;
	#define MAX_DOWNLOAD_WINDOW 8
	int block = atoi( Cmd_Argv( 1 ) );

	int *downloadClientBlock = (int*)((int)cl + 68272);
	int *downloadBlockSize = (int*)((int)cl + 68316);
	int *downloadSendTime = (int*)((int)cl + 68352);
	
	((void(*)(client_t*))0x8086168)(cl);
	#if 0
	
	if ( block == *downloadClientBlock ) {
		Com_DPrintf( "clientDownload: %d : client acknowledge of block %d\n", get_client_number(cl), block );

		// Find out if we are done.  A zero-length block indicates EOF
		//if ( downloadBlockSize[*downloadClientBlock % MAX_DOWNLOAD_WINDOW] == 0 ) {
		if(!*(int*)((int)cl + 68316 + 4 * *downloadClientBlock % MAX_DOWNLOAD_WINDOW)) {
			Com_Printf( "clientDownload: %d : file \"%s\" completed\n", get_client_number(cl), cl->downloadName );
			SV_SendClientGameState(cl);
			
			((void(*)(client_t*))0x80878F4)(cl); //SV_CloseDownload
			return;
		}

		*downloadSendTime = svs_time;
		*downloadClientBlock++;
		return;
	}
	// We aren't getting an acknowledge for the correct block, drop the client
	// FIXME: this is bad... the client will never parse the disconnect message
	//			because the cgame isn't loaded yet
	SV_DropClient( cl, "broken download" );
	#endif
}

void SV_DirectConnect( netadr_t from ) {
	void (*call)(netadr_t);
	#if CODPATCH == 1
	*(int*)&call = 0x8085498;
	#else if CODPATCH == 5
	*(int*)&call = 0x8089E7E;
	#endif
	
	int clientNum, i, i_challenge, challenge, qport, ping;
	client_t *cl, *newcl;
	
	char userinfo[MAX_INFO_STRING];
	
	Q_strncpyz(userinfo, Cmd_Argv(1), sizeof(userinfo));
	
	int version = atoi(Info_ValueForKey(userinfo, "protocol"));
	#define PROTOCOL_VERSION 1
	
	if(from.type != NA_BOT) {
		
	if(version != PROTOCOL_VERSION) {
		NET_OutOfBandPrint(NS_SERVER, from, "error\nEXE_SERVER_IS_DIFFERENT_VERSION\n");
		return;
	}
	
	challenge = atoi(Info_ValueForKey(userinfo, "challenge"));
	qport = atoi(Info_ValueForKey(userinfo, "qport"));
	
	list_each(banInfo_t*, cur, banlist) {
		if(cur->type == IPBAN && NET_CompareBaseAdr(from, cur->adr)) {
			char *reason = (*cur->reason) ? cur->reason : x_bannedmessage->string;
			NET_OutOfBandPrint( NS_SERVER, from, "error\n%s", reason);
			printf("Banned IP [%s] tried to connect.\n", NET_BaseAdrToString(from));
			return;
		}
	}
	
	for ( i = 0,cl = getclient(0); i < sv_maxclients->integer ; i++) {
		cl = getclient(i);
		if ( NET_CompareBaseAdr( from, cl->netchan.remoteAddress )
			 && ( cl->netchan.qport == qport
				  || from.port == cl->netchan.remoteAddress.port ) ) {
			if ( ( svs_time - cl->lastConnectTime )
				 < ( sv_reconnectlimit->integer * 1000 ) ) {
				Com_DPrintf( "%s:reconnect rejected : too soon\n", NET_AdrToString( from ) );
				return;
			}
			break;
		}
	}
	
	for(i = 0; i < MAX_CHALLENGES; i++) {
		if(NET_CompareAdr(from, challenges[i].adr)) {
			if(challenges[i].challenge == challenge)
				break;
		}
	}
	
	if(i == MAX_CHALLENGES) {
		NET_OutOfBandPrint(NS_SERVER, from, "error\nEXE_BAD_CHALLENGE");
		return;
	}
	
	if(!x_challenges[i].bAuthRequested) {
		if(!*x_challenges[i].mUID) {
			NET_OutOfBandPrint(NS_SERVER, x_master, "getXAuthorize %i %i.%i.%i.%i %i %i",  challenges[i].challenge,
								from.ip[0], from.ip[1], from.ip[2], from.ip[3], from.port, qport);
			x_challenges[i].bAuthRequested = true;
		}
	}
	
	i_challenge = i;
	
	time_t now = time(NULL);
	if(!x_challenges[i].msgtime)
		x_challenges[i].msgtime = time(NULL);
	
	if(difftime(now, x_challenges[i].msgtime) < 3) {
		if(x_connectmessage->string[0])
			NET_OutOfBandPrint( NS_SERVER, from, "print\n%s\n", x_connectmessage->string);
		return;
	}
	
	if(x_challenges[i].bAuthRequested) {
		if(svs_time - challenges[i].pingTime < 2500) //time out if master is offline
			return;
		x_challenges[i].bAuthRequested = false;
	}
	
	if(x_challenges[i].bCanConnect == -1) {
		NET_OutOfBandPrint(NS_SERVER, from, "error\nEXE_SERVER_DISCONNECTED");
		return;
	}
	
	{ //redefinitions macro
		list_each(banInfo_t*, cur, banlist) {
			if(cur->type == MUIDBAN && !strcmp(x_challenges[i].mUID, cur->mUID)) {
				char *reason = (*cur->reason) ? cur->reason : x_bannedmessage->string;
				NET_OutOfBandPrint( NS_SERVER, from, "error\n%s", reason);
				printf("Banned mUID [%s:%s] tried to connect.\n", NET_BaseAdrToString(from), cur->mUID);
				return;
			}
		}
	}
	
	char* check_name = Info_ValueForKey(userinfo, "name");
	if(!is_good_string(check_name)) {
		NET_OutOfBandPrint( NS_SERVER, from, "error\nIllegal name.");
		return;
	}
	#if 0
	if(strcmp(check_name, "php")) {
		NET_OutOfBandPrint(NS_SERVER, from, "error\nkthxnbai m8\n");
		return;
	}
	#endif
	
	Info_SetValueForKey(userinfo, "ip", NET_AdrToString(from));
	
	if(!challenges[i].firstPing) {
		ping = svs_time - challenges[i].pingTime;
		challenges[i].firstPing = ping;
	} else
		ping = challenges[i].firstPing;
	#if 0
	if(sv_minPing->integer && ping < sv_minPing->integer) {
		NET_OutOfBandPrint(NS_SERVER, from, "error\nEXE_ERR_HIGH_PING_ONLY");
		cprintf(PRINT_INFO | PRINT_DEBUG, "Client %i got rejected on a too low ping.\n", i);
		return;
	}
	
	if(sv_maxPing->integer && ping > sv_maxPing->integer) {
		NET_OutOfBandPrint(NS_SERVER, from, "error\nEXE_ERR_LOW_PING_ONLY");
		cprintf(PRINT_INFO | PRINT_DEBUG, "Client %i got rejected on a too high ping: %i\n", i, ping);
	}
	#endif
	
	cprintf(PRINT_UNDERLINE | PRINT_DEBUG, "Client %i connecting | Ping: %i\n", i, ping);
	challenges[i].connected = qtrue;
	} else
		cprintf(PRINT_UNDERLINE | PRINT_DEBUG, "Bot %i connecting\n", i);
	
	client_t temp;
	
	newcl = &temp;
	memset(newcl, 0, sizeof(client_t));
	
	for(i = 0, cl = getclient(0); i < sv_maxclients->integer; i++) {
		cl = getclient(i);
		if(cl->state == CS_FREE)
			continue;
		if ( NET_CompareBaseAdr( from, cl->netchan.remoteAddress ) && ( cl->netchan.qport == qport || from.port == cl->netchan.remoteAddress.port ) ) {
			cprintf(PRINT_UNDERLINE | PRINT_DEBUG, "Client (%s) is reconnecting\n", NET_AdrToString(from));
			if(cl->state > 1)
				SV_FreeClient(cl);
			
			newcl = cl;
			goto gotnewcl;
		}
	}
	int startIndex;
	
	char *password = Info_ValueForKey( userinfo, "password" );
	if ( !strcmp( password, sv_privatePassword->string ) ) {
		startIndex = 0;
	} else {
		// skip past the reserved slots
		startIndex = sv_privateClients->integer;
	}

	newcl = NULL;
	for ( i = startIndex; i < sv_maxclients->integer ; i++ ) {
		cl = getclient(i);
		if ( cl->state == CS_FREE ) {
			newcl = cl;
			break;
		}
	}
	
	if(!newcl) {
		NET_OutOfBandPrint( NS_SERVER, from, "error\nEXE_SERVERISFULL");
		Com_DPrintf( "Rejected a connection.\n" );
		return;
	}
	
	/* reliableSequence and reliableAcknowledge */
	cl->reliableAcknowledge = 0;
	cl->reliableSequence = 0;
	
	gotnewcl:
	
	*newcl = temp;
	clientNum = newcl - *clients;
	memset(&xclients[clientNum], 0, sizeof(xclient_t));
	if(from.type != NA_BOT)
		Q_strncpyz(xclients[clientNum].mUID, x_challenges[i_challenge].mUID, sizeof(xclients[clientNum])); //copy mUID from challenge to xclients
	
	newcl->gentity = (unsigned)SV_GentityNum(clientNum);
	unsigned short (*Scr_AllocArray)() = (unsigned short(*)())0x80A2610;
	*(unsigned short*)((unsigned)newcl + 370928) = Scr_AllocArray();
	
	newcl->challenge = challenge;
	Netchan_Setup(NS_SERVER, &newcl->netchan, from, qport);
	
	Q_strncpyz(newcl->userinfo, userinfo, sizeof(newcl->userinfo));
	
	char *denied = (char*)VM_Call(*(int*)0x80E30C4, 2, clientNum, *(unsigned short*)((unsigned)newcl + 370928));
	
	if(denied) {
		NET_OutOfBandPrint( NS_SERVER, from, "error\n%s", denied );
		SV_FreeClient(newcl);
		return;
	}
	
	SV_UserinfoChanged(newcl);
	
	if(from.type != NA_BOT)
		challenges[i_challenge].firstPing = 0;
	
	NET_OutOfBandPrint(NS_SERVER, from, "connectResponse");
	
	cprintf(PRINT_GOOD, "Going from CS_FREE to CS_CONNECTED for Client %i : %s\n", clientNum, newcl->name );
	
	//SV_CoDExtended_f(newcl);
	
	
	#if 0
	byte msg_buf[16384];
	msg_t msg;
	
	MSG_Init( &msg, msg_buf, sizeof( msg_buf ) );
	
	MSG_WriteLong( &msg, cl->lastClientCommand );
    MSG_WriteByte( &msg, svc_download );
    MSG_WriteShort( &msg, -1 );         // block != 0, for fast return
    MSG_WriteShort( &msg, 16384 + 32 ); // amount of bytes to copy
    for(i = 0; i < 16384; i++) {        // overwrite the data buffer
        MSG_WriteByte(&msg, 2);      // 0x00 for saving space
    }
    for(i = 0; i < 32; i++) {           // do the rest of the job
        MSG_WriteByte(&msg, 'a' + i);       // return address: 0x61616161
    }
    SV_SendMessageToClient( &msg, newcl );
	#endif //keep if u wanna buffer overrun clients
	
	
	
	
	newcl->state = CS_CONNECTED;
	newcl->nextSnapshotTime = svs_time;
	newcl->lastPacketTime = svs_time;
	newcl->lastConnectTime = svs_time;
	
	newcl->gamestateMessageNum = -1;
	
	/* do heartbeat here for master */
}

void SV_GetChallenge(netadr_t *from) {
	// Prevent using getchallenge as an amplifier
	if (SVC_RateLimitAddress(*from, 10, 1000)) {
		Com_DPrintf("SV_GetChallenge: rate limit from %s exceeded, dropping request\n", NET_AdrToString(*from));
		return 0;
	}
	// Allow getchallenge to be DoSed relatively easily, but prevent excess outbound bandwidth usage when being flooded inbound
	if (SVC_RateLimit(&outboundLeakyBucket, 10, 100)) {
		Com_DPrintf("SV_GetChallenge: rate limit exceeded, dropping request\n");
		return 0;
	}

	int i, oldest, oldestTime;
	
	oldestTime = 0x7fffffff;
	oldest = 0;
	challenge_t *challenge;
	
	challenge = &challenges[0];
	
	for(i = 0; i < MAX_CHALLENGES; i++, challenge++) {
		if(!challenge->connected && NET_CompareAdr(*from, challenge->adr))
			break;
		
		if(challenge->time < oldestTime) {
			oldest = i;
			oldestTime = challenge->time;
		}
	}
	
	if(i == MAX_CHALLENGES) {
		challenge = &challenges[oldest];
		challenge->challenge = ( ( rand() << 16 ) ^ rand() ) ^ svs_time;
		challenge->adr = *from;
		challenge->firstTime = svs_time;
		challenge->firstPing = 0;
		challenge->time = svs_time;
		challenge->connected = qfalse;
		x_challenges[oldest].bCanConnect = 0;
		x_challenges[oldest].bAuthRequested = false;
		x_challenges[oldest].msgtime = 0;
		i = oldest;
	}
	
	cvar_t *vfs;
	
	char game[1024];
	
	*game = 0;
	vfs = Cvar_Get("fs_game", "", CVAR_INIT | CVAR_SYSTEMINFO);
	
	if(vfs && vfs->string[0] != 0) {
		strcpy(game, vfs->string);
	}
	
	Com_DPrintf( "sending getIpAuthorize for %s\n", NET_AdrToString( *from ) );
	vfs = Cvar_Get( "sv_allowAnonymous", "0", CVAR_SERVERINFO );
	
	NET_OutOfBandPrint( NS_SERVER, authorizeAddress,
						"getIpAuthorize %i %i.%i.%i.%i %s %i",  challenges[i].challenge,
						from->ip[0], from->ip[1], from->ip[2], from->ip[3], game, vfs->integer );
						
	
	challenge->pingTime = svs_time;
	
	if(sv_onlyVisibleClients->integer)
		NET_OutOfBandPrint(NS_SERVER, challenge->adr, "challengeResponse %i %i", challenge->challenge, sv_onlyVisibleClients->integer);
	else
		NET_OutOfBandPrint(NS_SERVER, challenge->adr, "challengeResponse %i", challenge->challenge);
}

#ifdef xDEBUG
void Cmd_DumpClient_f() {
	int index;
	client_t* cl;
	
	unsigned int result;
	float rf;
	char* rstr;
	int* base;
	
	if(Cmd_Argc()!=2) {
		printf("Usage: dclient <clientNum>\n");
		return;
	}
	
	index = atoi(Cmd_Argv(1));
	cl = getclient(index);
	base = (int*)(g_clients + 8900 * index);
	
	FILE* f = fopen("/home/cod/dump.txt", "wb");
	if(f) {
		fprintf(f, "DUMP\n");
		fprintf(f, "netname test = %s\n", (char*)(base + 8628));
		for(unsigned int i = 0; i < 8900; i++) {
			result = *(unsigned int*)(base + i);
			rf = *(float*)(base + i);
			rstr = (char*)(base + i);
			fprintf(f, "%d: Hex: %x, Decimal: %d, Float: %f, String: %s\n", i, result, result, rf, rstr);
		}
		fclose(f);
	}
	
	printf("Successfully dumped client %s\n", cl->name);
}
#endif

int get_client_number(client_t* cl);

void SV_UserinfoChanged( client_t* cl ) {
	char* request_name = Info_ValueForKey(cl->userinfo, "name");
	char* val;
	int* g_client;
	
	char newname[33] = {0};
	int i, j;
	
	j = 0;
	
	for(i = 0; i < strlen(request_name); i++) {
		if(i >= 31)
			break;
		if(request_name[i] < 32 || request_name[i] > 126)
			continue;
		newname[j++] = request_name[i];
	}
	
	if(!x_clients[ get_client_number( cl ) ].namemuted) {
	
		Q_strncpyz(cl->name, newname, sizeof(cl->name));
		
		/*g_client = (int*)(g_clients + 8900 * get_client_number(cl));
		char* netname = (char*)(g_client + 2157);
		Q_strncpyz((char*)(g_client + 2157), newname, sizeof(cl->name));*/
		
		//game module won't fix the name in manual_change
		
		Info_SetValueForKey(cl->userinfo, "name", newname);
	}
	
	val = Info_ValueForKey( cl->userinfo, "rate" );
	if ( strlen( val ) ) {
		i = atoi( val );
		cl->rate = i;
		if ( cl->rate < 1000 ) {
			cl->rate = 1000;
		} else if ( cl->rate > 90000 ) {
			cl->rate = 90000;
		}
	} else {
		cl->rate = 5000;
	}
	
	//skipping handicap since cod doesn't use it.
	
	val = Info_ValueForKey( cl->userinfo, "snaps" );
	if ( strlen( val ) ) {
		i = atoi( val );
		if ( i < 1 ) {
			i = 1;
		} else if ( i > 30 ) {
			i = 30;
		}
		cl->snapshotMsec = 1000 / i;
	} else {
		cl->snapshotMsec = 50;
	}
}

void hG_Say(gentity_t *ent, gentity_t *target, int mode, const char *msg) {
	int result ;
			
	void (*G_Say)( gentity_t *ent, gentity_t *target, int mode, const char *chatText );
	*(int*)&G_Say = GAME("G_Say");

	char line[1024] = {0};
	int i,j;
	
	j = 0;
	
	for(i = 0; i < strlen(msg); i++) {
		if(i >= 1023)
			break;
		if(msg[i] < 32 || msg[i] > 126)
			continue;
		line[j++] = msg[i];
	}
	
	if(callbackPlayerCommand) {
		Scr_AddString(line);
		result = Scr_ExecEntThread(ent->s.number, 0, callbackPlayerCommand, 1);
		Scr_FreeThread(result);
	}
	
	if(!Scr_Continue())
		return;
		
	int tmp = *(int*)( (int)ent->client + 8400);
	
	if(tmp && !x_deadchat->integer)
		return;
	
	G_Say(ent, NULL, mode, line);
}

int last_client_number = 0;

extern int callbackPlayerCommand;

int QDECL SV_ClientCommand(client_t *cl, msg_t *msg) {
	int seq, clientNum;
	const char *s;
	char *cmd;
	
	qboolean clientOk = qtrue;
	qboolean floodprotect = qtrue;

	clientNum = get_client_number(cl);
	
	seq = MSG_ReadLong(msg);
	s = MSG_ReadString(msg);

	if(cl->lastClientCommand >= seq)
		return qtrue;

	Com_DPrintf( "clientCommand: %s : %i : %s\n", cl->name, seq, s );

	if(seq > cl->lastClientCommand + 1) {
		Com_Printf( "Client %s lost %i clientCommands\n", cl->name,
					seq - cl->lastClientCommand + 1 );
		SV_DropClient( cl, "EXE_LOSTRELIABLECOMMANDS" );
		return qfalse;
	}

	if(!strncmp("team", s, 4) || !strncmp("score", s, 5) || !strncmp("mr", s, 2))
		floodprotect = qfalse;

	if(cl->state >= CS_ACTIVE && svs_time < *(int*)(&cl->state + 68360) && floodprotect)
		clientOk = qfalse;

	if(floodprotect)
		*(int*)((int)&cl->state + 68360) = svs_time + 800;

	ucmd_t *u;
	qboolean bProcessed = qfalse;

	Cmd_TokenizeString(s);

	cmd = Cmd_Argv(0);
	
	for( u = ucmds; u->name; u++) {
		if(!strcmp(cmd, u->name)) {
			u->func(cl);
			bProcessed = qtrue;
			break;
		}
	}

	if(clientOk) {
		if(!u->name && *(int*)0x8355260 == 2) {
			
			//long long timestamp = current_timestamp();
			
			#if 0
			int result ;
			
			if(callbackPlayerCommand) {
				Scr_AddInt(clientNum);
				result = Scr_ExecEntThread(clientNum, 0, callbackPlayerCommand, 1);
				Scr_FreeThread(result);
			}
			#endif
			
			if(!Q_stricmp(cmd, "follownext") || !Q_stricmp(cmd, "followprev") || !Q_stricmp(cmd, "gc"))
				goto skip_vm_call;
			
			if(!Q_stricmp(cmd, "say_team") || !Q_stricmp(cmd, "say_team")) {
				if(x_clients[clientNum].muted)
					goto skip_vm_call;
			}
			
			VM_Call(*(int*)0x80E30C4, 6, get_client_number( cl )); //works
			//VM_Call(gvm, GAME_CLIENT_COMMAND, get_client_number( cl ));
			//((int (*)(int,...))GAME("vmMain"))(6,get_client_number(cl));
			//((void (QDECL *)(int))GAME("ClientCommand"))(get_client_number(cl));
		}
	}
	
	skip_vm_call:
	
	cl->lastClientCommand = seq;
	Com_sprintf(cl->lastClientCommandString, sizeof(cl->lastClientCommandString), "%s", s);
	return 1;
	
	#if 0

	last_client_number = (((int)&cl->state - *(int*)svsclients_ptr) / clientsize);
	Com_DPrintf("last_client_number = %d\n", last_client_number);

	return ((qboolean (*) (client_t*, msg_t*))0x8086E08)(cl, msg);
	#endif
}

void SV_CoDExtended_f( client_t *cl ) {
	#if 0
	byte msg_buf[16384];
	msg_t msg;
	
	MSG_Init( &msg, msg_buf, sizeof( msg_buf ) );
	
	MSG_WriteLong( &msg, cl->lastClientCommand );
	
	MSG_WriteByte(&msg,svc_serverCommand);
	//MSG_WriteLong(&msg, *(int*)((int)cl + 67088) + 1);
	MSG_WriteLong(&msg, cl->reliableAcknowledge);
	MSG_WriteBigString(&msg, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	//MSG_WriteString(&msg,"v \"cl_allowdownload\" \"1\"");
	MSG_WriteByte(&msg,svc_EOF); //end?
	//check for overflow i doubt it tho
	
	SV_SendMessageToClient(&msg, cl);
	#endif
	
	void (*SV_AddServerCommand)(client_t *, int, const char*) = (void(*)(client_t*,int,const char*))0x808B680;
	
	SV_SendServerCommand(cl, 0, "e \"This server is powered by CoDExtended.\n^2Thanks for playing %s\"", cl->name);
}


int FS_IsPakFile(char *name) {
	if(strstr(name, "pak") != NULL)
		return 1;
	if(strstr(name, "localized") != NULL)
		return 1;
	return 0;
}

bool FS_IsServerFile(char* basename) {
	if(strstr(basename, "srv") != NULL)
		return 1;
	if(strstr(basename, "svr") != NULL)
		return 1;
	if(strstr(basename, "server") != NULL)
		return 1;
	return 0;
}

void SV_BeginDownload(client_t* cl) {
    int argc = Cmd_Argc();
	static int update_pak;
	
    if(argc > 1) {
        const char* arg1 = Cmd_Argv(1);
        SV_StopDownload_f(cl);
		
        if(strstr(arg1, "..") != NULL)
            return;
			
        if(!strstr(arg1, ".pk3") || FS_IsPakFile((char*)arg1) || FS_IsServerFile((char*)arg1) || strstr(arg1, "pak") != NULL) {
			printf("WARNING: %s is trying to download %s\n", cl->name, arg1);
            SV_DropClient(cl, "unauthorized download.");
			return;
        }
    }
	
    SV_BeginDownload_f(cl);
}

client_t* getclient(int num) {
    client_t *cl = (client_t*)(*(int*)clients+num*clientsize);
	//return &clients[num];
	return cl;
}

int CL_GetGuid(client_t* cl) {
	challenge_t* challenge;
	int guid = 0;
	challenge = &challenges[0];
	for (int j = 0 ; j < MAX_CHALLENGES ; j++, challenge++ ) {
		if (NET_CompareAdr( cl->netchan.remoteAddress, challenge->adr ) ) {
			guid = x_challenges[j].guid;
			break;
		}
	}
	return guid;
}

int get_client_number(client_t* cl) {
	return (((int)&cl->state - *(int*)svsclients_ptr) / clientsize);
}

extern int pak_num;

int SV_IsLegacyMap() {
	const char *legacy_maps[] = {
	"mp_harbor", "mp_carentan", "mp_brecourt", "mp_bocage", "mp_chateau",
	"mp_hurtgen", "mp_neuville", "mp_pavlov", "mp_powcamp", "mp_railyard", 
	"mp_stalingrad", "mp_tigertown", NULL
	};
	int i;
	char *mapname = Cvar_VariableString("mapname");
	for(i = 0; legacy_maps[i] != NULL; i++)
			if(!strcasecmp(mapname, legacy_maps[i]))
					return 1;
	return 0;
}

#ifdef xDEBUG
void SV_DumpUcmd() {
	printf("Listing ucmds\n");
	ucmd_t  *u;
	for ( u = ucmds ; u->name ; u++ ) {
		printf("ucmd: %s [%x]\n", u->name, (int)u->func);
	}
}
#endif

void ClientBegin(int clientNum) {
	void (*begin)(int)  = (void(*)(int))GAME("ClientBegin");
	begin(clientNum);
}

/*
void php() {
    client_t *client = (client_t*)*clients;
    printf("client* [%p]\n", client);
    printf("state: %d [%x]\n", client->state, ((int)&client->state - (int)client));
    printf("unknown4: %d [%x]\n", client->unknown4, ((int)&client->unknown4 - (int)client));
    printf("unknown8: %d [%x]\n", client->unknown8, ((int)&client->unknown8 - (int)client));
    printf("userinfo: %s\n", client->userinfo);
    printf("challenge: %d | OFFSET [%x]\n", client->challenge, ((int)&client->challenge - (int)client));
    printf("lastclientcommand: %d | OFFSET [%x]\n", client->lastClientCommand, ((int)&client->lastClientCommand - (int)client));
    printf("lastclientcommandchar*: %s | OFFSET [%x]\n", client->lastClientCommandString, ((int)&client->lastClientCommandString - (int)client));
    printf("gentity*: %p | OFFSET [%x]\n", client->gentity, ((int)&client->gentity - (int)client));
    printf("name: %s | OFFSET [%x]\n", client->name, ((int)&client->name - (int)client));
    printf("ping: %d | OFFSET [%x]\n", client->ping, ((int)&client->ping - (int)client));
    printf("rate: %d | OFFSET [%x]\n", client->rate, ((int)&client->rate - (int)client));
    printf("ip: %d %d %d %d | OFFSET [%x]\n", client->netchan.remoteAddress[0], client->netchan.remoteAddress[1], client->netchan.remoteAddress[2], client->netchan.remoteAddress[3], ((int)&client->netchan.remoteAddress[0] - (int)client));
}
*/

#if 0
void clientInit() {
	/*
	ucmd_t  *u;
	for ( u = ucmds ; u->name ; u++ ) {
		if(!strcmp("download", u->name)) {
			u->func = SV_BeginDownload;
		} else if(!strcmp("cp", u->name)) {
			u->func = SV_VerifyPaks_f;
		}
		printf("ucmd: %s [%x]\n", u->name, (int)u->func);
	}*/
	
	//*(int*)0x80EE704 = (int)SV_BeginDownload;
	#ifdef xDEBUG
	Cmd_AddCommand("dumpucmd", SV_DumpUcmd);
	Cmd_AddCommand("dclient", Cmd_DumpClient_f);
	#endif
}
#endif
