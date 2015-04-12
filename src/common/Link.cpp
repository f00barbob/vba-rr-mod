// VisualBoyAdvance - Nintendo Gameboy/GameboyAdvance (TM) emulator.
// Copyright (C) 1999-2003 Forgotten
// Copyright (C) 2004 Forgotten and the VBA development team
// This file was written by denopqrihg

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

// Link.cpp : Emulation of GBA link cable
//
#include "../win32/stdafx.h"
#include<afxwin.h>

#include "../gba/GBAGlobals.h"
#include "../gba/gba.h"
#include<stdio.h>
//#include "win32/stdafx.h"
#include "../port.h"
#include "Link.h"
#include "../win32/vba.h"
#include "../win32/MainWnd.h"
#include "../win32/dialogs/LinkOptions.h"
#include "../win32/Reg.h"

#define UPDATE_REG(address, value) WRITE16LE(((u16 *)&ioMem[address]),value)
#define LINK_ID1FINISH 2
#define LINK_ID2FINISH 4
#define GBLINK_READY 8

int linktime = 0;
u8 tspeed=3;
u8 transfer=0;
LINKDATA *linkmem=NULL;
int linkid = 1, vbaid = 1;
HANDLE linksync[4];
int savedlinktime=0;
char inifile[] = "vba1.ini";
HANDLE mmf=NULL;
char linkevent[] = "VBA link event  ";
static int i, j;
int linktimeout = 1000;
int linklog = 0;
FILE *jjj = NULL;
LANLINKDATA lanlink;
u16 linkdata[4];
int lspeed = 0;
lserver ls;
lclient lc;
bool oncewait = false, after = false;

extern unsigned char *gbMemory;
extern int gbInterrupt;

int trtimedata[3][4] = {{34080, 8520, 5680, 2840}, {65536, 16384, 10923, 5461}, {99609, 24903, 16602, 8301}};
int trtimeend[3][4] = {{72527, 18132, 12088, 6044}, {106608, 26652, 17768, 8884}, {133692, 33423, 22282, 11141}};
int gbtime = 1024;

DWORD WINAPI LinkClientThread(void *);
DWORD WINAPI LinkServerThread(void *);
int StartServer(void);

void StartLink(WORD value){
	if(ioMem==NULL)
		return;
	if(!(READ16LE(&ioMem[0x134])&0x8000)){
			switch(value & 0x3000){
			case 0x2000: // Multiplayer
				if(value & 0x80){
					if(!linkid){  
						if(!transfer){
							if(lanlink.active){
								if(lanlink.connected){
									linkdata[0] = READ16LE(&ioMem[0x12a]);
									savedlinktime = linktime;
									tspeed = value & 3;
									ls.Send();
									transfer = 1;
									linktime = 0;
									UPDATE_REG(0x120, linkdata[0]);
									UPDATE_REG(0x122, 0xffff);
									WRITE32LE(&ioMem[0x124], 0xffffffff);
									if(lanlink.speed&&oncewait==false) ls.howmanytimes++;
									after = false;
								}
							} else if(linkmem->numgbas>1){
								linkmem->linkflags &= !(LINK_ID1FINISH | LINK_ID2FINISH);
								ResetEvent(linksync[0]);
								linkmem->linkcmd[0] = ('M'<<8)+(value&3);
								linkmem->linkdata[0] = READ16LE(&ioMem[0x12a]);
								transfer = 1;
								if(linkmem->numtransfers!=0)
									savedlinktime = linktime;
								else
									savedlinktime = 0;
								linktime = 0;
								tspeed = value & 3;
								WRITE32LE(&ioMem[0x120], 0xffffffff);
								WRITE32LE(&ioMem[0x124], 0xffffffff);
							} else {
								value &= 0xff7f;
								value |= (transfer!=0)<<7;
							}
						}
					} else {
						value &= 0xff7f;
						value |= (transfer!=0)<<7;
					}
				}
				value &= 0xffbb;
				value |= (linkid ? 0xc : 8);

				UPDATE_REG(0x128, value);
				UPDATE_REG(0x134, 3);
			break;
			
			case 0:	// Normal 8-bit
				value |= 4;
				UPDATE_REG(0x128, value);
				if(linklog) fprintf(jjj, "Attempt to use 8-bit Normal mode %04x\n", value);
				break;

			case 0x1000: // Normal 32-bit
				value |= 4;
				UPDATE_REG(0x128, value);
				if(linklog) fprintf(jjj, "Attempt to use 32-bit Normal mode %04x %x%x\n", value, READ16LE(&ioMem[0x122]), READ16LE(&ioMem[0x120]));
				break;

			case 0x3000:	// UART
				if(linklog&&!linkid) fprintf(jjj, "Attempt to use UART mode %04x\n", value);
				UPDATE_REG(0x128, value);
				break;
			}
	} else UPDATE_REG(0x128, value);
}

void StartGPLink(u16 value){
	if(!value){
		UPDATE_REG(0x134, 0);
		return;
	}
	switch(value&0xC000){
	case 0:
	case 0x4000:
		switch(READ16LE(&ioMem[0x128])&0x3000){
		case 0x2000:
			value &= 0xc0f0;
			value |= 3;
			UPDATE_REG(0x134, value);
			UPDATE_REG(0x128, ((READ16LE(&ioMem[0x128])&0xff8b)|(linkid ? 0xc : 8)|(linkid<<4)));
			return;
			break;
		}
		break;
	case 0x8000:
		if(linklog) 
			if(value==0x8000)
				fprintf(jjj, "Circuit reset\n");
			else
				fprintf(jjj, "Attempt to use General-purpose mode %04x\n", value);
		UPDATE_REG(0x134, value);
		break;
	case 0xC000:
		UPDATE_REG(0x134, value);
		break;
	}
	return;
}

void StartJOYLink(u16 value){
	if(!value){
		UPDATE_REG(0x140, 0);
		return;
	}
	if(READ16LE(&ioMem[0x134])&0xC000==0xC000){ // this is confusing as hell
		if(linklog) fprintf(jjj, "Attempt to use JOY-BUS mode %04x\n", value);
	}
	return;
}

void LinkUpdate(void){
	if(lanlink.active){
		if(lanlink.connected){
			if(after) return;
			if(linkid&&!transfer&&lc.numtransfers>0&&linktime>=savedlinktime){
				linkdata[linkid] = READ16LE(&ioMem[0x12a]);
				if(!lc.oncesend) lc.Send();
				lc.oncesend = false;
				UPDATE_REG(0x120, linkdata[0]);
				UPDATE_REG(0x128, READ16LE(&ioMem[0x128]) | 0x80);
				transfer = 1;
				if(lc.numtransfers==1) linktime = 0;
				else linktime -= savedlinktime;
			}
			if(transfer&&linktime>=trtimeend[lanlink.numgbas-1][tspeed]){
				if(READ16LE(&ioMem[0x128]) & 0x4000){
					IF |= 0x80;
					UPDATE_REG(0x202, IF);
				}
				UPDATE_REG(0x128, (READ16LE(&ioMem[0x128]) & 0xff0f) | (linkid << 4));
				transfer = 0;
				linktime -= trtimeend[lanlink.numgbas-1][tspeed];
				oncewait = false;
				if(!lanlink.speed){
					if(linkid) lc.Recv();
					else ls.Recv();
					UPDATE_REG(0x122, linkdata[1]);
					UPDATE_REG(0x124, linkdata[2]);
					UPDATE_REG(0x126, linkdata[3]);
					if(linklog) fprintf(jjj, "%04x %04x %04x %04x %10u\n", linkdata[0], linkdata[1], linkdata[2], linkdata[3], savedlinktime);
					oncewait = true;
				} else {
					after = true;
				}
			}
		}
		return;
	}
	if(linkid){	
		savedlinktime = linkmem->lastlinktime;
		if(linkid==1||(linkid==2&&(linkmem->linkflags&LINK_ID1FINISH))||(linkid==3&&(linkmem->linkflags&LINK_ID2FINISH))){
			if(!transfer&&linktime>=savedlinktime&&linkmem->numtransfers){
				if(linkid==1){
					linkmem->linkdata[1] = READ16LE(&ioMem[0x12a]);
				}
				else 
				if(linkid==2){
					linkmem->linkflags &= !LINK_ID1FINISH;
					linkmem->linkdata[2] = READ16LE(&ioMem[0x12a]);
				}
				else if(linkid==3){
					linkmem->linkflags &= !LINK_ID2FINISH;
					linkmem->linkdata[3] = READ16LE(&ioMem[0x12a]);
				}

				if(linkmem->numtransfers!=1)
					linktime -= savedlinktime;
				else
					linktime = 0;

				switch((linkmem->linkcmd[0])>>8){
				case 'M':
					tspeed = (linkmem->linkcmd[0]) & 3;
					transfer = 1;
					WRITE32LE(&ioMem[0x120], 0xffffffff);
					WRITE32LE(&ioMem[0x124], 0xffffffff);
					UPDATE_REG(0x128, READ16LE(&ioMem[0x128]) | 0x80);
					break;
				}
			}
		}
	}
	
	if(!transfer) return;

	if(transfer == 1 && linktime >= trtimedata[0][tspeed]){
		UPDATE_REG(0x120, linkmem->linkdata[0]);
		transfer++;
	}
	
	if(transfer == 2 && linktime >= trtimedata[1][tspeed]){
		if(!linkid){
			linkmem->numtransfers++;
			if(linkmem->numtransfers==0)
				linkmem->numtransfers=2;
			linkmem->lastlinktime=savedlinktime;
				SetEvent(linksync[1]);
			WaitForSingleObject(linksync[0], linktimeout);
			ResetEvent(linksync[0]);
		}
		if(linklog)
			fprintf(jjj, "%04x %04x %04x %04x %10u\n", 
				linkmem->linkdata[0], linkmem->linkdata[1], linkmem->linkdata[2], linkmem->linkdata[3], linkmem->lastlinktime);
			
		UPDATE_REG(0x122, linkmem->linkdata[1]);
		transfer++;
	}

	if(transfer == 3 && linktime >= trtimeend[0][tspeed] && linkmem->numgbas==2){
		if(linkid){
			SetEvent(linksync[0]);
			if(WaitForSingleObject(linksync[1], linktimeout)==WAIT_TIMEOUT){
				linkmem->numtransfers=0;
			}
			ResetEvent(linksync[1]);
		}
		transfer = 0;
		linktime -= trtimeend[0][tspeed];
		if((*(u16*)&ioMem[0x128]) & 0x4000){
			IF |= 0x80;
			UPDATE_REG(0x202, IF);
		}
		UPDATE_REG(0x128, (*(u16 *)&ioMem[0x128] & 0xff0f) | (linkid << 4));
		linkmem->linkdata[linkid] = 0xffff;
	}
	
	if(transfer == 3 && linktime >= trtimedata[2][tspeed]){
		if(linkid==1){
			linkmem->linkflags |= LINK_ID1FINISH;
			if(linkmem->numtransfers!=1)
				SetEvent(linksync[2]);
			if(WaitForSingleObject(linksync[1], linktimeout)==WAIT_TIMEOUT){
				linkmem->numtransfers=0;
			}
			ResetEvent(linksync[1]);
		}
		UPDATE_REG(0x124, linkmem->linkdata[2]);
		transfer++;
	}

	if(transfer == 4 && linktime >= trtimeend[1][tspeed] && linkmem->numgbas==3){
		if(linkid==2){
			SetEvent(linksync[0]);
			if(WaitForSingleObject(linksync[2], linktimeout)==WAIT_TIMEOUT){
				linkmem->numtransfers=0;
			}
		ResetEvent(linksync[2]);
		}
		transfer = 0;
		linktime -= trtimeend[1][tspeed];
		if((*(u16*)&ioMem[0x128]) & 0x4000){
			IF |= 0x80;
			UPDATE_REG(0x202, IF);
		}
		UPDATE_REG(0x128, (*(u16 *)&ioMem[0x128] & 0xff0f) | (linkid << 4));
		linkmem->linkdata[linkid] = 0xffff;
	}

	if(transfer == 4 && linktime >= trtimeend[2][tspeed]){
		if(linkid==2){
			linkmem->linkflags |= LINK_ID2FINISH;
			if(linkmem->numtransfers!=1)
				SetEvent(linksync[3]);
			if(WaitForSingleObject(linksync[2], linktimeout)==WAIT_TIMEOUT){
				linkmem->numtransfers=0;
			}
			ResetEvent(linksync[2]);
		}
		UPDATE_REG(0x126, linkmem->linkdata[3]);
		if(linkid==3){
			SetEvent(linksync[0]);
			if(WaitForSingleObject(linksync[3], linktimeout)==WAIT_TIMEOUT){
				linkmem->numtransfers=0;
			}
		ResetEvent(linksync[3]);
		}
		transfer = 0;
		linktime -= trtimeend[2][tspeed];
		if((*(u16*)&ioMem[0x128]) & 0x4000){
			IF |= 0x80;
			UPDATE_REG(0x202, IF);
		}
		UPDATE_REG(0x128, (*(u16 *)&ioMem[0x128] & 0xff0f) | (linkid << 4));
		linkmem->linkdata[linkid] = 0xffff;
	}

return;
}

void gbLinkStart(u8 value){
// Not in this version :-)
}


void gbLinkUpdate(void){
}

int InitLink(void){
	WSADATA wsadata;
	BOOL disable = true;

	if((linkmem=(LINKDATA*)malloc(sizeof(LINKDATA)))==NULL){
		MessageBox(NULL, "Not enough memory.", "Error!", MB_OK);
		return 0;
	}
	linkid = 0;
	inifile[3]='1';
	
	if(WSAStartup(MAKEWORD(1,1), &wsadata)!=0){
		WSACleanup();
		return 0;
	}
	
	if((lanlink.tcpsocket=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))==INVALID_SOCKET){
		MessageBox(NULL, "Couldn't create socket.", "Error!", MB_OK);
		WSACleanup();
		return 0;
	}

	setsockopt(lanlink.tcpsocket, IPPROTO_TCP, TCP_NODELAY, (char*)&disable, sizeof(BOOL)); 

	if((mmf=CreateFileMapping((HANDLE)0xffffffff, NULL, PAGE_READWRITE, 0, sizeof(LINKDATA), "VBA link memory"))==NULL){	  
		closesocket(lanlink.tcpsocket);
		WSACleanup();
		MessageBox(NULL, "Error creating file mapping", "Error", MB_OK|MB_ICONEXCLAMATION);
		return 0;
	}
		
	if(GetLastError() == ERROR_ALREADY_EXISTS)
		vbaid = 1;
	else
 		vbaid = 0;
		  
	if((linkmem=(LINKDATA *)MapViewOfFile(mmf, FILE_MAP_WRITE, 0, 0, sizeof(LINKDATA)))==NULL){
		closesocket(lanlink.tcpsocket);
		WSACleanup();
		CloseHandle(mmf);
		MessageBox(NULL, "Error mapping file", "Error", MB_OK|MB_ICONEXCLAMATION);
		return 0;
	}

	if(linkmem->linkflags&LINK_PARENTLOST)
		vbaid = 0;

	if(vbaid==0){
		inifile[3]='1';
		linkid = 0;
		if(linkmem->linkflags&LINK_PARENTLOST){
			linkmem->numgbas++;
			linkmem->linkflags &= ~LINK_PARENTLOST;
		}
		else
			linkmem->numgbas=1;

		for(i=0;i<4;i++){
			linkevent[15]=(char)i+'1';
			if((linksync[i]=CreateEvent(NULL, true, false, linkevent))==NULL){
				closesocket(lanlink.tcpsocket);
				WSACleanup();
				UnmapViewOfFile(linkmem);
				CloseHandle(mmf);
				for(j=0;j<i;j++){
					CloseHandle(linksync[j]);
				}
				MessageBox(NULL, "Error opening event", "Error", MB_OK|MB_ICONEXCLAMATION);
				return 0;
			}
		}
	} else {
		vbaid=linkmem->numgbas;
		linkid = vbaid;
		linkmem->numgbas++;
		linklog = 0;
		if(linkmem->numgbas>4){
			linkmem->numgbas=4;
			closesocket(lanlink.tcpsocket);
			WSACleanup();
			MessageBox(NULL, "5 or more GBAs not supported.", "Error!", MB_OK|MB_ICONEXCLAMATION);
			UnmapViewOfFile(linkmem);
			CloseHandle(mmf);
			return 0;
		} // hope for the best
		inifile[3]=(char)linkmem->numgbas+'0';
		for(i=0;i<4;i++){
			linkevent[15]=(char)i+'1';
			if((linksync[i]=OpenEvent(EVENT_ALL_ACCESS, false, linkevent))==NULL){
				closesocket(lanlink.tcpsocket);
				WSACleanup();
				CloseHandle(mmf);
				UnmapViewOfFile(linkmem);
				for(j=0;j<i;j++){
					CloseHandle(linksync[j]);
				}
				MessageBox(NULL, "Error opening event", "Error", MB_OK|MB_ICONEXCLAMATION);
				return 0;
			}
		}
	}

	linkmem->lastlinktime=0xffffffff;
	linkmem->numtransfers=0;
	linkmem->linkflags=0;
	lanlink.connected = false;
	lanlink.thread = NULL;
	lanlink.speed = false;
	for(i=0;i<4;i++){
		linkmem->linkdata[i] = 0xffff;
		linkdata[i] = 0xffff;
	}
return 1;
}

void CloseLink(void){
	if(lanlink.connected){
		if(linkid){
			char outbuffer[4];
			outbuffer[0] = 4;
			outbuffer[1] = -32;
			if(lanlink.type==0) send(lanlink.tcpsocket, outbuffer, 4, 0);
		} else {
			char outbuffer[12];
			int i;
			outbuffer[0] = 12;
			outbuffer[1] = -32;
			for(i=1;i<=lanlink.numgbas;i++){
				if(lanlink.type==0){
					send(ls.tcpsocket[i], outbuffer, 12, 0);
				} 
				closesocket(ls.tcpsocket[i]);
			}
		}
	}
	linkmem->numgbas--;
	if(!linkid&&linkmem->numgbas!=0)
		linkmem->linkflags|=LINK_PARENTLOST;
	CloseHandle(mmf);
	UnmapViewOfFile(linkmem);

	for(i=0;i<4;i++){
		if(linksync[i]!=NULL){
			PulseEvent(linksync[i]);
			CloseHandle(linksync[i]);
		}
	}
	regSetDwordValue("LAN", lanlink.active);
	if(linklog) fclose(jjj);
	closesocket(lanlink.tcpsocket);
	WSACleanup();
return;
}

lserver::lserver(void){
	intinbuffer = (int*)inbuffer;
	u16inbuffer = (u16*)inbuffer;
	intoutbuffer = (int*)outbuffer;
	u16outbuffer = (u16*)outbuffer;
	oncewait = false;
}

int lserver::Init(void *serverdlg){
	SOCKADDR_IN info;
	DWORD nothing;
	char str[100];

	info.sin_family = AF_INET;
	info.sin_addr.S_un.S_addr = INADDR_ANY;
	info.sin_port = htons(5738);

	if(bind(lanlink.tcpsocket, (LPSOCKADDR)&info, sizeof(SOCKADDR_IN))==SOCKET_ERROR){
		closesocket(lanlink.tcpsocket);
		if((lanlink.tcpsocket=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))==INVALID_SOCKET)
			return WSAGetLastError();
		if(bind(lanlink.tcpsocket, (LPSOCKADDR)&info, sizeof(SOCKADDR_IN))==SOCKET_ERROR)
			return WSAGetLastError();
	}

	if(listen(lanlink.tcpsocket, lanlink.numgbas)==SOCKET_ERROR)
		return WSAGetLastError();

	if(lanlink.thread!=NULL){
		lanlink.terminate = true;
		WaitForSingleObject(linksync[vbaid], 500);
		lanlink.thread = NULL;
	}
	lanlink.terminate = false;
	linkid = 0;
	
	gethostname(str, 100);
	((ServerWait*)serverdlg)->m_serveraddress.Format("Server IP address is: %s", inet_ntoa(*(LPIN_ADDR)(gethostbyname(str)->h_addr_list[0])));
	
	lanlink.thread = CreateThread(NULL, 0, LinkServerThread, serverdlg, 0, &nothing);

	return 0;

}

DWORD WINAPI LinkServerThread(void *serverdlg){	
	fd_set fdset;
	timeval wsocktimeout;
	char inbuffer[256], outbuffer[256];
	int *intinbuffer = (int*)inbuffer;
	u16 *u16inbuffer = (u16*)inbuffer;
	int *intoutbuffer = (int*)outbuffer;
	u16 *u16outbuffer = (u16*)outbuffer;
	BOOL disable = true;

	wsocktimeout.tv_sec = 1;
	wsocktimeout.tv_usec = 0;
	i = 0;
	
	while(i<lanlink.numgbas){
		fdset.fd_count = 1;
		fdset.fd_array[0] = lanlink.tcpsocket;
		if(select(0, &fdset, NULL, NULL, &wsocktimeout)==1){
			if(lanlink.terminate){
				SetEvent(linksync[vbaid]);
				return 0;
			}
			if((ls.tcpsocket[i+1]=accept(lanlink.tcpsocket, NULL, NULL))==INVALID_SOCKET){
				for(int j=1;j<i;j++) closesocket(ls.tcpsocket[j]);
				MessageBox(NULL, "Network error.", "Error", MB_OK);
				return 1;
			} else {
				setsockopt(ls.tcpsocket[i+1], IPPROTO_TCP, TCP_NODELAY, (char*)&disable, sizeof(BOOL));
				u16outbuffer[0] = i+1;
				u16outbuffer[1] = lanlink.numgbas;
				send(ls.tcpsocket[i+1], outbuffer, 4, 0);
				((ServerWait*)serverdlg)->m_plconn[i].Format("Player %d connected", i+1);
				((ServerWait*)serverdlg)->UpdateData(false);
				i++;
			}
		}
		((ServerWait*)serverdlg)->m_prgctrl.StepIt();
	}
	MessageBox(NULL, "All players connected", "Link", MB_OK);
	((ServerWait*)serverdlg)->SendMessage(WM_CLOSE, 0, 0);
		
	for(i=1;i<=lanlink.numgbas;i++){
		outbuffer[0] = 4;
		send(ls.tcpsocket[i], outbuffer, 4, 0);
	}

	lanlink.connected = true;

	return 0;
}

void lserver::Send(void){
	if(lanlink.type==0){	// TCP
		if(savedlinktime==-1){
			outbuffer[0] = 4;
			outbuffer[1] = -32;	//0xe0
			for(i=1;i<=lanlink.numgbas;i++){
				send(tcpsocket[i], outbuffer, 4, 0);
				recv(tcpsocket[i], inbuffer, 4, 0);
			}
		}
		outbuffer[1] = tspeed;
		u16outbuffer[1] = linkdata[0];
		intoutbuffer[1] = savedlinktime;
		if(lanlink.numgbas==1){
			if(lanlink.type==0){
				outbuffer[0] = 8;
				send(tcpsocket[1], outbuffer, 8, 0);
			}
		}
		else if(lanlink.numgbas==2){
			u16outbuffer[4] = linkdata[2];
			if(lanlink.type==0){
				outbuffer[0] = 10;
				send(tcpsocket[1], outbuffer, 10, 0);
				u16outbuffer[4] = linkdata[1];
				send(tcpsocket[2], outbuffer, 10, 0);
			}
		} else {
			if(lanlink.type==0){
				outbuffer[0] = 12;
				u16outbuffer[4] = linkdata[2];
				u16outbuffer[5] = linkdata[3];
				send(tcpsocket[1], outbuffer, 12, 0);
				u16outbuffer[4] = linkdata[1];
				send(tcpsocket[2], outbuffer, 12, 0);
				u16outbuffer[5] = linkdata[2];
				send(tcpsocket[3], outbuffer, 12, 0);
			}
		}
	}
	return;
}

void lserver::Recv(void){
	int numbytes;
	if(lanlink.type==0){	// TCP
		wsocktimeout.tv_usec =  0;
		wsocktimeout.tv_sec = linktimeout / 1000;
		fdset.fd_count = lanlink.numgbas;
		for(i=0;i<lanlink.numgbas;i++) fdset.fd_array[i] = tcpsocket[i+1];
		if(select(0, &fdset, NULL, NULL, &wsocktimeout)==0){
			return;
		}
		howmanytimes++;
		for(i=0;i<lanlink.numgbas;i++){
			numbytes = 0;
			inbuffer[0] = 1;
			while(numbytes<howmanytimes*inbuffer[0])
				numbytes += recv(tcpsocket[i+1], inbuffer+numbytes, 256-numbytes, 0);
			if(howmanytimes>1) memcpy(inbuffer, inbuffer+inbuffer[0]*(howmanytimes-1), inbuffer[0]);
			if(inbuffer[1]==-32){
				char message[30];
				lanlink.connected = false;
				sprintf(message, "Player %d disconnected.", i+2);
				MessageBox(NULL, message, "Link", MB_OK);
				outbuffer[0] = 4;
				outbuffer[1] = -32;
				for(i=1;i<lanlink.numgbas;i++){
					send(tcpsocket[i], outbuffer, 12, 0);
					recv(tcpsocket[i], inbuffer, 256, 0);
					closesocket(tcpsocket[i]);
				}
				return;
			}
			linkdata[i+1] = u16inbuffer[1];
		}
		howmanytimes = 0;
	}
	after = false;
	return;
}

lclient::lclient(void){
	intinbuffer = (int*)inbuffer;
	u16inbuffer = (u16*)inbuffer;
	intoutbuffer = (int*)outbuffer;
	u16outbuffer = (u16*)outbuffer;
	numtransfers = 0;
	oncesend = false;
	return;
}

int lclient::Init(LPHOSTENT hostentry, void *waitdlg){
	unsigned long notblock = 1;
	DWORD nothing;
	
	serverinfo.sin_family = AF_INET;
	serverinfo.sin_port = htons(5738);
	serverinfo.sin_addr = *((LPIN_ADDR)*hostentry->h_addr_list);
	
	if(ioctlsocket(lanlink.tcpsocket, FIONBIO, &notblock)==SOCKET_ERROR)
		return WSAGetLastError();

	if(lanlink.thread!=NULL){
		lanlink.terminate = true;
		WaitForSingleObject(linksync[vbaid], 500);
		lanlink.thread = NULL;
	}

	//((ServerWait*)waitdlg)->SetWindowText("Connecting..."); // this bork?

	lanlink.terminate = false;
	lanlink.thread = CreateThread(NULL, 0, LinkClientThread, waitdlg, 0, &nothing);
	return 0;
}

DWORD WINAPI LinkClientThread(void *waitdlg){
	fd_set fdset;
	timeval wsocktimeout;
	int numbytes;
	char inbuffer[16];
	u16 *u16inbuffer = (u16*)inbuffer;
	unsigned long block = 0;

	if(connect(lanlink.tcpsocket, (LPSOCKADDR)&lc.serverinfo, sizeof(SOCKADDR_IN))==SOCKET_ERROR){
		if(WSAGetLastError()!=WSAEWOULDBLOCK){
			MessageBox(NULL, "Couldn't connect to server.", "Link", MB_OK);
			return 1;
		}
		wsocktimeout.tv_sec = 1;
		wsocktimeout.tv_usec = 0;
		do{
			if(lanlink.terminate) return 0;
			fdset.fd_count = 1;
			fdset.fd_array[0] = lanlink.tcpsocket;
			((ServerWait*)waitdlg)->m_prgctrl.StepIt();
		} while(select(0, NULL, &fdset, NULL, &wsocktimeout)!=1&&connect(lanlink.tcpsocket, (LPSOCKADDR)&lc.serverinfo, sizeof(SOCKADDR_IN))!=0);
	}
	
	ioctlsocket(lanlink.tcpsocket, FIONBIO, &block);

	numbytes = 0;
	while(numbytes<4) 
		numbytes += recv(lanlink.tcpsocket, inbuffer+numbytes, 16, 0);
	linkid = (int)u16inbuffer[0];
	lanlink.numgbas = (int)u16inbuffer[1];
	
	((ServerWait*)waitdlg)->m_serveraddress.Format("Connected as #%d", linkid+1);
	if(lanlink.numgbas!=linkid)	((ServerWait*)waitdlg)->m_plconn[0].Format("Waiting for %d players to join", lanlink.numgbas-linkid);
	else ((ServerWait*)waitdlg)->m_plconn[0].Format("All players joined.");
		
	numbytes = 0;
	inbuffer[0] = 1;
	while(numbytes<inbuffer[0]) 
		numbytes += recv(lanlink.tcpsocket, inbuffer+numbytes, 16, 0);
		
	MessageBox(NULL, "Connected.", "Link", MB_OK);
	((ServerWait*)waitdlg)->SendMessage(WM_CLOSE, 0, 0);

	block = 1;

	ioctlsocket(lanlink.tcpsocket, FIONBIO, &block);

	lanlink.connected = true;
	return 0;
}

void lclient::CheckConn(void){
	if((numbytes=recv(lanlink.tcpsocket, inbuffer, 256, 0))>0){
		while(numbytes<inbuffer[0]) 
			numbytes += recv(lanlink.tcpsocket, inbuffer+numbytes, 256, 0);
		if(inbuffer[1]==-32){
				outbuffer[0] = 4;
				send(lanlink.tcpsocket, outbuffer, 4, 0);
				lanlink.connected = false;
				MessageBox(NULL, "Server disconnected.", "Link", MB_OK);
				return;
		}
		numtransfers = 1;
		savedlinktime = 0;
		linkdata[0] = u16inbuffer[1];
		tspeed = inbuffer[1] & 3;
		for(i=1, numbytes=4;i<=lanlink.numgbas;i++)
			if(i!=linkid) linkdata[i] = u16inbuffer[numbytes++];
		after = false;
		oncewait = true;
		oncesend = true;
	}
	return;
}


void lclient::Recv(void){
	fdset.fd_count = 1;
	fdset.fd_array[0] = lanlink.tcpsocket;
	wsocktimeout.tv_sec = linktimeout / 1000;
	wsocktimeout.tv_usec = 0;
	if(select(0, &fdset, NULL, NULL, &wsocktimeout)==0){
		numtransfers = 0;
		return;
	}
	numbytes = 0;
	inbuffer[0] = 1;
	while(numbytes<inbuffer[0]) 
		numbytes += recv(lanlink.tcpsocket, inbuffer+numbytes, 256, 0);
	if(inbuffer[1]==-32){
		outbuffer[0] = 4;
		send(lanlink.tcpsocket, outbuffer, 4, 0);
		lanlink.connected = false;
		MessageBox(NULL, "Server disconnected.", "Link", MB_OK);
		return;
	}
	tspeed = inbuffer[1] & 3;
	linkdata[0] = u16inbuffer[1];
	savedlinktime = intinbuffer[1];
	for(i=1, numbytes=4;i<lanlink.numgbas+1;i++)
		if(i!=linkid) linkdata[i] = u16inbuffer[numbytes++];
	numtransfers++;
	if(numtransfers==0) numtransfers = 2;
	after = false;
}

void lclient::Send(){
	outbuffer[0] = 4;
	outbuffer[1] = linkid<<2;
	u16outbuffer[1] = linkdata[linkid];
	send(lanlink.tcpsocket, outbuffer, 4, 0);
	return;
}

void LinkSStop(void){
	if(!oncewait){
		if(linkid) lc.Recv();
		else ls.Recv();
			
		oncewait = true;
		UPDATE_REG(0x122, linkdata[1]);
		UPDATE_REG(0x124, linkdata[2]);
		UPDATE_REG(0x126, linkdata[3]);
		if(linklog) fprintf(jjj, "%04x %04x %04x %04x %10u\n", linkdata[0], linkdata[1], linkdata[2], linkdata[3], savedlinktime);
	}
	return;
}

void LinkSSend(u16 value){
	if(linkid&&!lc.oncesend){
		linkdata[linkid] = value;
		lc.Send();
		lc.oncesend = true;
	}
}
