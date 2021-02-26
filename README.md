# BaseLib
Buffer:用于流式数据的存储与分割；
Pool：简易线程池，会创建CPU数目-2的工作线程，且这些线程会绑定到CPU上执行；
Log：简易日志库，支持日志保存与打印，设置保存日志状态时需执行LOG_INIT_FILELOGGER（string path）；path为保存路径（会创建当天的一个log）；
IndependentThread：独立任务线程，通常作为基类使用，子类需实现IndependentThread的Run函数，IndependentThread拥有自己的工作队列，运行时会自动读取任务；
SafeQue：无边界阻塞队列；
DataPacket：Json数据基类，可实现JSON数据序列化与反序列化，子类需实现EncodeJson与DecodeJson两个函数；
LibCurl：基于libcurl的封装类，可实现http客户端功能；
