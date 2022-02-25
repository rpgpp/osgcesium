//============================================================================
// �ļ���: AES.h (c++)
// ����    : ���齣��
// �汾    : 20091025
// ��ַ    ��http://blog.sina.com.cn/u/1216062855
// ����    : AES��C++ʵ�֣�ֻ�Ƕ�c��ļ򵥷�װ�����ܻ�����һ����
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
	
	//����AES���ģʽΪ���ܻ��ǽ��ܣ���˵����Կ���Ⱥ����ĳ���(����ʱΪ����)
	//key		---��Կ����
	//keySize	---��Կ����
	//inSize	---���ĳ���(����ʱΪ����)
	//isdecrypt ---�Ƿ�Ϊ����(����ʱΪTRUE)
	//�ɹ�����1 ʧ�ܷ���-1 ʧ�������ڲ������ò���ȷ����
    int setMode(unsigned char* key , int keySize , int inSize , bool isdecrypt);

	//��������
	//input		---�������ݣ�����ΪsetModeʱinSizeָ��
	//ouput		---���ܺ�����ݣ�����ΪsetModeʱinSizeָ��
    void cipher (const unsigned char* input , unsigned char* output);

	//��������
	//input		---�������ݣ�����ΪsetModeʱinSizeָ��
	//output	---�������ݣ�����ΪsetModeʱinSizeָ��
    void invCipher(const unsigned char* input , unsigned char* output);

private:
	unsigned long* mState;
	unsigned long* mWord;
	unsigned char  mNb , mNr;
	unsigned char  mIsdecrypt;
};

#endif /*AES_H_*/
