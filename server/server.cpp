/**
* @file
* @brief Server服务端类cpp文件.
* @author ruoxi
*
* 实现了Server服务端类.
*/

#include "server.h"
#include "utility.h"
#include "user_data_file.h"
#include "model.h"

LPCWSTR Server::s_UserFileName = TEXT("user.dat");

/**
* @return Server类的唯一实例.
*
* 用户只能通过该函数获得Server类的唯一实例.
* 若Server类还未被初始化, 则创建唯一的Server类对象,
* 否则直接返回该对象.
*/
Server &Server::GetInstance()
{
    if (!s_Initialized)
    {
        s_Instance = new Server;
        s_Initialized = true;
    }
    return *s_Instance;
}

void Server::DestroyInstance()
{
    if (s_Initialized)
    {
        delete s_Instance;
        s_Instance = NULL;
        s_Initialized = false;
    }
}

/**
* 将Server类的唯一构造函数声明为私有,
* 可以保证编译器不会帮助生成其他构造函数,
* 并且该类不可以显式的实例化.
* 该构造函数并不完成具体的初始化,
* 初始化工作通过Init函数来完成.
*/
Server::Server()
:
m_Platform(Platform::GetInstance()),
m_Logger(NULL)
{
}

Server *Server::s_Instance = NULL;

bool Server::s_Initialized = false;

Server::~Server()
{
}

/**
* @return 结果代码.
*
* 完成Server对象的初始化.
* 它对Socket环境进行初始化,
* 对平台进行初始化,
* 对TcpSocket对象进行初始化.
* 并且设置好默认的服务端端口.
* 当中任何一个步骤出现错误则直接返回对应的结果代码,
* 全部成功返回OK.
*/
RC Server::Init()
{
    RC rc;

    CHECK_RC(Socket::Init());

    // CHECK_RC(m_Platform.Init());
    CHECK_RC(m_Socket.Init());

    m_Port = 10086;

    Log(TEXT("服务器初始化完成."));

    return rc;
}

/**
* @return 结果代码.
*
* 完成Server对象的销毁.
* 它先销毁TcpSocket对象,
* 然后销毁平台,
* 最后清理Socket环境.
* 当中任何一个步骤出现错误则直接返回对应的结果代码,
* 全部成功返回OK.
*/
RC Server::Shut()
{
    RC rc;

    CHECK_RC(m_Socket.Shut());
    CHECK_RC(m_Platform.Shut());

    CHECK_RC(Socket::Shut());

    Log(TEXT("服务器销毁完成."));

    return rc;
}

/**
* @return 结果代码.
*
* 开辟一个系统线程专门进行监听,
* 以使主界面能够同时响应用户交互,
* 避免假死.
*/
RC Server::Listen()
{
    RC rc;

    HANDLE handle;
    handle = CreateThread(NULL,
        NULL,
        ThreadListen,
        (LPVOID)this,
        0,
        0);

    if(handle == INVALID_HANDLE_VALUE)
    {
        return RC::THREAD_CREATE_ERROR;
    }

    return OK;
}

/**
* @return 结果代码.
*
* 令服务器进入监听服务状态. 能够监听并服务1或多个客户端.
* 当有客户端连入, 接受连接, 得到客户端Socket.
* 创建一个ServiceThread对象, 将客户端Socket传入初始化该ServiceThread对象.
* 再通过系统调用创建出一个服务线程用于服务该客户端,
* 线程运行ThreadService函数, 将该ServiceThread对象作为参数传入.
* 具体的服务将由线程函数Service来完成.
*/
RC Server::RealListen()
{
    RC rc;

    CHECK_RC(m_Socket.Listen(m_Port));

    Log(TEXT("服务开始监听."));

    while (true)
    {
        TcpSocket *clientSocket = new TcpSocket();

        CHECK_RC(m_Socket.Accept(*clientSocket));

        Log(TEXT("接收到一个新连接."));
        ServiceThread *thread = new ServiceThread(clientSocket);

        HANDLE handle;
        handle = CreateThread(NULL,
            NULL,
            ThreadService,
            (LPVOID)thread,
            0,
            0);
        if(handle == INVALID_HANDLE_VALUE)
        {
            return RC::THREAD_CREATE_ERROR;
        }
    }
    return OK;
}

/**
* @param port 服务端端口.
*/
void Server::SetPort(int port)
{
    m_Port = port;
}

/**
* @param logger 从服务端应用程序传来的日志记录器.
*/
void Server::SetLogger(Logger *logger)
{
    m_Logger = logger;
}

/**
* @param msg 要写入日志的信息.
*/
void Server::Log(LPCWSTR msg)
{
    if (m_Logger == NULL)
    {
        return;
    }
    m_Logger->Log(msg);
}

/**
* @param clientSocket 客户端Socket.
*
* 接收一个客户端Socket对象初始化服务线程对象.
* 将m_IsLogined初始化为false.
* 并初始化用来服务客户端的TcpSocket对象.
*/
Server::ServiceThread::ServiceThread(TcpSocket *clientSocket)
:
m_IsLogined(false),
m_ClientSocket(clientSocket),
m_Id(s_Counter++)
{
    Log(TEXT("开始服务."));
}

Server::ServiceThread::~ServiceThread()
{
    m_ClientSocket->Shut();
    delete m_ClientSocket;
    Log(TEXT("退出服务."));
}

RC Server::ServiceThread::RecvCommand(CC &cc)
{
    return m_ClientSocket->RecvCommand(cc);
}

/**
* @return 结果代码.
*
* 服务用户的登录请求.
* 从客户端Socket接收一组用户名及密码,
* 查询用户数据文件, 判断用户名密码是否正确,
* 并发送对应的结果代码告之客户端.
* 若登陆成功, 则再置m_IsLogined为true, 表示已经登录.
*/
RC Server::ServiceThread::OnLogin()
{
    RC rc;

    wstring name, password;
    CHECK_RC(m_ClientSocket->RecvWString(name));
    CHECK_RC(m_ClientSocket->RecvWString(password));

    UserDataFile userDataFile(s_UserFileName);
    CHECK_RC(userDataFile.Parse());

    RC _rc = userDataFile.QueryUser(name, password);
    if (_rc == OK)
    {
        m_IsLogined = true;
        m_UserName = name;
    }
    CHECK_RC(m_ClientSocket->SendRC(_rc));

    wstring pathName = TEXT(".//") + m_UserName;
    if (!Utility::DirectoryExists(pathName.c_str()))
    {
        CreateDirectory(pathName.c_str(), NULL);
    }

    wstring msg = TEXT("用户") + m_UserName + TEXT("已登录.");
    Log(msg.c_str());

    return rc;
}

/**
* @return 结果代码.
*
* 服务用户的注册请求.
* 从客户端Socket接收一组用户名, 密码及确认密码.
* 查询用户数据文件, 判断用户是否已存在,
* 并发送对应的结果代码告之客户端.
* 若注册成功, 则更新用户数据文件, 将新的用户名密码添加.
* 之后为该用户创建该用户在服务器上的工作目录,
* 用于存放用户上传的模型或数据文件, 以及算法运行产生的数据结果文件.
*/
RC Server::ServiceThread::OnRegister()
{
    RC rc;

    wstring name, password;
    CHECK_RC(m_ClientSocket->RecvWString(name));
    CHECK_RC(m_ClientSocket->RecvWString(password));

    UserDataFile userDataFile(s_UserFileName);

    RC _rc = userDataFile.InsertUser(name, password);
    CHECK_RC(m_ClientSocket->SendRC(_rc));

    wstring msg = TEXT("注册用户") + m_UserName + TEXT("成功.");
    Log(msg.c_str());

    return rc;
}

/**
* @return 结果代码.
*
* 服务用户的发送模型文件请求.
* 创建用户名+.mod的模型文件存放于用户的工作目录中.
* 然后调用平台功能, 解析该模型文件,
* 在服务端重建出该模型, 并调用算法运行该模型(FIXME),
* 之后将产生的数据文件发送回客户端.
* 最后发送结果代码,
* 若前述步骤都无问题, 则返回OK.
* 否则发送对应的错误信息告知用户.
*/
RC Server::ServiceThread::OnSendModelFile()
{
    RC rc;

    wstring fileName = m_UserName + TEXT(".mod");
    RC _rc = m_ClientSocket->RecvFile(fileName.c_str());

    if (_rc == OK)
    {
        CFile modFile(fileName.c_str(), CFile::modeRead);
        CArchive ar(&modFile, CArchive::load);
        Model model;
        _rc = model.Load(ar);
        CHECK_RC(m_ClientSocket->SendRC(_rc));
        if (OK == _rc)
        {
            Utility::PromptMessage(TEXT("模型文件解析成功."));
            wstring datFileName = m_UserName + TEXT(".dat");
            wofstream os(datFileName.c_str());
            model.Run();
            os.close();
            _rc = m_ClientSocket->SendFile(datFileName.c_str());
            if (_rc == OK)
            {
                Utility::PromptMessage(TEXT("数据文件发送成功."));
            }
            else
            {
                Utility::PromptMessage(TEXT("数据文件发送失败."));
            }
        }
        else
        {
            Utility::PromptErrorMessage(TEXT("模型文件解析失败."));
        }
    }

    wstring msg = TEXT("接收用户") + m_UserName + TEXT("模型文件成功.");
    Log(msg.c_str());

    return _rc;
}

RC Server::ServiceThread::OnUploadFile()
{
    RC rc;

    wstring pathName = TEXT(".//") + m_UserName + TEXT("//");

    wstring fileName;
    CHECK_RC(m_ClientSocket->RecvWString(fileName));
    fileName = pathName + fileName;
    CHECK_RC(m_ClientSocket->RecvFile(fileName.c_str()));

    wstring msg = TEXT("响应用户") + m_UserName + TEXT("上传文件成功.");
    Log(msg.c_str());

    return rc;
}

RC Server::ServiceThread::OnDownloadFile()
{
    RC rc;

    wstring fileName;
    CHECK_RC(m_ClientSocket->RecvWString(fileName));

    fileName = TEXT(".//") + m_UserName + TEXT("//") + fileName;

    RC _rc;
    if (!Utility::FileExists(fileName.c_str()))
    {
        _rc = RC::DOWNLOAD_UNEXISTEFD_FILE_ERROR;
        CHECK_RC(m_ClientSocket->SendRC(_rc));
        return _rc;
    }
    CHECK_RC(m_ClientSocket->SendRC(_rc));

    CHECK_RC(m_ClientSocket->SendFile(fileName.c_str()));

    wstring msg = TEXT("响应用户")+ m_UserName + TEXT("下载文件成功.");
    Log(msg.c_str());

    return rc;
}

RC Server::ServiceThread::OnGetFileList()
{
    RC rc;

    CFileFind fileFind;
    wstring path = TEXT(".//") + m_UserName + TEXT("//*.*");
    BOOL found = fileFind.FindFile(path.c_str());
    while(found)  
    {  
        found = fileFind.FindNextFile();
        if (!fileFind.IsDots() &&
            !fileFind.IsDirectory())
        {
            wstring fileName(fileFind.GetFileName());
            CHECK_RC(m_ClientSocket->SendWString(fileName.c_str()));
        }
    }
    CHECK_RC(m_ClientSocket->SendWString(TEXT(" ")));

    return rc;
}

RC Server::ServiceThread::OnLogout()
{
    RC rc;

    RC _rc;
    CHECK_RC(m_ClientSocket->SendRC(_rc));

    wstring msg = TEXT("用户") + m_UserName + TEXT("已退出.");
    Log(msg.c_str());

    return rc;
}

void Server::ServiceThread::Log(LPCWSTR msg)
{
    CString realMsg = TEXT("服务线程");
    realMsg.AppendFormat(TEXT("%u: %s."), m_Id, msg);
    Server &server = Server::GetInstance();
    server.Log(realMsg);
}

UINT32 Server::ServiceThread::s_Counter = 0;

/**
* @param lparam 实际类型为Server对象的指针, 是一个具体的服务端实例.
*
* 通过传入的Server对象执行真正的监听函数.
*/
DWORD WINAPI Server::ThreadListen(LPVOID lparam)
{
    Server *server = (Server *)lparam;

    server->RealListen();

    return 0;
}

/**
* @param lparam 实际类型为ServiceThread, 是一个具体的服务线程对象.
*
* 每个服务端线程都通过该函数服务一个特定的服务线程对象,
* 即服务一个具体的客户端.
* 该函数不断接收客户端发来的服务请求命令,
* 解析该命令并调用对应的功能服务函数,
* 如登陆, 注册等等.
* 直至客户端发来退出请求, 该函数退出,
* 同时线程结束其使命.
*/
DWORD WINAPI Server::ThreadService(LPVOID lparam)
{
    ServiceThread *thread = (ServiceThread *)lparam;

    CC cc;
    while (true)
    {
        RC rc;
        CHECK_RC_MSG_NR(thread->RecvCommand(cc));
        if (OK != rc)
        {
            delete thread;
            return 1;
        }
        switch (cc.Get())
        {
        case CC::LOGIN_COMMAND:
            thread->OnLogin();
            break;
        case CC::REGISTER_COMMAND:
            thread->OnRegister();
            break;
        case CC::SEND_MODEL_FILE_COMMAND:
            thread->OnSendModelFile();
            break;
        case CC::UPLOAD_FILE_COMMAND:
            thread->OnUploadFile();
            break;
        case CC::GET_FILE_LIST_COMMAND:
            thread->OnGetFileList();
            break;
        case CC::DOWNLOAD_FILE_COMMAND:
            thread->OnDownloadFile();
            break;
        case CC::LOGOUT_COMMAND:
            thread->OnLogout();
            break;
        default:
            break;
        }
        if (CC::LOGOUT_COMMAND == cc.Get())
        {
            delete thread;
            break;
        }
    }

    return 0;
}
