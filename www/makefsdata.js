import { join, dirname, normalize, relative } from 'node:path';
import fs from 'node:fs';

import { fileURLToPath } from 'node:url';

import pako from 'pako';
const __makeExFile = true;	//是否将文件内容生成外部文件
const __useCompression = true; // 是否使用压缩功能
const __filename = fileURLToPath(import.meta.url);

const root = dirname(__filename).replace(normalize('www'), '');
const rootwww = dirname(__filename);
const buildPath = join(rootwww, 'build');
const ex_fsdata_addr = 0x00000000;			// 读取外部fsdata文件在qspiflash中的地址
const memory_section = '._Text_Area';		// 运行时保存fsdata数据所用的 RAM section，需要在ld文件中定义
const fsdataPath = normalize(join(root, 'Libs/httpd/fsdata.c'));
const exfilePath = normalize(join(root, 'Libs/httpd/ex_fsdata.bin'));
const extnames = ['.html', '.htm', '.shtml', '.shtm', '.ssi', '.xml', '.json', '.js', '.css', '.svg', '.ico', '.png', '.jpg', '.jpeg', '.bmp', '.gif'];
const includes_files = [ // 需要包含的文件
	/index(\.[a-z|A-Z|0-9|]*)?\.html/, 
	/favicon(\.[a-z|A-Z|0-9|]*)?\.ico/, 
	/main\-app(\.[a-z|A-Z|0-9|]*)?\.js/, 
	/layout(\.[a-z|A-Z|0-9|]*)?\.js/, 
	/page(\.[a-z|A-Z|0-9|]*)?\.js/
];

// These are the same content types that are used by the original makefsdata
const contentTypes = new Map([
	['html', 'text/html'],
	['htm', 'text/html'],
	[
		'shtml',
		'text/html\r\nExpires: Fri, 10 Apr 2008 14:00:00 GMT\r\nPragma: no-cache',
	],
	[
		'shtm',
		'text/html\r\nExpires: Fri, 10 Apr 2008 14:00:00 GMT\r\nPragma: no-cache',
	],
	[
		'ssi',
		'text/html\r\nExpires: Fri, 10 Apr 2008 14:00:00 GMT\r\nPragma: no-cache',
	],
	['gif', 'image/gif'],
	['png', 'image/png'],
	['jpg', 'image/jpeg'],
	['bmp', 'image/bmp'],
	['ico', 'image/x-icon'],
	['class', 'application/octet-stream'],
	['cls', 'application/octet-stream'],
	['js', 'application/javascript'],
	['ram', 'application/javascript'],
	['css', 'text/css'],
	['swf', 'application/x-shockwave-flash'],
	['xml', 'text/xml'],
	['xsl', 'text/xml'],
	['pdf', 'application/pdf'],
	['json', 'application/json'],
]);

const defaultContentType = 'text/plain';

const shtmlExtensions = new Set(['shtml', 'shtm', 'ssi', 'xml', 'json']);

const skipCompressionExtensions = new Set(['png', 'json']);

const serverHeader = 'IONIX-Hitbox';

const payloadAlignment = 4;
const hexBytesPerLine = 16;

function getFiles(dir) {
	let results = [];
	const list = fs.readdirSync(dir, { withFileTypes: true });
	// 遍历目录下的所有文件和子目录
	for (const file of list) {
		file.path = dir + '/' + file.name;
		// 如果是目录，则递归遍历
		if (file.isDirectory()) {
			results = results.concat(getFiles(file.path));
		} else if (file.isFile()) {
			// 如果是文件，则检查文件扩展名
			const ext = '.' + getLowerCaseFileExtension(file.name);
			if (extnames.includes(ext) && includes_files.some(pattern => pattern.test(file.name))) {
				results.push(file.path);
			}
		}
	}
	return results;
}

function getLowerCaseFileExtension(file) {
	const ext = file.split('.').pop();
	return ext ? ext.toLowerCase() : '';
}

function fixFilenameForC(filename) {
	let varName = '';
	for (let i = 0; i < filename.length; i++) {
		const c = filename[i];
		if (c >= 'a' && c <= 'z') {
			varName += c;
		} else if (c >= 'A' && c <= 'Z') {
			varName += c;
		} else if (c >= '0' && c <= '9') {
			varName += c;
		} else {
			varName += '_';
		}
	}
	return varName;
}

function CStringLength(str) {
	return Buffer.from(str, 'utf8').length;
}

function createHexString(value, prependComment = false) {
	let column = 0;
	let hexString = '';
	if (typeof value === 'string') {
		const bytes = Buffer.from(value, 'utf8');
		if (prependComment) {
			hexString += `/* "${value}" (${bytes.length} bytes) */\n`;
		}
		bytes.forEach((byte) => {
			hexString += '0x' + byte.toString(16).padStart(2, '0') + ',';
			if (++column >= hexBytesPerLine) {
				hexString += '\n';
				column = 0;
			}
		});
	} else if (Buffer.isBuffer(value) || value instanceof Uint8Array) {
		value.forEach((byte) => {
			hexString += '0x' + byte.toString(16).padStart(2, '0') + ',';
			if (++column >= hexBytesPerLine) {
				hexString += '\n';
				column = 0;
			}
		});
	} else {
		throw new Error('Invalid value type');
	}

	return hexString + '\n';
}

function createFileData(value) {
	if(typeof value === 'string') {
		const bytes = Buffer.from(value, 'utf8');
		return bytes;
	} else if(Buffer.isBuffer(value) || value instanceof Uint8Array) {
		return value;
	} else {
		return null;
	}
}

function concatenateArrayBuffers(buffers) {

	if(Array.isArray(buffers) == false) {
		console.log("buffers is not array: ", buffers)
		return;
	}

	// console.log(buffers)

	const infoArr = [];
	let bytelen = 0;
	buffers.forEach(buffer => {
		if(buffer && (Buffer.isBuffer(buffer) || buffer instanceof Uint8Array)) {
			bytelen += buffer.byteLength;
		}
	});

	let idx = 0; 
	const tmp = new Uint8Array(bytelen);
	
	buffers.forEach(buffer => {
		if(buffer && (Buffer.isBuffer(buffer) || buffer instanceof Uint8Array)) {
			// console.log('buffers.foreach: ', buffer, idx)
			tmp.set(new Uint8Array(buffer), idx);
			// console.log('tmp: ', tmp, tmp.buffer, idx)
			infoArr.push({
				start: idx,
				size: buffer.byteLength,
			});

			idx += buffer.byteLength;
		}
	});

	return {
		info: infoArr,
		buffer: Buffer.from(tmp),
	};
}

function makeFileBuffer(paddedQualifiedName, ext, isCompressed, fileContent, compressed = null) {
	// Check if we should compress this file
	if (__useCompression && !skipCompressionExtensions.has(ext)) {
		try {
			// Compress the content using pako
			const compressedContent = pako.deflate(fileContent, {
				level: 9,
				windowBits: 15,
				memLevel: 9,
			});
			isCompressed = true;
			compressed = compressedContent;
		} catch (error) {
			console.warn(`Warning: Compression failed for ${paddedQualifiedName}, using uncompressed version`);
			isCompressed = false;
			compressed = null;
		}
	} else {
		isCompressed = false;
		compressed = null;
	}

	let buffer = concatenateArrayBuffers([
		createFileData(paddedQualifiedName),
		createFileData('HTTP/1.0 200 OK\r\n'),
		createFileData(`Server: ${serverHeader}\r\n`),
		createFileData(`Content-Length: ${isCompressed ? compressed.byteLength : fileContent.byteLength}\r\n`),
		isCompressed ? createFileData('Content-Encoding: deflate\r\n') : null,
		createFileData(`Content-Type: ${contentTypes.get(ext) ?? defaultContentType}\r\n\r\n`),
		isCompressed ? compressed : fileContent
	]).buffer;

	return buffer;
}

function makeAllFileData(buffers) {
	/**
	 * first: [ length, size0, size1, size2, ... ]
	 */

	const sizeArr = buffers.map(buffer => buffer.byteLength);
	sizeArr.unshift(buffers.length);
	const uint32a = new DataView(new ArrayBuffer(sizeArr.length * 4));
	sizeArr.forEach((value, index) => uint32a.setUint32(index * 4, value, false));	//stm32都是littleEndian
	const firstBuffer = Buffer.from(uint32a.buffer);
	buffers.unshift(firstBuffer);
	return concatenateArrayBuffers( buffers );
}

function makefsdata() {
	let fsdata = '';
	fsdata += '#include "constant.hpp"\n';
	fsdata += '#include "fsdata.h"\n';
	fsdata += '#include "qspi-w25q64.h"\n';
	fsdata += '#include <stdbool.h>\n';
	fsdata += '#include <string.h>\n';
	fsdata += '#include <stdlib.h>\n\n';
	fsdata += '#define file_NULL (struct fsdata_file *) NULL\n\n';
	fsdata += '#ifndef FS_FILE_FLAGS_HEADER_INCLUDED\n';
	fsdata += '#define FS_FILE_FLAGS_HEADER_INCLUDED 1\n';
	fsdata += '#endif\n';
	fsdata += '#ifndef FS_FILE_FLAGS_HEADER_PERSISTENT\n';
	fsdata += '#define FS_FILE_FLAGS_HEADER_PERSISTENT 0\n';
	fsdata += '#endif\n';
	fsdata += '#ifndef FSDATA_FILE_ALIGNMENT\n';
	fsdata += '#define FSDATA_FILE_ALIGNMENT 0\n';
	fsdata += '#endif\n';
	fsdata += '#ifndef FSDATA_ALIGN_PRE\n';
	fsdata += '#define FSDATA_ALIGN_PRE\n';
	fsdata += '#endif\n';
	fsdata += '#ifndef FSDATA_ALIGN_POST\n';
	fsdata += '#define FSDATA_ALIGN_POST\n';
	fsdata += '#endif\n\n';
	fsdata += '#define ex_fsdata_addr FSDATA_ADDR\n\n';
	
	// 添加RAM区域定义
	fsdata += '// 定义RAM区域的起始地址和大小（根据链接器脚本中的定义）\n';
	fsdata += '#define RAM_START_ADDR      0x24000000\n';
	fsdata += '#define RAM_SIZE           (512 * 1024)  // 512KB\n';
	fsdata += '#define RAM_ALIGNMENT      32\n\n';
	
	// 添加RAM分配器
	fsdata += '// 用于跟踪RAM分配的简单分配器\n';
	fsdata += 'static uint32_t current_ram_addr = RAM_START_ADDR;\n\n';
	
	// 添加文件数据指针声明
	fsdata += '// 文件数据指针\n';
	const fileInfos = [];
	const fileDataBuffers = [];
	
	getFiles(buildPath).forEach((file) => {
		const qualifiedName = '/' + relative(buildPath, file).replace(/\\/g, '/');
		const varName = fixFilenameForC(qualifiedName);
		const qualifiedNameLength = CStringLength(qualifiedName) + 1;
		const paddedQualifiedNameLength =
			Math.ceil(qualifiedNameLength / payloadAlignment) * payloadAlignment;
		// Zero terminate the qualified name and pad it to the next multiple of payloadAlignment
		const paddedQualifiedName =
			qualifiedName +
			'\0'.repeat(1 + paddedQualifiedNameLength - qualifiedNameLength);
		fsdata += `static uint8_t* data_${varName} = NULL;\n`;
		
		// Read file content and create buffer
		const fileContent = fs.readFileSync(file);
		const ext = getLowerCaseFileExtension(file);
		
		// Create file buffer with compression if enabled
		const fileBuffer = makeFileBuffer(
			paddedQualifiedName,
			ext,
			false,  // This will be updated inside makeFileBuffer if compression is used
			fileContent
		);
		
		fileDataBuffers.push(fileBuffer);
		
		fileInfos.push({
			varName,
			paddedQualifiedNameLength,
			isSsiFile: shtmlExtensions.has(ext),
			size: fileBuffer.byteLength,
		});
	});
	fsdata += '\n';
	
	// 添加文件大小常量
	fsdata += '// 文件大小常量\n';
	fileInfos.forEach(info => {
		fsdata += `#define SIZE_${info.varName.toUpperCase()} ${info.size}\n`;
	});
	fsdata += '\n';
	
	fsdata += 'static bool fsdata_inited = false;\n\n';
	
	// 添加RAM分配函数
	fsdata += `// 简单的内存分配函数，返回对齐的地址
static void* ram_alloc(size_t size) {
	// 确保大小是32字节对齐的
	size = (size + RAM_ALIGNMENT - 1) & ~(RAM_ALIGNMENT - 1);
	
	// 检查是否有足够的空间
	if (current_ram_addr + size > RAM_START_ADDR + RAM_SIZE) {
		return NULL;
	}
	
	// 保存当前地址
	void* allocated_addr = (void*)current_ram_addr;
	
	// 更新下一个可用地址
	current_ram_addr += size;
	
	return allocated_addr;
}\n\n`;
	
	// 添加文件结构体定义
	let prevFile = 'NULL';
	fileInfos.forEach((info) => {
		fsdata += `struct fsdata_file file_${info.varName}[] = {{\n`;
		fsdata += `    file_${prevFile},\n`;
		fsdata += `    NULL,  // 将在运行时设置\n`;
		fsdata += `    NULL,  // 将在运行时设置\n`;
		fsdata += `    SIZE_${info.varName.toUpperCase()} - ${info.paddedQualifiedNameLength},\n`;
		fsdata += `    FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT\n`;
		fsdata += `}};\n\n`;
		prevFile = info.varName;
	});
	
	// 添加指针更新函数
	fsdata += `static void update_file_pointers(void) {\n`;
	fileInfos.forEach(info => {
		fsdata += `    // 更新${info.qualifiedName}的指针\n`;
		fsdata += `    ((struct fsdata_file *)file_${info.varName})->name = data_${info.varName};\n`;
		fsdata += `    ((struct fsdata_file *)file_${info.varName})->data = data_${info.varName} + ${info.paddedQualifiedNameLength};\n\n`;
	});
	fsdata += `}\n\n`;
	
	// 添加内存分配函数
	fsdata += `static bool allocate_memory(void) {\n`;
	fsdata += `    // 重置内存分配器\n`;
	fsdata += `    current_ram_addr = RAM_START_ADDR;\n\n`;
	
	fileInfos.forEach(info => {
		fsdata += `    data_${info.varName} = (uint8_t *)ram_alloc(SIZE_${info.varName.toUpperCase()});\n`;
	});
	
	fsdata += `\n    // 检查内存分配是否成功\n`;
	fsdata += `    if (`;
	fsdata += fileInfos.map(info => `data_${info.varName} == NULL`).join(' || ');
	fsdata += `) {\n        return false;\n    }\n    return true;\n}\n\n`;
	
	// 添加getFSRoot函数
	fsdata += `const struct fsdata_file * getFSRoot(void)\n{\n`;
	fsdata += `    if(fsdata_inited == false) {\n`;
	fsdata += `        int8_t result;\n`;
	fsdata += `        uint8_t d[4];\n`;
	fsdata += `        uint32_t len;\n`;
	fsdata += `        uint32_t addr;\n`;
	fsdata += `        uint32_t size;\n\n`;
	fsdata += `        // 分配内存\n`;
	fsdata += `        if (!allocate_memory()) {\n`;
	fsdata += `            return NULL;\n`;
	fsdata += `        }\n\n`;
	fsdata += `        printf("getFSRoot: allocate_memory success.\\n");\n\n`;
	
	fsdata += `        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr, 4);\n`;
	fsdata += `        len = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]); // 文件数量\n`;
	fsdata += `        addr = ex_fsdata_addr + 4 * (len + 1);\n\n`;
	
	fileInfos.forEach((info, index) => {
		fsdata += `        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + ${index}), sizeof(uint32_t));\n`;
		fsdata += `        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);\n`;
		fsdata += `        result = QSPI_W25Qxx_ReadBuffer(data_${info.varName}, addr, size);\n`;
		if (index < fileInfos.length - 1) {
			fsdata += `        addr += size;\n\n\n`;
		}
	});
	
	fsdata += `\n        // 更新文件结构体中的指针\n`;
	fsdata += `        update_file_pointers();\n\n`;
	fsdata += `        fsdata_inited = true;\n`;
	fsdata += `    }\n\n`;
	fsdata += `    return file_${fileInfos[fileInfos.length - 1].varName};\n`;
	fsdata += `}\n\n`;
	
	// 添加清理函数
	fsdata += `void fsdata_cleanup(void)\n{\n`;
	fsdata += `    if (fsdata_inited) {\n`;
	fsdata += `        // 释放所有动态分配的内存\n`;
	fileInfos.forEach(info => {
		fsdata += `        free(data_${info.varName});\n`;
	});
	fsdata += `\n        // 重置指针为 NULL\n`;
	fileInfos.forEach(info => {
		fsdata += `        data_${info.varName} = NULL;\n`;
	});
	fsdata += `\n        fsdata_inited = false;\n`;
	fsdata += `    }\n`;
	fsdata += `}\n

	const uint8_t numfiles = ${fileInfos.length};
	`;
	
	fs.writeFileSync(fsdataPath, fsdata, 'utf8');
	
	if(__makeExFile === true) {
		// 生成外部文件
		const allFileData = makeAllFileData(fileDataBuffers);
		fs.writeFileSync(exfilePath, allFileData.buffer, 'utf8');
		console.log('make bin file success.');
	}
}

makefsdata();
