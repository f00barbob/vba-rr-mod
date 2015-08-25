#ifndef POKERAM_H
#define POKERAM_H

#include <string>



typedef struct rammap {

	string item_names[256];

	int money[3];

	struct {
		int frames;
		int seconds;
		int minutes1;
		int minutes2;
		int hours1;
		int hours2;
	} time;

	struct{
		int item_base;
		int count;
		int qty_base;
		int item[20];
		int qty[20];
		int end;
	} items;

	struct{
		int item_base;
		int count;
		int qty_base;
		int item[50];
		int qty[50];
		int end;
	} pc_items;
	


} pokerammap;

static pokerammap pokeram;


void init_rammap()
{
	pokeram.money[0] = 0xD347;
	pokeram.money[1] = 0xD348;
	pokeram.money[2] = 0xD349;
	
	pokeram.time.hours2 =	0xDA40;
	pokeram.time.hours1 =	0xDA41;
	pokeram.time.minutes2 = 0xDA42;
	pokeram.time.minutes1 = 0xDA43;
	pokeram.time.seconds =	0xDA44;
	pokeram.time.frames =	0xDA45;
	
	
// gen 1	
	/*pokeram.items.count = 0xD31D;
	pokeram.items.item_base = 0xD31E;
	pokeram.items.qty_base = 0xD31F;
	pokeram.items.end = 0xD346;*/

// gen 2
	pokeram.items.count = 0xD8D7;
	pokeram.items.item_base = 0xD8D8;
	pokeram.items.qty_base = 0xD8D9;
	pokeram.items.end = 0xD346;

	pokeram.pc_items.count = 0xD53A;
	pokeram.pc_items.item_base = 0xD53B;
	pokeram.pc_items.qty_base = 0xD53C;
	pokeram.pc_items.end = 0xD59F;
	
	for (int i = 0; i < 12; i++) // edited for ball-bag on crystal
	{
		pokeram.items.item[i] = pokeram.items.item_base + 2 * i;
		pokeram.items.qty[i] = pokeram.items.qty_base + 2 * i;
	}

	for (int i = 0; i < 50; i++)
	{
		pokeram.pc_items.item[i] = pokeram.pc_items.item_base + 2 * i;
		pokeram.pc_items.qty[i] = pokeram.pc_items.qty_base + 2 * i;
	}


	pokeram.item_names[0] = "Nothing*";
	pokeram.item_names[1] = "Master Ball";
	pokeram.item_names[2] = "Ultra Ball";
	pokeram.item_names[3] = "Great Ball";
	pokeram.item_names[4] = "Poké Ball";
	pokeram.item_names[5] = "Town Map";
	pokeram.item_names[6] = "Bicycle";
	pokeram.item_names[7] = "?????";
	pokeram.item_names[8] = "Safari Ball";
	pokeram.item_names[9] = "Pokédex";
	pokeram.item_names[10] = "Moon Stone";
	pokeram.item_names[11] = "Antidote";
	pokeram.item_names[12] = "Burn Heal";
	pokeram.item_names[13] = "Ice Heal";
	pokeram.item_names[14] = "Awakening";
	pokeram.item_names[15] = "Parlyz Heal";
	pokeram.item_names[16] = "Full Restore";
	pokeram.item_names[17] = "Max Potion";
	pokeram.item_names[18] = "Hyper Potion";
	pokeram.item_names[19] = "Super Potion";
	pokeram.item_names[20] = "Potion";
	pokeram.item_names[21] = "BoulderBadge";
	pokeram.item_names[22] = "CascadeBadge";
	pokeram.item_names[23] = "ThunderBadge";
	pokeram.item_names[24] = "RainbowBadge";
	pokeram.item_names[25] = "SoulBadge";
	pokeram.item_names[26] = "MarshBadge";
	pokeram.item_names[27] = "VolcanoBadge";
	pokeram.item_names[28] = "EarthBadge";
	pokeram.item_names[29] = "Escape Rope";
	pokeram.item_names[30] = "Repel";
	pokeram.item_names[31] = "Old Amber";
	pokeram.item_names[32] = "Fire Stone";
	pokeram.item_names[33] = "Thunder Stone";
	pokeram.item_names[34] = "Water Stone";
	pokeram.item_names[35] = "HP Up";
	pokeram.item_names[36] = "Protein";
	pokeram.item_names[37] = "Iron";
	pokeram.item_names[38] = "Carbos";
	pokeram.item_names[39] = "Calcium";
	pokeram.item_names[40] = "Rare Candy";
	pokeram.item_names[41] = "Dome Fossil";
	pokeram.item_names[42] = "Helix Fossil";
	pokeram.item_names[43] = "Secret Key";
	pokeram.item_names[44] = "?????";
	pokeram.item_names[45] = "Bike Voucher";
	pokeram.item_names[46] = "X Accuracy";
	pokeram.item_names[47] = "Leaf Stone";
	pokeram.item_names[48] = "Card Key";
	pokeram.item_names[49] = "Nugget";
	pokeram.item_names[50] = "PP Up*";
	pokeram.item_names[51] = "Poké Doll";
	pokeram.item_names[52] = "Full Heal";
	pokeram.item_names[53] = "Revive";
	pokeram.item_names[54] = "Max Revive";
	pokeram.item_names[55] = "Guard Spec.";
	pokeram.item_names[56] = "Super Repel";
	pokeram.item_names[57] = "Max Repel";
	pokeram.item_names[58] = "Dire Hit";
	pokeram.item_names[59] = "Coin";
	pokeram.item_names[60] = "Fresh Water";
	pokeram.item_names[61] = "Soda Pop";
	pokeram.item_names[62] = "Lemonade";
	pokeram.item_names[63] = "S.S. Ticket";
	pokeram.item_names[64] = "Gold Teeth";
	pokeram.item_names[65] = "X Attack";
	pokeram.item_names[66] = "X Defend";
	pokeram.item_names[67] = "X Speed";
	pokeram.item_names[68] = "X Special";
	pokeram.item_names[69] = "Coin Case";
	pokeram.item_names[70] = "Oak's Parcel";
	pokeram.item_names[71] = "Itemfinder";
	pokeram.item_names[72] = "Silph Scope";
	pokeram.item_names[73] = "Poké Flute";
	pokeram.item_names[74] = "Lift Key";
	pokeram.item_names[75] = "Exp. All";
	pokeram.item_names[76] = "Old Rod";
	pokeram.item_names[77] = "Good Rod";
	pokeram.item_names[78] = "Super Rod";
	pokeram.item_names[79] = "PP Up";
	pokeram.item_names[80] = "Ether";
	pokeram.item_names[81] = "Max Ether";
	pokeram.item_names[82] = "Elixer";
	pokeram.item_names[83] = "Max Elixer";
	pokeram.item_names[196] = "HM01";
	pokeram.item_names[197] = "HM02";
	pokeram.item_names[198] = "HM03";
	pokeram.item_names[199] = "HM04";
	pokeram.item_names[200] = "HM05";
	pokeram.item_names[201] = "TM01";
	pokeram.item_names[202] = "TM02";
	pokeram.item_names[203] = "TM03";
	pokeram.item_names[204] = "TM04";
	pokeram.item_names[205] = "TM05";
	pokeram.item_names[206] = "TM06";
	pokeram.item_names[207] = "TM07";
	pokeram.item_names[208] = "TM08";
	pokeram.item_names[209] = "TM09";
	pokeram.item_names[210] = "TM10";
	pokeram.item_names[211] = "TM11";
	pokeram.item_names[212] = "TM12";
	pokeram.item_names[213] = "TM13";
	pokeram.item_names[214] = "TM14";
	pokeram.item_names[215] = "TM15";
	pokeram.item_names[216] = "TM16";
	pokeram.item_names[217] = "TM17";
	pokeram.item_names[218] = "TM18";
	pokeram.item_names[219] = "TM19";
	pokeram.item_names[220] = "TM20";
	pokeram.item_names[221] = "TM21";
	pokeram.item_names[222] = "TM22";
	pokeram.item_names[223] = "TM23";
	pokeram.item_names[224] = "TM24";
	pokeram.item_names[225] = "TM25";
	pokeram.item_names[226] = "TM26";
	pokeram.item_names[227] = "TM27";
	pokeram.item_names[228] = "TM28";
	pokeram.item_names[229] = "TM29";
	pokeram.item_names[230] = "TM30";
	pokeram.item_names[231] = "TM31";
	pokeram.item_names[232] = "TM32";
	pokeram.item_names[233] = "TM33";
	pokeram.item_names[234] = "TM34";
	pokeram.item_names[235] = "TM35";
	pokeram.item_names[236] = "TM36";
	pokeram.item_names[237] = "TM37";
	pokeram.item_names[238] = "TM38";
	pokeram.item_names[239] = "TM39";
	pokeram.item_names[240] = "TM40";
	pokeram.item_names[241] = "TM41";
	pokeram.item_names[242] = "TM42";
	pokeram.item_names[243] = "TM43";
	pokeram.item_names[244] = "TM44";
	pokeram.item_names[245] = "TM45";
	pokeram.item_names[246] = "TM46";
	pokeram.item_names[247] = "TM47";
	pokeram.item_names[248] = "TM48";
	pokeram.item_names[249] = "TM49";
	pokeram.item_names[250] = "TM50";
	pokeram.item_names[251] = "TM51";
	pokeram.item_names[252] = "TM52";
	pokeram.item_names[253] = "TM53";
	pokeram.item_names[254] = "TM54";
	pokeram.item_names[255] = "TM55";

}

#endif