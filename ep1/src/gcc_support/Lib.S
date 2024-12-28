.syntax unified

.global __rt_memclr
.equ __rt_memclr, 0x03FEB6B4 | 0

.global AFW_AddAppToRoutingStack
.equ AFW_AddAppToRoutingStack, 0x101874B4 | 1

.global AFW_CreateInternalQueuedEvPriv
.equ AFW_CreateInternalQueuedEvPriv, 0x100B0578 | 1

.global AFW_GenAppInstanceId
.equ AFW_GenAppInstanceId, 0x10187604 | 1

.global AFW_GetEv
.equ AFW_GetEv, 0x100B0698 | 1

.global AFW_InquireRoutingStackByRegId
.equ AFW_InquireRoutingStackByRegId, 0x101878F4 | 1

.global APP_ConsumeEv
.equ APP_ConsumeEv, 0x10110BFA | 1

.global APP_Exit
.equ APP_Exit, 0x10110D12 | 1

.global APP_GetInstData
.equ APP_GetInstData, 0x10110DA2 | 1

.global APP_HandleEventPrepost
.equ APP_HandleEventPrepost, 0x10111194 | 1

.global APP_Register
.equ APP_Register, 0x1011159C | 1

.global APP_SyncML_MainRegister
.equ APP_SyncML_MainRegister, 0x10115620 | 1

.global DL_FsCloseFile
.equ DL_FsCloseFile, 0x100952CC | 1

.global DL_FsFSeekFile
.equ DL_FsFSeekFile, 0x10095528 | 1

.global DL_FsGetFileSize
.equ DL_FsGetFileSize, 0x100957CC | 1

.global DL_FsOpenFile
.equ DL_FsOpenFile, 0x10095230 | 1

.global DL_FsReadFile
.equ DL_FsReadFile, 0x10095442 | 1

.global strlen
.equ strlen, 0x10867408 | 1

.global suAllocMem
.equ suAllocMem, 0x1056D494 | 1

.global suFreeMem
.equ suFreeMem, 0x1056D642 | 1

.global suLogData
.equ suLogData, 0x10593444 | 1

.global u_atou
.equ u_atou, 0x103C3AE0 | 1

.global vsprintf
.equ vsprintf, 0x10867508 | 1
