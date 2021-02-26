//
// Created by Yaozy on 2020/4/15.
//

#include "Curl.h"
#include <iostream>

using namespace std;
namespace Base {
    LibCurl::LibCurl() {
        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();
        mime = nullptr;
        mime = curl_mime_init(curl);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CallBack);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
        code = CURLE_OK;
        list = nullptr;
        httpCode = NONE;
    }

    LibCurl::~LibCurl() {
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    };

    void LibCurl::Init() {
        curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CallBack);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
        curl_mime_init(curl);
    }

    void LibCurl::CleanUp() {
        curl_easy_reset(curl);
        CleanHeaderList();
        CleanFrom();
    }

    bool LibCurl::SetPort(const int &port) {
        code = curl_easy_setopt(curl, CURLOPT_PORT, port);
        return code == CURLE_OK;
    }

    bool LibCurl::SetTimeOut(const int &second) {
        code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, second);
        return code == CURLE_OK;
    }

    bool LibCurl::SetConnectTimeout(const int &second) {
        code = curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, second);
        return code == CURLE_OK;
    }

    bool LibCurl::SetUserAgent(const string &agent) {
        if (agent.empty())
            return false;
        code = curl_easy_setopt(curl, CURLOPT_USERAGENT, agent.c_str());
        return code == CURLE_OK;
    }

    bool LibCurl::SetCookie(const string &cookie) {
        code = curl_easy_setopt(curl, CURLOPT_COOKIE, cookie.c_str());
        return code == CURLE_OK;
    }

    bool LibCurl::SetCookieFile(const string &cookiePath) {
        code = curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookiePath.c_str());
        return code == CURLE_OK;
    }

    bool LibCurl::SetResumeFrom(const long &fSeek) {
        if (fSeek < 0)
            return false;
        //设置文件偏移量
        code = curl_easy_setopt(curl, CURLOPT_RESUME_FROM, fSeek);
        return code == CURLE_OK;
    }

    bool LibCurl::SetResumeFromLarge(const long long &fSeek) {
        if (fSeek < 0)
            return false;
        //设置文件偏移量
        code = curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, fSeek);
        return code == CURLE_OK;
    }

    bool LibCurl::SetRedirect(const int &retry) {
        //自动设置header中的REFERER
        code = curl_easy_setopt(curl, CURLOPT_AUTOREFERER, true);
        code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);//支持重定向
        if (code == CURLE_OK)
            code = curl_easy_setopt(curl, CURLOPT_MAXREDIRS, retry);
        return code == CURLE_OK;
    }

    bool LibCurl::SetProgressDisplay(const bool &status) {
        //设置进度开关
        code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, status);
        //设置进度展示函数
        if (code == CURLE_OK) {
            code = curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);
            code = curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this);
        }
        return code == CURLE_OK;
    }

    bool LibCurl::SetCA(const string &caPath) {
        code = curl_easy_setopt(curl, CURLOPT_CAINFO, caPath.c_str());
        code = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
        code = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
        return code == CURLE_OK;
    }

    string LibCurl::AddQueryParameters(string &url, const string &queryParameters) {
        url = url + "?" + queryParameters;
        return url;
    }

    bool LibCurl::AddHeader(const string &key, const string &value) {
        string Header;
        Header = key + "=" + value;
        list = curl_slist_append(list, Header.c_str());
        code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
        code = curl_easy_setopt(curl, CURLOPT_QUOTE, list);
        return code == CURLE_OK;
    }

    void LibCurl::CleanHeaderList() {
        if (list != nullptr)
            curl_slist_free_all(list);
        list = nullptr;
    }

    void LibCurl::AddForm(const string &key, const string &value, const string &type) {
        auto *Part = curl_mime_addpart(mime);
        curl_mime_name(Part, key.c_str());
        curl_mime_data(Part, value.c_str(), value.size());
        curl_mime_type(Part, type.c_str());
    }

    void LibCurl::AddFile(const string &key, const string &filePath, const string &type) {
        curl_mimepart *Part = curl_mime_addpart(mime);
        curl_mime_name(Part, key.c_str());
        curl_mime_filedata(Part, filePath.c_str());
        curl_mime_type(Part, type.c_str());
    }

    void LibCurl::AddMemFile(const string &key, const string &value, const string &fileName, const string &type) {
        auto *Part = curl_mime_addpart(mime);
        curl_mime_name(Part, key.c_str());
        curl_mime_data(Part, value.c_str(), value.size());
        curl_mime_filename(Part, fileName.c_str());
        curl_mime_type(Part, type.c_str());
    }

    void LibCurl::CleanFrom() {
        if (mime != nullptr)
            curl_mime_free(mime);
        mime = nullptr;
        mime = curl_mime_init(curl);
    }

    bool LibCurl::Get(const string &address) {
//    assert(Address.empty());
        httpCode = GET;
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
        curl_easy_setopt(curl, CURLOPT_URL, address.c_str());
        code = curl_easy_perform(curl);
        CleanHeaderList();
        return CURLE_OK == code;
    }

    bool LibCurl::Post(const string &address) {
        callBackStr = "";
        httpCode = POST;
        list = curl_slist_append(list, "Expect:");
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        curl_easy_setopt(curl, CURLOPT_URL, address.c_str());
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
        curl_easy_setopt(curl, CURLOPT_HEADER, 0L);

        code = curl_easy_perform(curl);
        CleanHeaderList();
        CleanFrom();

        return CURLE_OK == code;
    }

    bool LibCurl::DownLoadFile(const string &address, string &downLoadFilePath) {
        outFile.open(downLoadFilePath, ios::out | ios::app | ios::binary);
        httpCode = DOWNLOAD;
        list = curl_slist_append(list, "Expect:");
        curl_easy_setopt(curl, CURLOPT_POST, true);
        curl_easy_setopt(curl, CURLOPT_URL, address.c_str());
        /*if (FromPtr != nullptr) {
            curl_easy_setopt(Curl, CURLOPT_INFILESIZE_LARGE, FromPtr->bufferlength);
        }
        curl_easy_setopt(Curl, CURLOPT_HTTPPOST, FromPtr);
    */
        curl_easy_setopt(curl, CURLOPT_HEADER, 0L);
        code = curl_easy_perform(curl);
        CleanHeaderList();
        CleanFrom();
        outFile.close();
        return CURLE_OK == code;
    }

    string LibCurl::GetErr() {
        return curl_easy_strerror(code);
    }

    size_t LibCurl::CallBack(char *ptr, size_t Size, size_t Nmemb, void *UserData) {
        auto *Ptr = (LibCurl *) UserData;
        int Len = 0;
        switch (Ptr->httpCode) {
            case NONE:
                break;
            case GET:
                Len = WriteCallback(ptr, Size, Nmemb, UserData);
                break;
            case POST:
                Len = WriteCallback(ptr, Size, Nmemb, UserData);
                break;
            case DOWNLOAD:
                Len = DownLoadCallback(ptr, Size, Nmemb, UserData);
                break;
        }
        return Size * Len;
    }

    size_t LibCurl::WriteCallback(char *ptr, size_t size, size_t Nmemb, void *UserData) {
        auto *Ptr = (LibCurl *) UserData;
        string &Buffer = *(static_cast<string *>(&Ptr->callBackStr));
        size_t Len = size * Nmemb;
        for (size_t i = 0; i < Len; ++i) {
            Buffer += *ptr;
            ++ptr;
        }
        return Len;
    }

    size_t LibCurl::DownLoadCallback(const char *ptr, size_t Size, size_t Nmemb, void *UserData) {
        auto *Ptr = (LibCurl *) UserData;
        Ptr->outFile.write(ptr, Nmemb);
        return Nmemb;
    }

    int LibCurl::ProgressCallback(void *UserData, double dltotal, double dlnow, double ultotal, double ulnow) {
        if (dltotal > -0.1 && dltotal < 0.1) {
            return 0;
        }
        int nPos = (int) ((dlnow / dltotal) * 100);
        cout << nPos << endl;
        return 0;
    }
}