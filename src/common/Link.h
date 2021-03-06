// VisualBoyAdvance - Nintendo Gameboy/GameboyAdvance (TM) emulator.
// Copyright (C) 1999-2003 Forgotten
// Copyright (C) 2004 Forgotten and the VBA development team

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or(at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#include<winsock.h>


#ifndef LINKH
#define LINKH
#define LINK_PARENTLOST 0x80
#define CONNECTION_OK 9999;

typedef struct {
	WORD linkdata[4];
	WORD linkcmd[4];
	WORD numtransfers;
	int lastlinktime;
	unsigned char numgbas;
	unsigned char linkflags;
} LINKDATA;

class lserver{
	fd_set fdset;
	timeval wsocktimeout;
	//timeval udptimeout;
	char inbuffer[256], outbuffer[256];
	int *intinbuffer;
	u16 *u16inbuffer;
	int *intoutbuffer;
	u16 *u16outbuffer;
	int counter;
public:
	int howmanytimes;
	SOCKET tcpsocket[4];
	SOCKADDR_IN udpaddr[4];
	lserver(void);
	int Init(void*);
	void Send(void);
	void Recv(void);
};

class lclient{
	fd_set fdset;
	timeval wsocktimeout;
	char inbuffer[256], outbuffer[256];
	int *intinbuffer;
	u16 *u16inbuffer;
	int *intoutbuffer;
	u16 *u16outbuffer;
	int numbytes;
public:
	bool oncesend;
	SOCKADDR_IN serverinfo;
	SOCKET noblock;
	int numtransfers;
	lclient(void);
	int Init(LPHOSTENT, void*);
	void Send(void);
	void Recv(void);
	void CheckConn(void);
};

typedef struct {
	SOCKET tcpsocket;
	//SOCKET udpsocket;
	int numgbas;
	HANDLE thread;
	u8 type;
	u8 server;
	bool terminate;
	bool connected;
	bool speed;
	bool active;
} LANLINKDATA;

extern int vbaid;
extern int linklog;
extern int linktime;
extern void StartLink(u16);
extern void StartGPLink(u16);
extern void StartJOYLink(u16);
extern void LinkUpdate(void);
extern void LinkChildStop(void);
extern void LinkChildSend(u16);
extern LANLINKDATA lanlink;

#endif
