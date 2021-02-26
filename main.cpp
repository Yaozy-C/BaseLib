#include <iostream>
#include <future>
#include "Buffer.h"
#include "DataPacket.h"
#include "Pool.h"
#include "Log.h"

using namespace std;

class RfidDev : public Base::DataPacket {
public:
    RfidDev();

    RfidDev(const RfidDev &rfidDev);

    RfidDev &operator=(const RfidDev &rfidDev);

    ~RfidDev();

    void DecodeJson(cJSON *reader) override;

    void EncodeJson(cJSON *writer) override;

    string id;
    string name;
    vector<string> ip;
    int port;
};


RfidDev::RfidDev() {
    port = 0;
}

RfidDev::RfidDev(const RfidDev &rfidDev) {
    id = rfidDev.id;
    name = rfidDev.name;
    ip = rfidDev.ip;
    port = rfidDev.port;
}

RfidDev &RfidDev::operator=(const RfidDev &rfidDev) {
    if (this == &rfidDev)
        return *this;
    id = rfidDev.id;
    name = rfidDev.name;
    ip = rfidDev.ip;
    port = rfidDev.port;
    return *this;
}

RfidDev::~RfidDev() = default;

void RfidDev::DecodeJson(cJSON *reader) {
    Base::GetJsonValue(reader, "ID", id);
    Base::GetJsonValue(reader, "Name", name);
    Base::GetJsonValue(reader, "IP", ip);
    Base::GetJsonValue(reader, "Port", port);
}

void RfidDev::EncodeJson(cJSON *writer) {
    Base::AddJsonValue(writer, "ID", id);
    Base::AddJsonValue(writer, "Name", name);
    Base::AddJsonValue(writer, "IP", ip);
    Base::AddJsonValue(writer, "Port", port);
}

class B {
public:
    explicit B(int num) {
        arr = num;
    }

    B(const B &a) = delete;

    B operator=(const B &a) = delete;

    int arr;
};

class A {
public:
    explicit A(int num): index(num) {}

    A(const A &a) = delete;

    A operator=(const A &a) = delete;

    int change(B &b) {
        index += b.arr;
        cout << "index:" + to_string(index) << endl;
        return 0;
    }

    int index;
};

int main() {
    LOG_INIT_LOGGER();
    Base::ThreadPool threadPool;
    auto start = std::chrono::steady_clock::now();
    A a(0);
    B b(2);
    auto f1 = bind(&A::change, &a, ref(b));
    for (int i = 0; i < 10; ++i) {
        threadPool.AddTask(f1);
    }
    threadPool.Stop();

    auto end = std::chrono::steady_clock::now();
    auto tt = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    LOG_DEBUG(to_string(tt.count()));

    return 0;
}