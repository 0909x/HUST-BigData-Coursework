#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <windows.h>
#include <sstream>
#include <unordered_map>
#include <iomanip>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

// 配置参数数据结构
struct ServerConfig {
    string IP = "127.0.0.1";
    int PORT = 5050;
    string HOME = "./";
    int WaitCapacity = 10;
};

// 结构体存储HTTP请求解析结果
struct ParsedRequest {
    string method;
    string uri;
    string httpVersion;
    unordered_map<string, string> headers;
};

// ContentType映射
unordered_map<string, string> contentTypes = {
    {".html", "text/html; charset=utf-8"},
    {".htm", "text/html; charset=utf-8"},
    {".xml", "text/xml; charset=utf-8"},
    {".gif", "image/gif"},
    {".png", "image/png"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".mp3", "audio/mpeg"},
    {".mp4", "video/mp4"},
    {".ico", "image/x-icon"},
    {".pdf", "application/pdf"},
    {".zip", "application/zip"},
    {".txt", "text/plain; charset=utf-8"},
    {".css", "text/css; charset=utf-8"},
    {".js", "application/javascript; charset=utf-8"},
    {".json", "application/json; charset=utf-8"}
};

// URL重定向映射
unordered_map<string, string> UrlMap = {
    {"/moved.html", "/moved/moved.html"},
    {"/old", "/new"}
};

// 写日志函数
void WriteLog(string Ip, int Port, string note, string path) {
    ofstream logFile("./log.txt", ios::app);
    if (logFile.is_open()) {
        time_t currentTime = time(nullptr);
        tm* localTime = localtime(&currentTime);
        logFile << "When "
            << localTime->tm_year + 1900 << "-"
            << setw(2) << setfill('0') << localTime->tm_mon + 1 << "-"
            << setw(2) << setfill('0') << localTime->tm_mday << " "
            << setw(2) << setfill('0') << localTime->tm_hour << ":"
            << setw(2) << setfill('0') << localTime->tm_min << ":"
            << setw(2) << setfill('0') << localTime->tm_sec << ",";
        logFile << "IP: " << Ip << " Port: " << Port
            << " Try to get the source from " << path
            << ", but " << note << " occurred" << endl;
        logFile.close();
    }
}

// 获取当前时间的HTTP格式
string getCurrentTimeHTTP() {
    time_t now = time(nullptr);
    tm* gmt = gmtime(&now);
    char buffer[100];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);
    return string(buffer);
}

// 解析HTTP请求报文
ParsedRequest ParseHttpRequest(const char* request) {
    ParsedRequest parsedRequest;
    istringstream requestStream(request);
    string line;

    // 解析请求行
    if (getline(requestStream, line)) {
        istringstream lineStream(line);
        lineStream >> parsedRequest.method >> parsedRequest.uri >> parsedRequest.httpVersion;
    }

    // 解析请求头
    while (getline(requestStream, line) && line != "\r") {
        size_t colonPos = line.find(':');
        if (colonPos != string::npos) {
            string key = line.substr(0, colonPos);
            string value = line.substr(colonPos + 1);
            // 去除首尾空白字符
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t\r") + 1);
            parsedRequest.headers[key] = value;
        }
    }

    cout << "Request Method: " << parsedRequest.method << endl;
    cout << "Request URI: " << parsedRequest.uri << endl;
    cout << "HTTP Version: " << parsedRequest.httpVersion << endl;

    return parsedRequest;
}

// 构建标准的HTTP响应头
string BuildResponseHeader(int statusCode, const string& contentType, long contentLength = -1, const string& location = "") {
    stringstream header;

    // 状态行
    switch (statusCode) {
    case 200: header << "HTTP/1.1 200 OK\r\n"; break;
    case 301: header << "HTTP/1.1 301 Moved Permanently\r\n"; break;
    case 400: header << "HTTP/1.1 400 Bad Request\r\n"; break;
    case 403: header << "HTTP/1.1 403 Forbidden\r\n"; break;
    case 404: header << "HTTP/1.1 404 Not Found\r\n"; break;
    case 500: header << "HTTP/1.1 500 Internal Server Error\r\n"; break;
    default: header << "HTTP/1.1 500 Internal Server Error\r\n"; break;
    }

    // 标准响应头
    header << "Server: SimpleHTTPServer/1.0\r\n";
    header << "Date: " << getCurrentTimeHTTP() << "\r\n";
    header << "Connection: close\r\n";

    // 特殊头部
    if (!location.empty()) {
        header << "Location: " << location << "\r\n";
    }

    if (contentLength >= 0) {
        header << "Content-Length: " << contentLength << "\r\n";
    }

    if (!contentType.empty()) {
        header << "Content-Type: " << contentType << "\r\n";
    }

    header << "\r\n";  // 空行分隔头部和实体

    return header.str();
}

// 返回错误信息并关闭连接
void SendErrorResponse(SOCKET clientSocket, const sockaddr_in& clientAddr, int errorCode, const string& path) {
    string errorMessage;
    string contentType = "text/html; charset=utf-8";
    string location = "";

    switch (errorCode) {
    case 400:
        errorMessage = "<html><head><title>400 Bad Request</title></head><body><h1>400 Bad Request</h1><p>Your browser sent a request that this server could not understand.</p></body></html>";
        break;
    case 403:
        errorMessage = "<html><head><title>403 Forbidden</title></head><body><h1>403 Forbidden</h1><p>You don't have permission to access this resource.</p></body></html>";
        break;
    case 404:
        errorMessage = "<html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1><p>The requested URL " + path + " was not found on this server.</p></body></html>";
        break;
    case 301:
        if (UrlMap.find(path) != UrlMap.end()) {
            location = UrlMap[path];
        }
        errorMessage = "The document has moved <a href=\"" + location + "\">here</a>.";
        contentType = "text/html; charset=utf-8";
        break;
    default:
        errorMessage = "<html><head><title>500 Internal Server Error</title></head><body><h1>500 Internal Server Error</h1><p>The server encountered an internal error and was unable to complete your request.</p></body></html>";
        break;
    }

    cout << "Response: " << errorCode << endl;
    WriteLog(inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), to_string(errorCode), path);

    string responseHeader = BuildResponseHeader(errorCode, contentType, errorMessage.length(), location);
    string fullResponse = responseHeader + errorMessage;

    send(clientSocket, fullResponse.c_str(), fullResponse.length(), 0);
    closesocket(clientSocket);
}

// 获取文件大小
long getFileSize(ifstream& file) {
    file.seekg(0, ios::end);
    long size = file.tellg();
    file.seekg(0, ios::beg);
    return size;
}

// 处理HTTP请求
void HandleHttpRequest(SOCKET clientSocket, const ServerConfig& serverConfig) {
    char buffer[8192];  // 增大缓冲区
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead == SOCKET_ERROR || bytesRead == 0) {
        cerr << "Receive data failed or connection closed!" << endl;
        closesocket(clientSocket);
        return;
    }
    buffer[bytesRead] = '\0';

    // 获取客户端信息
    sockaddr_in clientAddr;
    int addrLen = sizeof(clientAddr);
    getpeername(clientSocket, (struct sockaddr*)&clientAddr, &addrLen);

    cout << "Client IP: " << inet_ntoa(clientAddr.sin_addr) << endl;
    cout << "Client Port: " << ntohs(clientAddr.sin_port) << endl;

    // 解析HTTP请求
    ParsedRequest parsedRequest = ParseHttpRequest(buffer);
    string pathStr = parsedRequest.uri;

    // 安全检查：防止路径遍历攻击
    if (pathStr.find("..") != string::npos || pathStr.find("//") != string::npos) {
        SendErrorResponse(clientSocket, clientAddr, 403, pathStr);
        return;
    }

    // 只支持GET方法
    if (parsedRequest.method != "GET") {
        SendErrorResponse(clientSocket, clientAddr, 400, pathStr);
        return;
    }

    // 检查URL重定向
    if (UrlMap.find(pathStr) != UrlMap.end()) {
        SendErrorResponse(clientSocket, clientAddr, 301, pathStr);
        return;
    }

    // 处理根路径请求
    if (pathStr == "/") {
        pathStr = "/index.html";
    }

    // 构造完整文件路径
    string filePath = serverConfig.HOME + pathStr;

    // 安全检查：确保文件在HOME目录内
    if (filePath.find(serverConfig.HOME) != 0) {
        SendErrorResponse(clientSocket, clientAddr, 403, pathStr);
        return;
    }

    ifstream fileStream(filePath, ios::binary);
    if (!fileStream.is_open()) {
        SendErrorResponse(clientSocket, clientAddr, 404, pathStr);
        return;
    }

    // 获取文件信息
    long fileSize = getFileSize(fileStream);

    // 获取文件扩展名
    string fileExtension = ".txt";
    size_t dotPos = pathStr.rfind('.');
    if (dotPos != string::npos) {
        fileExtension = pathStr.substr(dotPos);
    }

    // 获取Content-Type
    string contentType = contentTypes.count(fileExtension) ?
        contentTypes[fileExtension] : "application/octet-stream";

    // 构建响应头
    string responseHeader = BuildResponseHeader(200, contentType, fileSize);

    cout << "Response: 200 OK, File: " << pathStr << ", Size: " << fileSize << " bytes" << endl;

    // 发送响应头
    if (send(clientSocket, responseHeader.c_str(), responseHeader.length(), 0) == SOCKET_ERROR) {
        cerr << "Send header failed!" << endl;
        fileStream.close();
        closesocket(clientSocket);
        return;
    }

    // 发送文件内容
    long totalSent = 0;
    while (!fileStream.eof()) {
        fileStream.read(buffer, sizeof(buffer));
        int bytesRead = fileStream.gcount();
        if (bytesRead > 0) {
            int bytesSent = send(clientSocket, buffer, bytesRead, 0);
            if (bytesSent == SOCKET_ERROR) {
                cerr << "Send data failed!" << endl;
                break;
            }
            totalSent += bytesSent;
        }
    }

    fileStream.close();

    if (totalSent == fileSize) {
        cout << "File sent successfully: " << totalSent << "/" << fileSize << " bytes" << endl;
        WriteLog(inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), "200 OK", pathStr);
    }
    else {
        cout << "File sent partially: " << totalSent << "/" << fileSize << " bytes" << endl;
        WriteLog(inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), "200 OK (Partial)", pathStr);
    }

    closesocket(clientSocket);
}

int main() {
    ServerConfig serverConfig;
    serverConfig.IP = "127.0.0.1";
    serverConfig.PORT = 5050;
    serverConfig.HOME = "./";
    serverConfig.WaitCapacity = 10;

    cout << "Server Configuration:" << endl;
    cout << "IP: " << serverConfig.IP << endl;
    cout << "Port: " << serverConfig.PORT << endl;
    cout << "Home Directory: " << serverConfig.HOME << endl;
    cout << "Wait Capacity: " << serverConfig.WaitCapacity << endl;
    cout << "----------------------------------------" << endl;

    // 初始化Winsock
    WSADATA wsaData;
    int nRc = WSAStartup(0x0202, &wsaData);
    if (nRc) {
        cerr << "Winsock startup failed with error: " << nRc << endl;
        return 1;
    }
    if (wsaData.wVersion != 0x0202) {
        cerr << "Winsock version is not correct!" << endl;
        WSACleanup();
        return 1;
    }
    cout << "Winsock startup OK!" << endl;

    // 创建监听socket
    SOCKET srvSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (srvSocket == INVALID_SOCKET) {
        cerr << "Create listen socket failed!" << endl;
        WSACleanup();
        return 1;
    }
    cout << "Create listen socket success!" << endl;

    // 设置服务器地址和端口
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(serverConfig.IP.c_str());
    addr.sin_port = htons(serverConfig.PORT);

    // 绑定socket
    if (bind(srvSocket, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR) {
        cerr << "Socket bind failed!" << endl;
        closesocket(srvSocket);
        WSACleanup();
        return 1;
    }
    cout << "Socket bind OK!" << endl;

    // 开始监听
    if (listen(srvSocket, serverConfig.WaitCapacity) == SOCKET_ERROR) {
        cerr << "Socket listen failed!" << endl;
        closesocket(srvSocket);
        WSACleanup();
        return 1;
    }
    cout << "Socket listen OK!" << endl;

    cout << "HTTP server is running on http://" << serverConfig.IP << ":" << serverConfig.PORT << " ..." << endl;
    cout << "Home directory: " << serverConfig.HOME << endl;
    cout << "Press Ctrl+C to stop the server." << endl;
    cout << "========================================" << endl;

    // 主服务循环
    while (true) {
        sockaddr_in clientAddr;
        int addrLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(srvSocket, (LPSOCKADDR)&clientAddr, &addrLen);

        if (clientSocket == INVALID_SOCKET) {
            cerr << "Accept failed!" << endl;
            continue;
        }

        cout << "========================================" << endl;
        cout << "New connection accepted!" << endl;

        // 处理HTTP请求
        HandleHttpRequest(clientSocket, serverConfig);
    }

    // 清理资源
    closesocket(srvSocket);
    WSACleanup();
    return 0;
}