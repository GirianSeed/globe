#include "hipdef.h"
.data
.global head_top
.align 7
head_top:
.word SCE_HIG_VERSION, 0, 0, (head_end - head_top)/16
.ascii "Reflect_Fish"
.word Reflect_Fish_plug_top - head_top
.ascii	"Micro\0\0\0\0\0\0\0"
.word Micro_plug_top - head_top
head_end:


	
Reflect_Fish_plug_top:
.byte SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_REFLECT, SCE_HIG_PLUGIN_STATUS 
.word SCE_HIP_FISHEYE_PLUG | (SCE_HIP_REVISION<<24), 0, (Reflect_Fish_plug_end - Reflect_Fish_plug_top)/16
.byte 0x01, 0x00, 0, 0
.word  0, 0, 0x00000000
/* ins-plugin block list */
.byte SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_MICRO, SCE_HIG_PLUGIN_STATUS
.word SCE_HIP_MICRO_PLUG | (SCE_HIP_REVISION<<24), Micro_plug_top - head_top, 0
/* data block list */                    
Reflect_Fish_plug_end:
		

Micro_plug_top:
.byte SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_MICRO, SCE_HIG_PLUGIN_STATUS 
.word SCE_HIP_MICRO_PLUG | (SCE_HIP_REVISION<<24), 0, (Micro_plug_end - Micro_plug_top)/16
.byte 0x00, 0x01, 0, 0
.word  0, 0, 0x00000000
/* ins-plugin block list */
/* data block list */
.byte SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_MICRO, SCE_HIG_DATA_STATUS
.word SCE_HIP_MICRO_DATA | (SCE_HIP_REVISION<<24), Micro_data_top - head_top, 0
Micro_plug_end:

Micro_data_top:
.byte SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_MICRO, SCE_HIG_DATA_STATUS 
.word SCE_HIP_MICRO_DATA | (SCE_HIP_REVISION<<24), 0, (Micro_data_end - Micro_data_top)/16
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
.float 0.0,0.0,0.0,0.0
Micro_data_end:

