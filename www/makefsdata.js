import path from 'node:path';
import fs from 'node:fs';

import { fileURLToPath } from 'node:url';

import pako from 'pako';
const __makeExFile = true;	//是否将文件内容生成外部文件
const __filename = fileURLToPath(import.meta.url);

const root = path.dirname(__filename).replace(path.normalize('www'), '');
const rootwww = path.dirname(__filename);
const buildPath = path.join(rootwww, 'build');
const ex_fsdata_addr = 0x00000000;			// 读取外部fsdata文件在qspiflash中的地址
const memory_section = '._Text_Area';		// 运行时保存fsdata数据所用的 RAM section，需要在ld文件中定义
const fsdataPath = path.normalize(path.join(root, 'Libs/httpd/fsdata.c'));
const exfilePath = path.normalize(path.join(root, 'Libs/httpd/ex_fsdata.bin'));

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

const serverHeader = 'GP2040-CE';

const payloadAlignment = 4;
const hexBytesPerLine = 16;

function getFiles(dir) {
	let results = [];
	const list = fs.readdirSync(dir, { withFileTypes: true });
	for (const file of list) {
		file.path = dir + '/' + file.name;
		if (file.isDirectory()) {
			results = results.concat(getFiles(file.path));
		} else if (file.isFile()) {
			results.push(file.path);
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
	fsdata += '\n';
	fsdata += '#define file_NULL (struct fsdata_file *) NULL\n';
	fsdata += '\n';
	fsdata += '#ifndef FS_FILE_FLAGS_HEADER_INCLUDED\n';
	fsdata += '#define FS_FILE_FLAGS_HEADER_INCLUDED 1\n';
	fsdata += '#endif\n';
	fsdata += '#ifndef FS_FILE_FLAGS_HEADER_PERSISTENT\n';
	fsdata += '#define FS_FILE_FLAGS_HEADER_PERSISTENT 0\n';
	fsdata += '#endif\n';
	fsdata += '/* FSDATA_FILE_ALIGNMENT: 0=off, 1=by variable, 2=by include */\n';
	fsdata += '#ifndef FSDATA_FILE_ALIGNMENT\n';
	fsdata += '#define FSDATA_FILE_ALIGNMENT 0\n';
	fsdata += '#endif\n';
	fsdata += '#ifndef FSDATA_ALIGN_PRE\n';
	fsdata += '#define FSDATA_ALIGN_PRE\n';
	fsdata += '#endif\n';
	fsdata += '#ifndef FSDATA_ALIGN_POST\n';
	fsdata += '#define FSDATA_ALIGN_POST\n';
	fsdata += '#endif\n';
	fsdata += '#if FSDATA_FILE_ALIGNMENT==2\n';
	fsdata += '#include "fsdata_alignment.h"\n';
	fsdata += '#endif\n';
	fsdata += '\n';
	fsdata += '#define ex_fsdata_addr FSDATA_ADDR\n';
	fsdata += '\n';
	fsdata += `#define __Text_Area__ __attribute__((section("${memory_section}")))\n`;
	fsdata += '\n';
	fsdata += 'static bool fsdata_inited = false;\n';
	fsdata += '\n';

	let payloadAlignmentDummyCounter = 0;

	const fileInfos = [];
	const fileDataBuffers = [];

	getFiles(buildPath).forEach((file) => {
		const ext = getLowerCaseFileExtension(file);

		const qualifiedName = '/' + path.relative(buildPath, file).replace(/\\/g, '/');
		const varName = fixFilenameForC(qualifiedName);

		fsdata += '#if FSDATA_FILE_ALIGNMENT==1\n';
		fsdata += `static const unsigned int dummy_align_${varName} = ${payloadAlignmentDummyCounter++};\n`;
		fsdata += '#endif\n';

		const fileContent = fs.readFileSync(file);
		let compressed = fileContent.buffer;
		let isCompressed = false;
		if (!skipCompressionExtensions.has(ext)) {
			compressed = pako.deflate(fileContent, {
				level: 9,
				windowBits: 15,
				memLevel: 9,
			});
			console.log(
				`Compressed ${qualifiedName} from ${fileContent.byteLength} to ${
					compressed.byteLength
				} bytes (${Math.round(
					(compressed.byteLength / fileContent.byteLength) * 100,
				)}%)`,
			);
			if (compressed.byteLength >= fileContent.byteLength) {
				console.log(
					`Skipping compression of ${qualifiedName}, compressed size is larger than original`,
				);
			} else {
				isCompressed = true;
			}
		} else {
			console.log(`Skipping compression of ${qualifiedName} by file extension`);
		}

		const qualifiedNameLength = CStringLength(qualifiedName) + 1;
		const paddedQualifiedNameLength =
			Math.ceil(qualifiedNameLength / payloadAlignment) * payloadAlignment;
		// Zero terminate the qualified name and pad it to the next multiple of payloadAlignment
		const paddedQualifiedName =
			qualifiedName +
			'\0'.repeat(1 + paddedQualifiedNameLength - qualifiedNameLength);

		const fileBuffer = makeFileBuffer(paddedQualifiedName, ext, isCompressed, fileContent, compressed);

		// 是否生成外部文件
		if(__makeExFile === true) {
			// 生成二进制文件数据
			fileDataBuffers.push(fileBuffer);
			fsdata += `__Text_Area__ static unsigned char data_${varName}[${fileBuffer.byteLength}] FSDATA_ALIGN_PRE;\n\n`;
		} else {
			fsdata += `static unsigned char data_${varName}[] FSDATA_ALIGN_PRE = {\n`;
			fsdata += `/* ${qualifiedName} (${qualifiedNameLength} chars) */\n`;
			fsdata += createHexString(paddedQualifiedName, false);
			fsdata += '\n';
			fsdata += '/* HTTP header */\n';
			fsdata += createHexString('HTTP/1.0 200 OK\r\n', true);
			fsdata += createHexString(`Server: ${serverHeader}\r\n`, true);
			fsdata += createHexString(
				`Content-Length: ${
					isCompressed ? compressed.byteLength : fileContent.byteLength
				}\r\n`,
				true,
			);
			if (isCompressed) {
				fsdata += createHexString('Content-Encoding: deflate\r\n', true);
			}
			fsdata += createHexString(
				`Content-Type: ${contentTypes.get(ext) ?? defaultContentType}\r\n\r\n`,
				true,
			);
			fsdata += `/* raw file data (${
				isCompressed ? compressed.byteLength : fileContent.byteLength
			} bytes) */\n`;
			fsdata += createHexString(isCompressed ? compressed : fileContent);
			fsdata += `};\n\n`;
		}
		

		fileInfos.push({
			varName,
			paddedQualifiedNameLength,
			isSsiFile: shtmlExtensions.has(ext),
		});

	});
	
	let prevFile = 'NULL';
	fileInfos.forEach((fileInfo) => {
		fsdata += `const struct fsdata_file file_${fileInfo.varName}[] = {{\n`;
		fsdata += `file_${prevFile},\n`;
		fsdata += `data_${fileInfo.varName},\n`;
		fsdata += `data_${fileInfo.varName} + ${fileInfo.paddedQualifiedNameLength},\n`;
		fsdata += `sizeof(data_${fileInfo.varName}) - ${fileInfo.paddedQualifiedNameLength},\n`;
		fsdata += `FS_FILE_FLAGS_HEADER_INCLUDED | ${
			fileInfo.isSsiFile
				? 'FS_FILE_FLAGS_SSI'
				: 'FS_FILE_FLAGS_HEADER_PERSISTENT'
		}\n`;
		fsdata += '}};\n\n';

		prevFile = fileInfo.varName;
	});

	fsdata += `
const struct fsdata_file * getFSRoot(void)
{
	if(fsdata_inited == false) {
		int8_t result;
        uint8_t d[4];
        uint32_t len;
        uint32_t addr;
        uint32_t size;

		result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr, 4);
        len = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]); // 文件数量
	`;
	
	fileInfos.forEach((fileInfo, index) => {
		if(index == 0) {
			fsdata += `addr = ex_fsdata_addr + 4 * (len + 1);`;
		} else {
			fsdata += `addr += size;`;
		}
		fsdata += `
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + ${index}), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data_${fileInfo.varName}, addr, size);
		`;
	});

	fsdata += `
		fsdata_inited = true;
	}

	return file_${fileInfos[fileInfos.length - 1].varName};
}
	\n`;

	fsdata += `const uint8_t numfiles = ${fileInfos.length};\n`;

	fs.writeFileSync(fsdataPath, fsdata, 'utf8');

	if(__makeExFile === true) {
		// 生成外部文件
		const allFileData = makeAllFileData(fileDataBuffers);

		fs.writeFileSync(exfilePath, allFileData.buffer, 'utf8');

		console.log('make bin file success.');
	}
}

makefsdata();
