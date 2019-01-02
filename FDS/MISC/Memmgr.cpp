#include "Base/FDS_DEFS.H"
#include "Base/FDS_VARS.H"
#include "Base/FDS_DECS.H"

#include <map>

using namespace std;
map<mword, mword> g_AlignedBlockMap;

void *getAlignedBlock(mword size, mword alignment)
{
	mword addr = (dword)malloc(size+alignment);
	mword aligned = (addr + alignment-1)&(~(alignment-1));
	
	//g_AlignedBlockMap.insert(pair<mword, mword> ();
	g_AlignedBlockMap[aligned] = addr;
	return (void *)aligned;
}

void freeAlignedBlock(void *ptr)
{
	mword aligned = (mword)ptr;
	bool exists = g_AlignedBlockMap.find(aligned) != g_AlignedBlockMap.end();
	mword addr;
	if (exists)
	{
		addr = g_AlignedBlockMap[aligned];
	} else {
		addr = aligned;
	}

	if (!addr) return;
	free((void *)addr);
}
