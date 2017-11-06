#include <networkUtils/CURLFTPHelper.h>
#include <utils/StreamTools.h>

using namespace networkUtils;

void CURLFTPHelper::initializeFTPConnection(CURL * curl,
	const std::string & user,
	const std::string & password,
	const SSLHostVerification hostVerification,
	const std::string & caPath)
{
	std::string user_(user);
	std::string password_(password);

	//login, password
	if (user_.empty() == true) {
		user_ = "anonymous";
		password_ = "anonymous@localhost";
	}

	curl_easy_setopt(curl, CURLOPT_USERNAME, user_.c_str());
	curl_easy_setopt(curl, CURLOPT_PASSWORD, password_.c_str());
	// tylko IPv4
	curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);

	//czy weryfikujemy serwer
	if (hostVerification != HVNone){
		// czy serwer ma weryfikowaæ nasz certyfikat
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
		// typ weryfikacji hosta
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, hostVerification);
		// je¿eli jest ¶cie¿ka certyfikatu to j± podajemy
		if (caPath.empty() == false){
			curl_easy_setopt(curl, CURLOPT_CAINFO, caPath.c_str());
		}
	}
	else{
		// czy serwer ma weryfikowaæ nasz certyfikat
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	}
}

int seek(void *stream, curl_off_t offset, int origin)
{
	std::istream * istream = static_cast<std::istream*>(stream);
	std::ios_base::seekdir dir = std::ios_base::beg;

	if (origin == SEEK_CUR){
		dir = std::ios_base::cur;
	}
	else if (origin == SEEK_END){
		dir = std::ios_base::end;
	}

	int ret = 0;

	try{
		istream->seekg(offset, dir);
	}
	catch (...){
		ret = 2;
	}

	return ret;
}

size_t _read(void* buffer, size_t size, size_t nmemb, void* stream)
{
	std::istream * istream = static_cast<std::istream*>(stream);
	auto offset = size * nmemb;
	try{
		if (istream->eof() == true){
			offset = 0;
		}
		else{
			offset = utils::StreamTools::forceReadSome(*istream, (char*)buffer, offset);
		}
	}
	catch (...){
		offset = 0;
	}

	return offset;
}

size_t _write(void *buffer, size_t size, size_t nmemb, void *stream)
{
	std::ostream * ostream = static_cast<std::ostream*>(stream);
	char * start = static_cast<char*>(buffer);
	auto offset = size * nmemb;	
	try{
		ostream->write((char*)buffer, offset);
		if (ostream->fail() == true || ostream->bad() == true){
			offset = 0;
		}
	}
	catch (...){
		offset = 0;
	}

	return offset;
}

int setDownloadCustomProgress(ICURLFTPProgress* progress, curl_off_t t, /* dltotal */ curl_off_t d, /* dlnow */ curl_off_t ultotal, curl_off_t ulnow)
{
	if (progress->aborted() == true) {
		return -1;
	}

	progress->setProcessedData(d >= 0 ? d : 0);
	progress->setProgress(t > 0 ? ((float)d / (float)t) : 0.0f);
	return 0;
}

int setUploadCustomProgress(ICURLFTPProgress* progress, curl_off_t t, /* dltotal */ curl_off_t d, /* dlnow */ curl_off_t ultotal, curl_off_t ulnow)
{
	if (progress->aborted() == true) {
		return -1;
	}

	progress->setProcessedData(ulnow >= 0 ? ulnow : 0);
	progress->setProgress(ultotal > 0 ? ((float)ulnow / (float)ultotal) : 0.0f);
	return 0;
}

void CURLFTPHelper::initializeFTPDownload(CURL * curl,
	const std::string & remoteFileUrl,
	std::ostream * localDestination,
	ICURLFTPProgress * progress)
{
	curl_easy_setopt(curl, CURLOPT_URL, remoteFileUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 0L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _write);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, localDestination);

	if (progress != nullptr){
		// przywracamy progres
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
		curl_easy_setopt(curl, CURLOPT_XFERINFODATA, progress);
		curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, setDownloadCustomProgress);
	}
	else{
		// brak progesu domy¶nie
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
	}
}

void CURLFTPHelper::initializeFTPUpload(CURL * curl,
	const std::string & remoteFileUrl,
	std::istream * localSource,
	ICURLFTPProgress * progress)
{
	curl_easy_setopt(curl, CURLOPT_URL, remoteFileUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, _read);
	curl_easy_setopt(curl, CURLOPT_READDATA, localSource);
	curl_easy_setopt(curl, CURLOPT_SEEKFUNCTION, seek);
	curl_easy_setopt(curl, CURLOPT_SEEKDATA, localSource);

	if (progress != nullptr){
		// przywracamy progres
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
		curl_easy_setopt(curl, CURLOPT_XFERINFODATA, progress);
		curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, setUploadCustomProgress);		
	}
	else{
		// brak progesu domy¶lnie
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
	}
}
