import sys
import struct
import subprocess
import io

# 定义魔数 (MTAD in little-endian)
METADATA_MAGIC = ord('D') | (ord('A') << 8) | (ord('T') << 16) | (ord('M') << 24)  # 0x4D544144

def get_section_info(elf_file, section_name):
    """从 ELF 文件中获取指定段的信息"""
    # 使用 objdump 获取段信息
    result = subprocess.run([
        'arm-none-eabi-objdump',
        '-h', '-w',
        elf_file
    ], capture_output=True, text=True)
    
    if result.returncode != 0:
        return 0, 0, 0, 0
        
    # 解析 objdump 输出
    lines = result.stdout.split('\n')
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
                
                return vma_start, vma_end, lma_start, lma_end
            except (IndexError, ValueError) as e:
                continue
    
    return 0, 0, 0, 0

def print_vector_table(elf_file):
    """打印 ELF 文件中的中断向量表前16个条目"""
    print("\nVector Table Contents from ELF:")
    
    # 使用 objcopy 先提取 .isr_vector 段到临时文件
    temp_file = "temp_vector.bin"
    result = subprocess.run([
        'arm-none-eabi-objcopy',
        '--only-section=.isr_vector',
        '-O', 'binary',
        elf_file,
        temp_file
    ], capture_output=True)
    
    if result.returncode != 0:
        print("Error extracting vector table")
        return
    
    # 读取二进制数据
    try:
        with open(temp_file, 'rb') as f:
            data = []
            for i in range(16):
                value = struct.unpack('<I', f.read(4))[0]
                data.append(value)
    except:
        print("Error reading vector table")
        return
    finally:
        import os
        if os.path.exists(temp_file):
            os.remove(temp_file)
    
    vector_names = [
        "Initial_SP",
        "Reset_Handler",
        "NMI_Handler",
        "HardFault_Handler",
        "MemManage_Handler",
        "BusFault_Handler",
        "UsageFault_Handler",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "SVC_Handler",
        "DebugMon_Handler",
        "Reserved",
        "PendSV_Handler",
        "SysTick_Handler"
    ]
    
    # 打印向量表
    for i in range(16):
        print(f"Vector {i:2d} ({vector_names[i]:20s}): 0x{data[i]:08X}")

def dump_elf_header(elf_file):
    """打印 ELF 文件的头部信息"""
    result = subprocess.run([
        'arm-none-eabi-readelf',
        '-h',  # 显示 ELF 头
        elf_file
    ], capture_output=True, text=True)
    print("\nELF Header:")
    print(result.stdout)

def dump_binary_header(bin_file, size=64):
    """打印二进制文件的头部内容"""
    with open(bin_file, 'rb') as f:
        data = f.read(size)
    print("\nBinary file header:")
    for i in range(0, len(data), 16):
        hex_data = ' '.join(f'{b:02X}' for b in data[i:i+16])
        print(f'{i:04X}: {hex_data}')

def dump_metadata_content(metadata_bin):
    """打印元数据的详细内容（十六进制格式）"""
    print("\nMetadata Content (hex):")
    with open(metadata_bin, 'rb') as f:
        data = f.read()
    
    # 解析并打印结构化数据
    print("\nParsed structure:")
    f = io.BytesIO(data)
    
    # 魔数
    magic = struct.unpack('<I', f.read(4))[0]
    print(f"Magic: 0x{magic:08X} ('{chr(magic & 0xFF)}{chr((magic >> 8) & 0xFF)}{chr((magic >> 16) & 0xFF)}{chr((magic >> 24) & 0xFF)}')")
    
    # 段信息
    section_names = ['.isr_vector', '.text', '.data', '.bss', '.rodata']
    for name in section_names:
        values = struct.unpack('<IIII', f.read(16))
        print(f"\n{name}:")
        print(f"  VMA: 0x{values[0]:08X} - 0x{values[1]:08X}")
        print(f"  LMA: 0x{values[2]:08X} - 0x{values[3]:08X}")

def dump_file_content(filename, size=64, title="File Content"):
    """通用的文件内容打印函数（十六进制格式）"""
    print(f"\n{title} (first {size} bytes):")
    try:
        with open(filename, 'rb') as f:
            data = f.read(size)
            for i in range(0, len(data), 16):
                hex_data = ' '.join(f'{b:02X}' for b in data[i:i+16])
                ascii_data = ''.join(chr(b) if 32 <= b <= 126 else '.' for b in data[i:i+16])
                print(f'{i:04X}: {hex_data:<48} |{ascii_data}|')
    except Exception as e:
        print(f"Error reading file: {e}")

def main():
    if len(sys.argv) != 4:
        print("Usage: python generate_metadata.py <elf_file> <metadata_bin> <metadata_txt>")
        sys.exit(1)

    elf_file = sys.argv[1]
    metadata_bin = sys.argv[2]
    metadata_txt = sys.argv[3]
    
    # 打印 ELF 文件内容
    # dump_file_content(elf_file, 64, "ELF File Content")
    
    # 打印 ELF 头信息
    # dump_elf_header(elf_file)
    
    # 打印中断向量表内容
    print_vector_table(elf_file)
    
    # 获取各个段的信息（按照结构体字段的顺序）
    sections = [
        ('.isr_vector', get_section_info(elf_file, '.isr_vector')), # 必须在最前面
        ('.text', get_section_info(elf_file, '.text')),
        ('.data', get_section_info(elf_file, '.data')),
        ('.bss', get_section_info(elf_file, '.bss')),
        ('.rodata', get_section_info(elf_file, '.rodata'))
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

    # 打印元数据内容
    dump_metadata_content(metadata_bin)

if __name__ == '__main__':
    main() 