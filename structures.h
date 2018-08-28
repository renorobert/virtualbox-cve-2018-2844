
#define RT_OFFSETOF(type, member)  __builtin_offsetof (type, member)

typedef uint32_t HGSMISIZE;
typedef uint32_t HGSMIOFFSET;

/* defined in HGSMIDefs.h */

/* The buffer description flags. */
#define HGSMI_BUFFER_HEADER_F_SEQ_MASK     0x03 /* Buffer sequence type mask. */
#define HGSMI_BUFFER_HEADER_F_SEQ_SINGLE   0x00 /* Single buffer, not a part of a sequence. */
#define HGSMI_BUFFER_HEADER_F_SEQ_START    0x01 /* The first buffer in a sequence. */
#define HGSMI_BUFFER_HEADER_F_SEQ_CONTINUE 0x02 /* A middle buffer in a sequence. */
#define HGSMI_BUFFER_HEADER_F_SEQ_END      0x03 /* The last buffer in a sequence. */

#pragma pack(1) /** @todo not necessary. use AssertCompileSize instead. */
/* 16 bytes buffer header. */
typedef struct HGSMIBUFFERHEADER
{
	uint32_t    u32DataSize;            /* Size of data that follows the header. */

	uint8_t     u8Flags;                /* The buffer description: HGSMI_BUFFER_HEADER_F_* */

	uint8_t     u8Channel;              /* The channel the data must be routed to. */
	uint16_t    u16ChannelInfo;         /* Opaque to the HGSMI, used by the channel. */

	union {
		uint8_t au8Union[8];            /* Opaque placeholder to make the union 8 bytes. */

		struct
		{                               /* HGSMI_BUFFER_HEADER_F_SEQ_SINGLE */
			uint32_t u32Reserved1;      /* A reserved field, initialize to 0. */
			uint32_t u32Reserved2;      /* A reserved field, initialize to 0. */
		} Buffer;

		struct
		{                               /* HGSMI_BUFFER_HEADER_F_SEQ_START */
			uint32_t u32SequenceNumber; /* The sequence number, the same for all buffers in the sequence. */
			uint32_t u32SequenceSize;   /* The total size of the sequence. */
		} SequenceStart;

		struct
		{                               /* HGSMI_BUFFER_HEADER_F_SEQ_CONTINUE and HGSMI_BUFFER_HEADER_F_SEQ_END */
			uint32_t u32SequenceNumber; /* The sequence number, the same for all buffers in the sequence. */
			uint32_t u32SequenceOffset; /* Data offset in the entire sequence. */
		} SequenceContinue;
	} u;
} HGSMIBUFFERHEADER;

/* 8 bytes buffer tail. */
typedef struct HGSMIBUFFERTAIL
{
	uint32_t    u32Reserved;        /* Reserved, must be initialized to 0. */
	uint32_t    u32Checksum;        /* Verifyer for the buffer header and offset and for first 4 bytes of the tail. */
} HGSMIBUFFERTAIL;

/* defined in VBoxVideo.h */

# define VBVA_VDMA_CMD    11 /* G->H DMA command             */


typedef uint64_t VBOXVIDEOOFFSET;

/*
 * We can not submit the DMA command via VRAM since we do not have control over
 * DMA command buffer [de]allocation, i.e. we only control the buffer contents.
 * In other words the system may call one of our callbacks to fill a command buffer
 * with the necessary commands and then discard the buffer w/o any notification.
 *
 * We have only DMA command buffer physical address at submission time.
 *
 * so the only way is to */
typedef struct VBOXVDMACBUF_DR
{
	uint16_t fFlags;
	uint16_t cbBuf;
	/* RT_SUCCESS()     - on success
	 * VERR_INTERRUPTED - on preemption
	 * VERR_xxx         - on error */
	int32_t  rc;
	union
	{
		uint64_t phBuf;
		VBOXVIDEOOFFSET offVramBuf;
	} Location;
	uint64_t aGuestData[7];
} VBOXVDMACBUF_DR, *PVBOXVDMACBUF_DR;


#pragma pack(1)

/*
 * VBOXSHGSMI made on top HGSMI and allows receiving notifications
 * about G->H command completion
 */
/* SHGSMI command header */
typedef struct VBOXSHGSMIHEADER
{
	uint64_t pvNext;    /*<- completion processing queue */
	uint32_t fFlags;    /*<- see VBOXSHGSMI_FLAG_XXX Flags */
	uint32_t cRefs;     /*<- command referece count */
	uint64_t u64Info1;  /*<- contents depends on the fFlags value */
	uint64_t u64Info2;  /*<- contents depends on the fFlags value */
} VBOXSHGSMIHEADER, *PVBOXSHGSMIHEADER;

#define VBVA_ADAPTER_INFORMATION_SIZE 65536

#define VBVA_VDMA_CTL   	10 /* setup G<->H DMA channel info */
#define VBVA_VDMA_CMD    	11 /* G->H DMA command             */

/* VBOXVDMACBUF_DR::phBuf specifies offset in VRAM */
#define VBOXVDMACBUF_FLAG_BUF_VRAM_OFFSET 0x00000001
/* command buffer follows the VBOXVDMACBUF_DR in VRAM, VBOXVDMACBUF_DR::phBuf is ignored */
#define VBOXVDMACBUF_FLAG_BUF_FOLLOWS_DR  0x00000002


typedef enum
{
	VBOXVDMACMD_TYPE_UNDEFINED         = 0,
	VBOXVDMACMD_TYPE_DMA_PRESENT_BLT   = 1,
	VBOXVDMACMD_TYPE_DMA_BPB_TRANSFER,
	VBOXVDMACMD_TYPE_DMA_BPB_FILL,
	VBOXVDMACMD_TYPE_DMA_PRESENT_SHADOW2PRIMARY,
	VBOXVDMACMD_TYPE_DMA_PRESENT_CLRFILL,
	VBOXVDMACMD_TYPE_DMA_PRESENT_FLIP,
	VBOXVDMACMD_TYPE_DMA_NOP,
	VBOXVDMACMD_TYPE_CHROMIUM_CMD, /* chromium cmd */
	VBOXVDMACMD_TYPE_DMA_BPB_TRANSFER_VRAMSYS,
	VBOXVDMACMD_TYPE_CHILD_STATUS_IRQ /* make the device notify child (monitor) state change IRQ */
} VBOXVDMACMD_TYPE;


typedef struct VBOXVDMACMD
{
	VBOXVDMACMD_TYPE enmType;
	uint32_t u32CmdSpecific;
} VBOXVDMACMD, *PVBOXVDMACMD;

typedef struct VBOXVDMACMD_DMA_BPB_TRANSFER
{
	uint32_t cbTransferSize;
	uint32_t fFlags;
	union
	{
		uint64_t phBuf;
		VBOXVIDEOOFFSET offVramBuf;
	} Src;
	union
	{
		uint64_t phBuf;
		VBOXVIDEOOFFSET offVramBuf;
	} Dst;
} VBOXVDMACMD_DMA_BPB_TRANSFER, *PVBOXVDMACMD_DMA_BPB_TRANSFER;

/* defined in HGSMIChannels.h */

/* Predefined channel identifiers. Used internally by VBOX to simplify the channel setup. */
/* A reserved channel value */
#define HGSMI_CH_RESERVED     0x00
/* HGCMI: setup and configuration */
#define HGSMI_CH_HGSMI        0x01
/* Graphics: VBVA */
#define HGSMI_CH_VBVA         0x02
/* Graphics: Seamless with a single guest region */
#define HGSMI_CH_SEAMLESS     0x03
/* Graphics: Seamless with separate host windows */
#define HGSMI_CH_SEAMLESS2    0x04
/* Graphics: OpenGL HW acceleration */
#define HGSMI_CH_OPENGL       0x05

void InitializeHeader(HGSMIBUFFERHEADER *, HGSMISIZE, uint8_t, uint16_t, uint32_t);

