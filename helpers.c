#include <stdint.h>
#include <sys/types.h>
#include "structures.h"

static uint32_t hgsmiHashProcess(uint32_t hash,
		const void *pvData,
		size_t cbData)
{
	const uint8_t *pu8Data = (const uint8_t *)pvData;

	while (cbData--)
	{
		hash += *pu8Data++;
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	return hash;
}

static uint32_t hgsmiHashEnd(uint32_t hash)
{
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash;
}

uint32_t HGSMIChecksum(HGSMIOFFSET offBuffer,
		const HGSMIBUFFERHEADER *pHeader,
		const HGSMIBUFFERTAIL *pTail)
{
	uint32_t u32Checksum = 0;

	u32Checksum = hgsmiHashProcess(u32Checksum, &offBuffer, sizeof(offBuffer));
	u32Checksum = hgsmiHashProcess(u32Checksum, pHeader, sizeof(HGSMIBUFFERHEADER));
	u32Checksum = hgsmiHashProcess(u32Checksum, pTail, RT_OFFSETOF(HGSMIBUFFERTAIL, u32Checksum));
	return hgsmiHashEnd(u32Checksum);
}

void InitializeHeader(HGSMIBUFFERHEADER *pHeader, HGSMISIZE cbBuffer, uint8_t u8Channel, 
		uint16_t u16ChannelInfo, uint32_t offBuffer)
{
	pHeader->u8Flags        = HGSMI_BUFFER_HEADER_F_SEQ_SINGLE;
	pHeader->u32DataSize    = cbBuffer;
	pHeader->u8Channel      = u8Channel;
	pHeader->u16ChannelInfo = u16ChannelInfo;

	HGSMIBUFFERTAIL *pTail = (struct HGSMIBUFFERTAIL *)((uint64_t)pHeader + cbBuffer +  sizeof(HGSMIBUFFERHEADER));

	pTail->u32Reserved = 0;
	pTail->u32Checksum = HGSMIChecksum(offBuffer, pHeader, pTail);
}

