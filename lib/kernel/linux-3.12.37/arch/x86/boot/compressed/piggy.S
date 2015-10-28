.section ".rodata..compressed","a",@progbits
.globl z_input_len
z_input_len = 4853091
.globl z_output_len
z_output_len = 10221540
.globl z_extract_offset
z_extract_offset = 0x530000
.globl z_extract_offset_negative
z_extract_offset_negative = -0x530000
.globl input_data, input_data_end
input_data:
.incbin "arch/x86/boot/compressed/vmlinux.bin.gz"
input_data_end:
