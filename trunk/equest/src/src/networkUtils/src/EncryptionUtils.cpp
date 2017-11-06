#include <networkUtils/EncryptionUtils.h>
#include <openssl/aes.h>
#include "QtCore/QDataStream"
#include <array>

#define DEFAULT_KEY_1 {"tO8_)n7DSF^hBZBb"}
#define DEFAULT_KEY_2 {"asl)*sa_P@!jsNSA"}

//http://stackoverflow.com/questions/1007337/encrypting-and-decrypting-a-small-file-using-openssl

QString networkUtils::EncryptionUtils::encrypt(const QString& txt)
{
	EncryptionKey key1 = DEFAULT_KEY_1;
	EncryptionKey key2 = DEFAULT_KEY_2;
	return encrypt(txt, key1, key2);
}

QByteArray crypt(const QByteArray& txt, const networkUtils::EncryptionUtils::EncryptionKey& key1, networkUtils::EncryptionUtils::EncryptionKey key2, bool encrypt)
{
	int bytes_read;
	QDataStream bstream(txt);

	unsigned char indata[AES_BLOCK_SIZE];
	unsigned char outdata[AES_BLOCK_SIZE];
	
	/* data structure that contains the key itself */
	AES_KEY aeskey;

	/* set the encryption key */
	AES_set_encrypt_key(key1.data(), 128, &aeskey);

	/* set where on the 128 bit encrypted block to begin encryption*/
	int num = 0;
	QByteArray outarray;
	while (1) {
		bytes_read = bstream.readRawData((char*)indata, AES_BLOCK_SIZE);
		//bytes_read = fread(indata, 1, AES_BLOCK_SIZE, ifp);

		AES_cfb128_encrypt(indata, outdata, bytes_read, &aeskey, key2.data(), &num, encrypt ? AES_ENCRYPT : AES_DECRYPT);

		outarray.append((const char*)outdata, bytes_read);
		//bytes_written = fwrite(outdata, 1, bytes_read, ofp);
		if (bytes_read < AES_BLOCK_SIZE)
			break;
	}
	return outarray;
}


QString networkUtils::EncryptionUtils::encrypt(const QString& txt, const EncryptionKey& key1, const EncryptionKey& key2)
{
	if (txt.isEmpty()) {
		throw std::invalid_argument("Text to encrypt is empty");
	}
	QByteArray ba = crypt(txt.toUtf8(), key1, key2, true);
	return ba.toBase64();
}

QString networkUtils::EncryptionUtils::decrypt(const QString& txt)
{
	EncryptionKey key1 = DEFAULT_KEY_1;
	EncryptionKey key2 = DEFAULT_KEY_2;
	return decrypt(txt, key1, key2);
}

QString networkUtils::EncryptionUtils::decrypt(const QString& txt, const EncryptionKey& key1, const EncryptionKey& key2)
{
	if (txt.isEmpty()) {
		throw std::invalid_argument("Text to encrypt is empty");
	}
	QByteArray ba = fromBase64(txt);
	return crypt(ba, key1, key2, false);
}

QString networkUtils::EncryptionUtils::toBase64(const QByteArray& buffer)
{
	return QString(buffer.toBase64());
}

QByteArray networkUtils::EncryptionUtils::fromBase64(const QString& buffer)
{
	return QByteArray::fromBase64(buffer.toUtf8());
}

networkUtils::EncryptionUtils::EncryptionKey networkUtils::EncryptionUtils::convertEncryptionKey(const QString& key)
{
	if (key.size() != EncryptionKeyLength) {
		throw std::invalid_argument("Invalid key size");
	}
	std::string skey = key.toStdString();
	EncryptionKey d;
	std::copy(std::begin(skey), std::end(skey), std::begin(d));
	return d;
}


