import sys
import struct
import subprocess
import re

def get_section_info(hex_file, section_name):
    """从 hex 文件中获取指定段的信息"""
    print(f"\nDebug: Looking for section {section_name}")
    
    # 使用 objdump 获取 hex 文件信息
    result = subprocess.run(['arm-none-eabi-objdump', '-h', hex_file], capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error running objdump command: {result.stderr}")
        return 0, 0, 0, 0
    
    objdump_output = result.stdout
    print("Debug: objdump output:")
    print(objdump_output)
    
    # 解析段信息
    lines = objdump_output.split('\n')
    for line in lines:
        if section_name in line:
            try:
                parts = line.split()
                size = int(parts[2], 16)  # Size
                vma = int(parts[3], 16)   # VMA
                lma = int(parts[4], 16)   # LMA
                
                vma_start = vma
                vma_end = vma + size
                lma_start = lma
                lma_end = lma + size
                
                print(f"Debug: Found section {section_name}:")
                print(f"  Size: 0x{size:08X}")
                print(f"  VMA: 0x{vma_start:08X} - 0x{vma_end:08X}")
                print(f"  LMA: 0x{lma_start:08X} - 0x{lma_end:08X}")
                return vma_start, vma_end, lma_start, lma_end
            except (IndexError, ValueError) as e:
                print(f"Debug: Error parsing line: {e}")
                continue
    
    print(f"Debug: Section {section_name} not found")
    return 0, 0, 0, 0

def main():
    if len(sys.argv) != 4:
        print("Usage: python generate_metadata.py <hex_file> <metadata_bin> <metadata_txt>")
        sys.exit(1)

    hex_file = sys.argv[1]
    metadata_bin = sys.argv[2]
    metadata_txt = sys.argv[3]
    
    # 获取各个段的信息
    text_vma_start, text_vma_end, text_lma_start, text_lma_end = get_section_info(hex_file, '.text')
    data_vma_start, data_vma_end, data_lma_start, data_lma_end = get_section_info(hex_file, '.data')
    bss_vma_start, bss_vma_end, bss_lma_start, bss_lma_end = get_section_info(hex_file, '.bss')
    rodata_vma_start, rodata_vma_end, rodata_lma_start, rodata_lma_end = get_section_info(hex_file, '.rodata')
    isr_vector_vma_start, isr_vector_vma_end, isr_vector_lma_start, isr_vector_lma_end = get_section_info(hex_file, '.isr_vector')

    # 写入文本文件
    with open(metadata_txt, 'w') as f:
        f.write(f"Magic: MTAD\n")
        f.write(f"\nText Section:\n")
        f.write(f"  VMA: 0x{text_vma_start:08X} - 0x{text_vma_end:08X}\n")
        f.write(f"  LMA: 0x{text_lma_start:08X} - 0x{text_lma_end:08X}\n")
        
        f.write(f"\nData Section:\n")
        f.write(f"  VMA: 0x{data_vma_start:08X} - 0x{data_vma_end:08X}\n")
        f.write(f"  LMA: 0x{data_lma_start:08X} - 0x{data_lma_end:08X}\n")
        
        f.write(f"\nBSS Section:\n")
        f.write(f"  VMA: 0x{bss_vma_start:08X} - 0x{bss_vma_end:08X}\n")
        f.write(f"  LMA: 0x{bss_lma_start:08X} - 0x{bss_lma_end:08X}\n")
        
        f.write(f"\nROData Section:\n")
        f.write(f"  VMA: 0x{rodata_vma_start:08X} - 0x{rodata_vma_end:08X}\n")
        f.write(f"  LMA: 0x{rodata_lma_start:08X} - 0x{rodata_lma_end:08X}\n")
        
        f.write(f"\nISR Vector Section:\n")
        f.write(f"  VMA: 0x{isr_vector_vma_start:08X} - 0x{isr_vector_vma_end:08X}\n")
        f.write(f"  LMA: 0x{isr_vector_lma_start:08X} - 0x{isr_vector_lma_end:08X}\n")

    # 写入二进制文件
    with open(metadata_bin, 'wb') as f:
        # Magic number: 'MTAD'
        f.write(b'MTAD')
        
        # Text section
        f.write(struct.pack('<II', text_vma_start, text_vma_end))
        f.write(struct.pack('<II', text_lma_start, text_lma_end))
        
        # Data section
        f.write(struct.pack('<II', data_vma_start, data_vma_end))
        f.write(struct.pack('<II', data_lma_start, data_lma_end))
        
        # BSS section
        f.write(struct.pack('<II', bss_vma_start, bss_vma_end))
        f.write(struct.pack('<II', bss_lma_start, bss_lma_end))
        
        # ROData section
        f.write(struct.pack('<II', rodata_vma_start, rodata_vma_end))
        f.write(struct.pack('<II', rodata_lma_start, rodata_lma_end))
        
        # ISR Vector section
        f.write(struct.pack('<II', isr_vector_vma_start, isr_vector_vma_end))
        f.write(struct.pack('<II', isr_vector_lma_start, isr_vector_lma_end))

if __name__ == '__main__':
    main() 