//
// Created by Yaozy on 2020/4/15.
//

#ifndef CURL_CURL_H
#define CURL_CURL_H

#include <curl/curl.h>
#include <fstream>
#include <string>

namespace Base {

    using std::string;

    enum HTTPCode {
        NONE = 0,
        GET,
        POST,
        DOWNLOAD,
    };

    class LibCurl {
    public:
        LibCurl();

        ~LibCurl();

        //设置端口
        void Init();

        void CleanUp();

        bool SetPort(const int &port);

        //设置超时时间
        bool SetTimeOut(const int &second);

        //设置连接时间
        bool SetConnectTimeout(const int &second);

        //设置代理
        bool SetUserAgent(const string &agent);

        //设置Cookie
        bool SetCookie(const string &cookie);

        //设置Cookie文件
        bool SetCookieFile(const string &cookiePath);

        //设置断点续传起始位置
        bool SetResumeFrom(const long &fSeek);

        //设置断点续传起始位置(大文件)
        bool SetResumeFromLarge(const long long &fSeek);

        //设置页面跳转次数
        bool SetRedirect(const int &retry);

        //设置显示下载进度
        bool SetProgressDisplay(const bool &status);

        //设置CA证书
        bool SetCA(const string &caPath);

        //设置公钥
        //设置证书
        //添加Query Parameters
        static string AddQueryParameters(string &Url, const string &queryParameters);

        //添加变量到Header
        bool AddHeader(const string &key, const string &value);

        //清空Header
        void CleanHeaderList();

        //添加变量到表单
        void AddForm(const string &key, const string &value, const string &type);

        //添加内存文件到表单
        void AddFile(const string &key, const string &filePath, const string &type);

        void AddMemFile(const string &key, const string &value, const string &fileName, const string &type);

        //清空表单
        void CleanFrom();

        //获取错误消息
        string GetErr();

        //Get方法
        bool Get(const string &address);

        //Post方法
        bool Post(const string &address);

        //下载文件
        bool DownLoadFile(const string &address, string &downLoadFilePath);

        //返回消息
        string callBackStr;

    private:
        CURL *curl;
        CURLcode code;
        struct curl_slist *list;
        curl_mime *mime;
        HTTPCode httpCode;
        std::ofstream outFile;

    protected:
        static size_t WriteCallback(char *ptr, size_t size, size_t nmemb, void *userData);

        static size_t CallBack(char *ptr, size_t size, size_t nmemb, void *userData);

        static size_t DownLoadCallback(const char *ptr, size_t size, size_t nmemb, void *userData);

        //下载进度显示函数
        static int ProgressCallback(void *UserData, double dltotal, double dlnow, double ultotal, double ulnow);
    };
}
#endif //CURL_CURL_H
