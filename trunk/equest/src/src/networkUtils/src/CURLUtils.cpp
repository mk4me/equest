#include <networkUtils/CURLUtils.h>

using namespace networkUtils;

CURLPtr networkUtils::wrapCurlEasy(CURL * curl)
{
	return CURLPtr(curl, &curl_easy_cleanup);
}

CURLMPtr networkUtils::wrapCurlMulti(CURLM * curl)
{
	return CURLMPtr(curl, &curl_multi_cleanup);
}

CURLPtr networkUtils::createCurlEasy()
{
	return wrapCurlEasy(curl_easy_init());
}

CURLMPtr networkUtils::createCurlMulti()
{
	return wrapCurlMulti(curl_multi_init());
}