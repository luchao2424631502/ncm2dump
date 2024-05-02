#include "dump.h"

static uint8_t core_key[0x10] = {
	0x68, 0x7A, 0x48, 0x52,
       	0x41, 0x6D, 0x73, 0x6F,
       	0x35, 0x6B, 0x49, 0x6E,
       	0x62, 0x61, 0x78, 0x57
}; 

static uint8_t meta_key[0x10] = {
	0x23, 0x31, 0x34, 0x6C, 
	0x6A, 0x6B, 0x5F, 0x21,
	0x5C, 0x5D, 0x26, 0x30,
	0x55, 0x3C, 0x27, 0x28
};

static uint8_t header_valid[] = {
       	0x43, 0x54, 0x45, 0x4e,
       	0x46, 0x44, 0x41, 0x4d
};

uint8_t *pkcs7_unpad(uint8_t *data, uint32_t *len_addr)
{
	// size_t len = strlen(data);
	size_t len = *len_addr;
	if (data[len - 1] < 1 || data[len - 1] > 16)
		return NULL;
	uint8_t padding = data[len - 1];
	uint8_t *new_data = malloc(sizeof(uint8_t) * (len - padding));
	memcpy(new_data, data, len - padding);
	// 更新长度
	*len_addr = len - padding;
	free(data);
	return new_data;
}

uint8_t *cut_neteasemusic(uint8_t *data)
{
	uint8_t *new_data = malloc(sizeof(uint8_t) * (strlen(data) - 17));
	memcpy(new_data, data + 17, strlen(data) - 17);
	free(data);
	return new_data;
}

uint8_t *cut_first(uint8_t *data, uint32_t len, uint32_t cut_num)
{
	uint32_t new_len = len - cut_num;
	uint8_t *new_data = malloc(sizeof(uint8_t) * new_len);
	memcpy(new_data, data + cut_num, new_len);
	free(data);
	return new_data;
}

uint8_t *dump(uint8_t *filedata, size_t filesize,
		uint32_t *music_file_size, char **filename)
{
	uint32_t file_offset = 0;
	// 0. 校验header
	for (size_t i = 0; i < 8; i++) {
		if (header_valid[i] != filedata[i]) {
			goto header_error;
		}
	}

	file_offset = 10;
	uint32_t key_length = *(uint32_t *)(filedata + file_offset);
	uint32_t original_key_length = key_length;

	file_offset += 4;
	uint8_t *key_data_array = malloc(sizeof(uint8_t) * key_length);
	memcpy(key_data_array, (uint8_t *)(filedata + file_offset), key_length);
	
	// 1. 解密RC4的秘钥
	for (size_t i = 0; i < key_length; i++)
		key_data_array[i] ^= 0x64;

	struct AES_ctx ctx;
	AES_init_ctx(&ctx, core_key);
	for (size_t i = 0; i < ((key_length)>>4); i++)
		AES_ECB_decrypt(&ctx, (16*i)+key_data_array);

	key_data_array = pkcs7_unpad(key_data_array, &key_length); // 更新key_length
	// key_data_array = cut_neteasemusic(key_data_array); // 得到的是RC4的秘钥
	key_data_array = cut_first(key_data_array, key_length, 17);
	key_length -= 17;
	
	// key_length = strlen(key_data_array);

	// 生成RC4-KSA
	uint8_t *key_box = malloc(sizeof(uint8_t) * 256);
	for(size_t i = 0; i < 256; i++)
		key_box[i] = i;
	// parray1(key_box, 256);
	uint32_t c=0, last_byte=0, key_offset=0;
	for(size_t i = 0; i < 256; i++) {
		uint8_t swap = key_box[i];
		c = (swap + last_byte + key_data_array[key_offset]) & 0xff;
		key_offset += 1;
		if (key_offset >= key_length)
			key_offset = 0;
		key_box[i] = key_box[c];
		key_box[c] = swap;
		last_byte = c;
	}

	// 2. 解密meta_data
	// file_offset += 128; // 不一定是固定的128
	file_offset += original_key_length;
	uint32_t meta_length = *(uint32_t *)(file_offset + filedata);
	px(meta_length);
	file_offset += 4; // meta_data 起始偏移量=0x92 , 0x340=file_offset+meta_length
	uint8_t *meta_data = malloc(sizeof(uint8_t) * meta_length);
	memcpy(meta_data, file_offset + filedata, meta_length);
	// parray1(meta_data, meta_length); // 截止这里是没有问题的
	
	for (size_t i = 0; i < meta_length; i++)
		meta_data[i] ^= 0x63;
	
	// meta_data += 22; // 这里有问题[FIXED] 下面解决了内存泄露
	meta_data = cut_first(meta_data, meta_length, 22);
	
	size_t meta_decode_data_len;
	uint8_t *meta_decode_data = b64_decode_ex(meta_data, meta_length, &meta_decode_data_len); // base64解码后数据

	meta_decode_data_len = (meta_decode_data_len >> 4) << 4;
	
	// base64解码正确, 进行AES解码
	struct AES_ctx ctx2;
	AES_init_ctx(&ctx2, meta_key);
	for (size_t i = 0; i < (meta_decode_data_len >> 4); i++)
		AES_ECB_decrypt(&ctx2, (16*i)+meta_decode_data);

	meta_decode_data = pkcs7_unpad(meta_decode_data, (uint32_t *)&meta_decode_data_len);

	parray1(meta_decode_data, strlen(meta_decode_data));
	ps(meta_decode_data + 6);

	// 成功解析出json格式的元数据, 下一步解析file data
	uint8_t *meta_data_json = meta_decode_data + 6; 
#if 1 // 通过json(文件元数据)得到 文件名/格式
	cJSON *json = cJSON_Parse(meta_data_json);
	char *music_name = strdup(cJSON_GetObjectItem(json,"musicName")->valuestring);
	char *music_format = strdup(cJSON_GetObjectItem(json,"format")->valuestring);
	char *music_file_name = malloc(strlen(music_name)+strlen(music_format)+2);
	memcpy(music_file_name, music_name, strlen(music_name));
	music_file_name[strlen(music_name)] = '.';
	memcpy(music_file_name + strlen(music_name) + 1, music_format, 
			strlen(music_format));
	music_file_name[strlen(music_name)+1+strlen(music_format)] = '\0';
	*filename = music_file_name; // 返回真实文件名
#endif
	file_offset += meta_length; // 跳过了元数据data
#ifdef GET_CRC
	uint32_t crc_value = *(uint32_t *)(filedata + file_offset);
#endif
	file_offset += 4; // 跳过CRC
	file_offset += 5; // 跳过GAP
	uint32_t image_size = *(uint32_t *)(filedata + file_offset);
	file_offset += 4; // 跳过image_size
#ifdef GET_IMAGE
	uint8_t *image_data = malloc(sizeof(uint8_t) * image_size);
	memcpy(image_data, filedata + file_offset, image_size);
#endif
	file_offset += image_size; // 跳过image_data

	// 生成mp3文件
	size_t music_size = filesize - file_offset; // 音频数据字节大小
	uint8_t *music_data = malloc(sizeof(uint8_t) * music_size); // 音频数据(待解密)
	memcpy(music_data, filedata + file_offset, music_size);
	px(music_size);

	uint8_t *seg_data = music_data;
	uint32_t seg_offset = 0x0;
	while (1) {
		uint32_t seg_size = 0;
		if ((music_size - seg_offset) == 0x0) {
			break;
		}
		else if ((music_size - seg_offset) < 0x8000) {
			seg_size = music_size - seg_offset;
		} else {
			seg_size = 0x8000;
		}
		for (size_t i = 1; i <= seg_size; i++) {
			uint8_t tmp = i & 0xff;
			seg_data[i - 1] ^= key_box[(key_box[tmp] + key_box[(key_box[tmp]+tmp) & 0xff]) & 0xff];
		}

		seg_data += seg_size;
		seg_offset += seg_size;
	}
	*music_file_size = music_size; // 返回真实音乐文件大小
	
	// 释放内存
	free(key_data_array);
	free(key_box);
	free(meta_data);
	free(music_name);
	free(music_format);
	return music_data;
pointer_error:
	ps("pointer error\n");
	return NULL;
header_error:
	ps("header error\n");
	return NULL;
}

int work_convert(char *input_path)
{
	//int fd = open("./test3.ncm", O_RDWR | O_CREAT);
	int fd = open(input_path, O_RDWR | O_CREAT);
	if (-1 == fd)
		return -1;

	struct stat st;
	if (-1 == fstat(fd, &st))
		return -1;
	size_t filesize = st.st_size;

	void *filedata = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

	uint32_t music_file_size = 0;
	char *filename = NULL;
	uint8_t *decode_file_data = dump(filedata, filesize,
			&music_file_size, &filename);
	if (!decode_file_data) {
		ps("decode_file_data ERROR!\n");
		return -1;
	}

	px(music_file_size);

	// 生成转换的文件
	int outfd;
	outfd = open(filename, O_CREAT | O_RDWR, 0644);

	if (-1 == outfd) {
		ps("outfd ERROR!\n");
		return -1;
	}

	uint32_t tmp_offset = 0;
	uint32_t tmp_size = 0;
	while (tmp_offset < music_file_size) {
		tmp_size = write(outfd, decode_file_data + tmp_offset, music_file_size - tmp_offset);
		tmp_offset += tmp_size;
	}

	close(fd);
	close(outfd);
	munmap(filedata, st.st_size);
	// 释放文件数据非常重要(否则内存占用的很严重)
	free(decode_file_data);
	free(filename);
}
