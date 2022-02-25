//============================================================================
// 文件名: AES.h (c++)
// 作者    : 幽灵剑客
// 版本    : 20091025
// 地址    ：http://blog.sina.com.cn/u/1216062855
// 描述    : AES的C++实现，只是对c版的简单封装，性能基本上一样。
//============================================================================

#ifndef AES_H_
#define AES_H_


#define    len16    16
#define    len24    24
#define    len32    32

class AES
{
public:
    AES();
    ~AES();

public:
	
	//设置AES类的模式为加密还是解密，并说明密钥长度和明文长度(解密时为密文)
	//key		---密钥数据
	//keySize	---密钥长度
	//inSize	---明文长度(解密时为密文)
	//isdecrypt ---是否为解密(解密时为TRUE)
	//成功返回1 失败返回-1 失败是由于参数设置不正确引起
    int setMode(unsigned char* key , int keySize , int inSize , bool isdecrypt);

	//加密数据
	//input		---明文数据，长度为setMode时inSize指定
	//ouput		---加密后的数据，长度为setMode时inSize指定
    void cipher (const unsigned char* input , unsigned char* output);

	//解密数据
	//input		---密文数据，长度为setMode时inSize指定
	//output	---明文数据，长度为setMode时inSize指定
    void invCipher(const unsigned char* input , unsigned char* output);

private:
	unsigned long* mState;
	unsigned long* mWord;
	unsigned char  mNb , mNr;
	unsigned char  mIsdecrypt;
};

#endif /*AES_H_*/
