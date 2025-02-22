import sys
import struct
import subprocess

# 定义魔数 (MTAD in little-endian)
METADATA_MAGIC = ord('D') | (ord('A') << 8) | (ord('T') << 16) | (ord('M') << 24)  # 0x4D544144

def get_section_info(elf_file, section_name):
    """从 ELF 文件中获取指定段的信息"""
    print(f"\nDebug: Looking for section {section_name}")
    
    # 使用 readelf 获取段信息，包括 LMA
    result = subprocess.run(['arm-none-eabi-readelf', '-S', '-W', elf_file], capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error running readelf command: {result.stderr}")
        return 0, 0, 0, 0
    
    readelf_output = result.stdout
    print("Debug: readelf output:")
    print(readelf_output)
    
    # 解析段信息
    lines = readelf_output.split('\n')
    for i, line in enumerate(lines):
        if section_name in line:
            try:
                # readelf -S 输出格式示例:
                # [Nr] Name         Type     Address   Off    Size   ES Flg Lk Inf Al
                # [ 1] .text       PROGBITS 30000000 010000 001234 00  AX  0   0  4
                # Load to 90000000
                parts = line.split()
                addr = int(parts[3], 16)  # VMA
                size = int(parts[5], 16)  # Size
                
                # 查找 LMA 信息
                lma = addr  # 默认 LMA = VMA
                for j in range(i+1, min(i+4, len(lines))):
                    if "Load to" in lines[j]:
                        lma = int(lines[j].split()[-1], 16)
                        break
                
                vma_start = addr
                vma_end = addr + size
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
    
    # 如果没找到，尝试使用 objdump 获取更多信息
    result = subprocess.run(['arm-none-eabi-objdump', '-h', '-w', elf_file], capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error running objdump command: {result.stderr}")
        return 0, 0, 0, 0
        
    objdump_output = result.stdout
    print("Debug: objdump output:")
    print(objdump_output)
    
    # 解析 objdump 输出
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
                
                print(f"Debug: Found section {section_name} (from objdump):")
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
        print("Usage: python generate_metadata.py <elf_file> <metadata_bin> <metadata_txt>")
        sys.exit(1)

    elf_file = sys.argv[1]
    metadata_bin = sys.argv[2]
    metadata_txt = sys.argv[3]
    
    # 获取各个段的信息（按照结构体字段的顺序）
    sections = [
        ('.text', get_section_info(elf_file, '.text')),          # text
        ('.data', get_section_info(elf_file, '.data')),          # data
        ('.bss', get_section_info(elf_file, '.bss')),            # bss
        ('.rodata', get_section_info(elf_file, '.rodata')),      # rodata
        ('.isr_vector', get_section_info(elf_file, '.isr_vector')) # isr_vector
    ]

    # 写入文本文件
    with open(metadata_txt, 'w') as f:
        f.write(f"Magic: MTAD\n")
        for name, (vma_start, vma_end, lma_start, lma_end) in sections:
            f.write(f"\n{name} Section:\n")
            f.write(f"  VMA: 0x{vma_start:08X} - 0x{vma_end:08X}\n")
            f.write(f"  LMA: 0x{lma_start:08X} - 0x{lma_end:08X}\n")

    # 写入二进制文件
    with open(metadata_bin, 'wb') as f:
        # Magic number: 'MTAD' (写入为小端序: 'DATM')
        f.write(struct.pack('<I', METADATA_MAGIC))  # 0x4D544144 -> 44 41 54 4D
        
        # 写入所有段的信息
        for _, (vma_start, vma_end, lma_start, lma_end) in sections:
            f.write(struct.pack('<IIII',  # '<' 表示小端序
                vma_start, vma_end,
                lma_start, lma_end))

if __name__ == '__main__':
    main() 