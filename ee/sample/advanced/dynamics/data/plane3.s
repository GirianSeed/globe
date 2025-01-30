#include "def.h"
.data
.align 7
head_top:
.word SCE_HIG_VERSION, 0, 0, (head_end - head_top) / 16
.ascii "Frame\0\0\0\0\0\0\0"
.word Frame_plug_top - head_top
.ascii "Micro\0\0\0\0\0\0\0"
.word Micro_plug_top - head_top
.ascii "Shape\0\0\0\0\0\0\0"
.word Shape_plug_top - head_top
.ascii "Hrchy\0\0\0\0\0\0\0"
.word Hrchy_plug_top - head_top
head_end:
Frame_plug_top:
.byte SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_FRAME, SCE_HIG_PLUGIN_STATUS
.word SCE_HIP_FRAME_PLUG | (SCE_HIP_REVISION << 24), 0, (Frame_plug_end - Frame_plug_top) / 16
.byte 0x03, 0x00, 0x00, 0x00
.word 0x00000000, 0x00000000, 0x00000000
.byte SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_MICRO, SCE_HIG_PLUGIN_STATUS
.word SCE_HIP_MICRO_PLUG | (SCE_HIP_REVISION << 24), Micro_plug_top - head_top, 0
.byte SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_SHAPE, SCE_HIG_PLUGIN_STATUS
.word SCE_HIP_SHAPE_PLUG | (SCE_HIP_REVISION << 24), Shape_plug_top - head_top, 0
.byte SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_HRCHY, SCE_HIG_PLUGIN_STATUS
.word SCE_HIP_HRCHY_PLUG | (SCE_HIP_REVISION << 24), Hrchy_plug_top - head_top, 0
Frame_plug_end:
Shape_plug_top:
.byte SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_SHAPE, SCE_HIG_PLUGIN_STATUS
.word SCE_HIP_SHAPE_PLUG | (SCE_HIP_REVISION << 24), 0, (Shape_plug_end - Shape_plug_top) / 16
.byte 0x00, 0x02, 0x00, 0x00
.word 0x00000000, 0x00000000, 0x00000000
.byte SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_SHAPE, SCE_HIG_DATA_STATUS
.word SCE_HIP_SHAPE_DATA | (SCE_HIP_REVISION << 24), Shape_data_top - head_top, 0
.byte SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_SHAPE, SCE_HIG_DATA_STATUS
.word SCE_HIP_BASEMATRIX | (SCE_HIP_REVISION << 24), Basematrix_data_top - head_top, 0
Shape_plug_end:
Micro_plug_top:
.byte SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_MICRO, SCE_HIG_PLUGIN_STATUS
.word SCE_HIP_MICRO_PLUG | (SCE_HIP_REVISION << 24), 0, (Micro_plug_end - Micro_plug_top) / 16
.byte 0x00, 0x01, 0x00, 0x00
.word 0x00000000, 0x00000000, 0x00000000
.byte SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_MICRO, SCE_HIG_DATA_STATUS
.word SCE_HIP_MICRO_DATA | (SCE_HIP_REVISION << 24), Micro_data_top - head_top, 0
Micro_plug_end:
Hrchy_plug_top:
.byte SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_HRCHY, SCE_HIG_PLUGIN_STATUS
.word SCE_HIP_HRCHY_PLUG | (SCE_HIP_REVISION << 24), 0, (Hrchy_plug_end - Hrchy_plug_top) / 16
.byte 0x00, 0x03, 0x00, 0x00
.word 0x00000000, 0x00000000, 0x00000000
.byte SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_HRCHY, SCE_HIG_DATA_STATUS
.word SCE_HIP_HRCHY_DATA | (SCE_HIP_REVISION << 24), Hrchy_data_top - head_top, 0
.byte SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_SHAPE, SCE_HIG_DATA_STATUS
.word SCE_HIP_BASEMATRIX | (SCE_HIP_REVISION << 24), Basematrix_data_top - head_top, 0
.byte SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_HRCHY, SCE_HIG_DATA_STATUS
.word SCE_HIP_PIVOT_DATA | (SCE_HIP_REVISION << 24), Pivot_data_top - head_top, 0
Hrchy_plug_end:
Shape_data_top:
.byte SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_SHAPE, SCE_HIG_DATA_STATUS
.word SCE_HIP_SHAPE_DATA | (SCE_HIP_REVISION << 24), 0, (Shape_data_end - Shape_data_top) / 16
.word 0x00000000, 0x00000000, 0x00000000, 0x00000001
.word 0x00000000, 0x00000070, 0x00000000, 0x00000001
.word 0x00000000, 0x00000001, 0x00000000, 0x00000000
.word 0x00000000, 0x00000000, 0x00000000, 0x00000000
.word 0x00000000, 0x00000000, 0x00000000, 0x00000000
.word 0x00000000, 0x00000060, 0x0000380b, 0x00000006
.float -10.000000, 0.000000, -10.000000, 1.000000 
.float -10.000000, 0.000000, 10.000000, 1.000000 
.float 10.000000, 0.000000, 10.000000, 1.000000 
.float -10.000000, 0.000000, -10.000000, 1.000000 
.float 10.000000, 0.000000, 10.000000, 1.000000 
.float 10.000000, 0.000000, -10.000000, 1.000000 
.float 0.000000, -1.000000, 0.000000, 1.000000 
.float 0.000000, -1.000000, 0.000000, 1.000000 
.float 0.000000, -1.000000, 0.000000, 1.000000 
.float 0.000000, -1.000000, 0.000000, 1.000000 
.float 0.000000, -1.000000, 0.000000, 1.000000 
.float 0.000000, -1.000000, 0.000000, 1.000000 
.float 0.000000, 0.000000, 1.000000, 0.000000 
.float 0.000000, 0.000000, 1.000000, 0.000000 
.float 0.000000, 0.000000, 1.000000, 0.000000 
.float 0.000000, 0.000000, 1.000000, 0.000000 
.float 0.000000, 0.000000, 1.000000, 0.000000 
.float 0.000000, 0.000000, 1.000000, 0.000000 
.float 200.000000, 200.000000, 200.000000, 128.000000 
.float 200.000000, 200.000000, 200.000000, 128.000000 
.float 200.000000, 200.000000, 200.000000, 128.000000 
.float 200.000000, 200.000000, 200.000000, 128.000000 
.float 200.000000, 200.000000, 200.000000, 128.000000 
.float 200.000000, 200.000000, 200.000000, 128.000000 
Shape_data_end:
Basematrix_data_top:
.byte SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_SHAPE, SCE_HIG_DATA_STATUS
.word SCE_HIP_BASEMATRIX | (SCE_HIP_REVISION << 24), 0, (Basematrix_data_end - Basematrix_data_top) / 16
.word 0x00000000, 0x00000000, 0x00000001, 0x00000001
.word 0x00000000, 0x00000000, 0x00000000, 0x00000000
.float 0.98480775301, 0.17364817767, 0.000000, 0.000000
.float -0.17364817767,0.98480775301, 0.000000, 0.0 
.float 0.000000, 0.000000, 1.000000, 0.0 
.float 0.000000, 5.201000, 3.000000, 1.000000
.float 0.98480775301, 0.17364817767, 0.000000, 0.000000
.float -0.17364817767,0.98480775301, 0.000000, 0.0           
.float 0.000000, 0.000000, 1.000000, 0.000000
.float 0.000000, 0.000000, 0.000000, 1.000000          
Basematrix_data_end:
Micro_data_top:
.byte SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_MICRO, SCE_HIG_DATA_STATUS
.word SCE_HIP_MICRO_DATA | (SCE_HIP_REVISION << 24), 0, (Micro_data_end - Micro_data_top) / 16
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
Micro_data_end:
Hrchy_data_top:
.byte SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_HRCHY, SCE_HIG_DATA_STATUS
.word SCE_HIP_HRCHY_DATA | (SCE_HIP_REVISION << 24), 0, (Hrchy_data_end - Hrchy_data_top) / 16
.word 0x00000000, 0x00000000, 0x00000101, 0x00000001
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 0.000000, 0.000000, 0.000000, 0.000000 
.float 1.000000, 1.000000, 1.000000, 0.000000 
.word 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff
Hrchy_data_end:
Pivot_data_top:
.byte SCE_HIP_COMMON, SCE_HIP_FRAMEWORK, SCE_HIP_HRCHY, SCE_HIG_DATA_STATUS
.word SCE_HIP_PIVOT_DATA | (SCE_HIP_REVISION << 24), 0, (Pivot_data_end - Pivot_data_top) / 16
.word 0x00000000, 0x00000000, 0x00000000, 0x00000001
.float 0.000000, 0.000000, 0.000000, 1.000000 
Pivot_data_end:
