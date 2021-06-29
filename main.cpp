#include <iostream>
#include <bitset>

using namespace std;

bitset<28> rotateLeftShift(bitset<28> origin_key, int shift_bits)
{
	bitset<28> result = origin_key;
	for (int i = 27; i >= 0; i--)
	{
		if (i - shift_bits < 0)
			result[i] = origin_key[i - shift_bits + 28];
		else
			result[i] = origin_key[i - shift_bits];
	}
	return result;
}

bitset<48>* generateKeys(bitset<64> input_key)
{
	cout << "*-----* 1 生成子密钥 *-----*" << endl;
	// 置换选择矩阵1
	const int PC_1[] = { 57, 49, 41, 33, 25, 17, 9,
						 1, 58, 50, 42, 34, 26, 18,
						 10, 2, 59, 51, 43, 35, 27,
						 19, 11, 3, 60, 52, 44, 36,
						 63, 55, 47, 39, 31, 23, 15,
						 7, 62, 54, 46, 38, 30, 22,
						 14, 6, 61, 53, 45, 37, 29,
						 21, 13, 5, 28, 20, 12, 4 };
	// 置换选择矩阵2
	const int PC_2[] = { 14, 17, 11, 24, 1, 5, 3, 28,
						 15, 6, 21, 10, 23, 19, 12, 4,
						 26, 8, 16, 7, 27, 20, 13, 2,
						 41, 52, 31, 37, 47, 55, 30, 40,
						 51, 45, 33, 48, 44, 49, 39, 56,
						 34, 53, 46, 42, 50, 36, 29, 32 };
	// 左转移方案
	const int LEFT_SHIFT_SCHEME[] = { 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };
	auto sub_keys = new bitset<48>[16]; // 最终的子密钥结果
	bitset<56> temp_key; // 存储中间结果
	// 置换选择1
	for (int i = 0; i < 56; i++)
	{
		temp_key[55 - i] = input_key[64 - PC_1[i]];
	}
	cout << "step 1: 置换选择1" << endl;
	cout << "\t(C0, D0) = " << temp_key << endl;
	bitset<28> c, d;
	cout << "step 2: 产生16个子密钥" << endl;
	for (int loop = 0; loop < 16; loop++)
	{
		cout << "生成第" << loop + 1 << "个子密钥" << endl;
		// 拷贝
		for (int i = 28; i < 56; i++)
		{
			c[i - 28] = temp_key[i];
		}
		for (int i = 0; i < 28; i++)
		{
			d[i] = temp_key[i];
		}
		// 循环左移
		c = rotateLeftShift(c, LEFT_SHIFT_SCHEME[loop]);
		d = rotateLeftShift(d, LEFT_SHIFT_SCHEME[loop]);
		cout << "\t循环左移" << endl << "\tC" << loop + 1 << "=" << c << ", D" << loop + 1 << "=" << d << endl;
		// 合并结果
		for (int i = 28; i < 56; i++)
		{
			temp_key[i] = c[i - 28];
		}
		for (int i = 0; i < 28; i++)
		{
			temp_key[i] = d[i];
		}
		// 置换选择2
		for (int i = 0; i < 48; i++)
		{
			sub_keys[loop][47 - i] = temp_key[56 - PC_2[i]];
		}
		cout << "\t置换选择2" << endl << "\tK" << loop + 1 << "=" << sub_keys[loop] << endl;
	}
	cout << endl;
	return sub_keys;
}

bitset<32> f(bitset<32> r, bitset<48> k)
{
	cout << "\t加密函数计算" << endl;
	const int E[] = { 32, 1, 2, 3, 4, 5,
					  4, 5, 6, 7, 8, 9,
					  8, 9, 10, 11, 12, 13,
					  12, 13, 14, 15, 16, 17,
					  16, 17, 18, 19, 20, 21,
					  20, 21, 22, 23, 24, 25,
					  24, 25, 26, 27, 28, 29,
					  28, 29, 30, 31, 32, 1 };
	const int S_BOX[8][4][16] = {
		{
			{ 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 },
			{ 0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8 },
			{ 4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0 },
			{ 15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13 }
		},
		{
			{ 15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10 },
			{ 3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5 },
			{ 0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15 },
			{ 13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9 }
		},
		{
			{ 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8 },
			{ 13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1 },
			{ 13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7 },
			{ 1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12 }
		},
		{
			{ 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15 },
			{ 13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9 },
			{ 10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4 },
			{ 3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14 }
		},
		{
			{ 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9 },
			{ 14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6 },
			{ 4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14 },
			{ 11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3 }
		},
		{
			{ 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11 },
			{ 10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8 },
			{ 9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6 },
			{ 4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13 }
		},
		{
			{ 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1 },
			{ 13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6 },
			{ 1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2 },
			{ 6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12 }
		},
		{
			{ 13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7 },
			{ 1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2 },
			{ 7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8 },
			{ 2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11 }
		}
	};
	const int P[] = { 16, 7, 20, 21, 29, 12, 28, 17,
					  1, 15, 23, 26, 5, 18, 31, 10,
					  2, 8, 24, 14, 32, 27, 3, 9,
					  19, 13, 30, 6, 22, 11, 4, 25 };
	bitset<48> expand_r;
	// 扩展置换
	for (int i = 0; i < 48; i++)
	{
		expand_r[47 - i] = r[32 - E[i]];
	}
	cout << "\t\t扩展置换: " << expand_r << endl;
	// 异或
	expand_r = expand_r ^ k;
	cout << "\t\t扩展置换与Ki异或: " << expand_r << endl;
	// S盒转换
	bitset<32> s_output;
	for (int i = 0, j = 0; i < 48; i += 6, j += 4)
	{
		int row = expand_r[47 - i] * 2 + expand_r[47 - i - 5];
		int col = expand_r[47 - i - 1] * 8 + expand_r[47 - i - 2] * 4 + expand_r[47 - i - 3] * 2 + expand_r[47 - i - 4];
		int num = S_BOX[i / 6][row][col];
		bitset<4> temp(num);
		s_output[31 - j] = temp[3];
		s_output[31 - j - 1] = temp[2];
		s_output[31 - j - 2] = temp[1];
		s_output[31 - j - 3] = temp[0];
	}
	cout << "\t\tS盒输出: " << s_output << endl;
	// 置换函数P
	bitset<32> result;
	for (int i = 0; i < 32; ++i)
	{
		result[31 - i] = s_output[32 - P[i]];
	}
	cout << "\t\t经过置换函数P: " << result << endl;
	return result;
}

bitset<64> desEncrypt(bitset<64> plain_bits, bitset<48>* sub_keys)
{
	cout << "*-----* 2 加密 *-----*" << endl;
	const int IP[] = { 58, 50, 42, 34, 26, 18, 10, 2,
					   60, 52, 44, 36, 28, 20, 12, 4,
					   62, 54, 46, 38, 30, 22, 14, 6,
					   64, 56, 48, 40, 32, 24, 16, 8,
					   57, 49, 41, 33, 25, 17, 9, 1,
					   59, 51, 43, 35, 27, 19, 11, 3,
					   61, 53, 45, 37, 29, 21, 13, 5,
					   63, 55, 47, 39, 31, 23, 15, 7 };
	const int INVERSE_IP[] = { 40, 8, 48, 16, 56, 24, 64, 32,
							   39, 7, 47, 15, 55, 23, 63, 31,
							   38, 6, 46, 14, 54, 22, 62, 30,
							   37, 5, 45, 13, 53, 21, 61, 29,
							   36, 4, 44, 12, 52, 20, 60, 28,
							   35, 3, 43, 11, 51, 19, 59, 27,
							   34, 2, 42, 10, 50, 18, 58, 26,
							   33, 1, 41, 9, 49, 17, 57, 25 };
	bitset<64> init_bits;
	cout << "step 1: 初始置换" << endl;
	// 初始置换
	for (int i = 0; i < 64; i++)
	{
		init_bits[63 - i] = plain_bits[64 - IP[i]];
	}
	bitset<32> l, r, temp;
	// 拆分成LR两部分
	for (int i = 32; i < 64; i++)
	{
		l[i - 32] = init_bits[i];
	}
	cout << "\tL0=" << l << endl;
	for (int i = 0; i < 32; i++)
	{
		r[i] = init_bits[i];
	}
	cout << "\tR0=" << r << endl;
	cout << "step 2: 16轮置换和代换迭代" << endl;
	// DES迭代
	for (int loop = 0; loop < 16; loop++)
	{
		cout << "\t第" << loop + 1 << "轮" << endl;
		temp = l;
		l = r;
		r = temp ^ f(r, sub_keys[loop]);
		cout << "\tL" << loop + 1 << "=" << l << endl;
		cout << "\tR" << loop + 1 << "=" << r << endl;
	}
	bitset<64> merged_text;
	// 合并LR
	for (int i = 0; i < 32; ++i)
		merged_text[i] = l[i];
	for (int i = 32; i < 64; ++i)
		merged_text[i] = r[i - 32];
	// 逆初始置换
	bitset<64> cipher_bits;
	for (int i = 0; i < 64; ++i)
	{
		cipher_bits[63 - i] = merged_text[64 - INVERSE_IP[i]];
	}
	// 返回密文
	cout << endl;
	return cipher_bits;
}

int main()
{
	int64_t plain_text = 0x0123456789ABCDEF, key = 0x133457799BBCDFF1;
	bitset<64> plain_bits(plain_text);
	bitset<64> key_bits(key);
	cout << "明文: " << plain_bits << endl << "密钥: " << key_bits << endl << endl;
	bitset<48>* sub_keys = generateKeys(key);
	bitset<64> cipher_bits = desEncrypt(plain_bits, sub_keys);
	cout << "密文: " << cipher_bits << endl;
	delete[] sub_keys;
	return 0;
}
