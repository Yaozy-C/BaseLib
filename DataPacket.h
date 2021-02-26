//
// Created by Yaozy on 2019/10/9.
//

#ifndef DATAPACKET_H
#define DATAPACKET_H

#include <string>
#include <vector>
#include <type_traits>
#include <iostream>
#include "cJSON.h"

using namespace std;
namespace Base {

    class DataPacket;

    void GetJsonValue(cJSON *node, string &value);

    void GetJsonValue(cJSON *node, int &value);

    void GetJsonValue(cJSON *node, double &value);

    void GetJsonValue(cJSON *node, bool &value);

    void GetJsonValue(cJSON *node, DataPacket &value);

    template<typename T>
    void GetJsonValue(cJSON *node, vector<T> &value) {
        value.clear();
        if (!cJSON_IsNull(node) && cJSON_IsArray(node)) {
            int ArraySize = cJSON_GetArraySize(node);
            for (int i = 0; i < ArraySize; ++i) {
                cJSON *item = cJSON_GetArrayItem(node, i);
                T temp;
                GetJsonValue(item, temp);
                value.emplace_back(temp);
            }
        }
    }

    template<typename T>
    void GetJsonValue(cJSON *node, const char *nodeName, T &value) {
        cJSON *temp = cJSON_GetObjectItem(node, nodeName);
        GetJsonValue(temp, value);
    }

    cJSON *cJSON_Create(string &value);

    cJSON *cJSON_Create(int &value);

    cJSON *cJSON_Create(double &value);

    cJSON *cJSON_Create(bool &value);

    cJSON *cJSON_Create(float &value);

    cJSON *cJSON_Create(DataPacket &value);

    template<typename T>
    void AddJsonValue(cJSON *node, const char *nodeName, T &Value) {
        cJSON *item = cJSON_Create(Value);
        cJSON_AddItemToObject(node, nodeName, item);
    }

    template<typename T>
    void AddJsonValue(cJSON *node, const char *nodeName, vector<T> &value) {
        cJSON *obj = cJSON_CreateArray();
        for (int i = 0; i < value.size(); ++i) {
            cJSON *inode = cJSON_Create(value[i]);
            cJSON_AddItemToArray(obj, inode);
        }
        cJSON_AddItemToObject(node, nodeName, obj);
    }

    class DataPacket {
    private:

    public:
        DataPacket() = default;

        ~DataPacket() = default;

//        virtual void Clear();

        virtual string GetJsonText();

        virtual void SetJsonText(const string &text);

        virtual cJSON *CreateWriter();

        virtual void EncodeJson(cJSON *writer) = 0;

        virtual void DecodeJson(cJSON *reader) = 0;
    };


    template<typename T>
    class ObjectList : public DataPacket {
    private:
        vector<T> list;
    public:
        ObjectList() = default;

        ~ObjectList() = default;

        T &operator[](int index) {
            return list[index];
        };

        void Add(T &item) {
            list.emplace_back(item);
        };

        void DecodeJson(cJSON *reader) override {
            list.clear();
            if (cJSON_IsArray(reader)) {
                int ArraySize = cJSON_GetArraySize(reader);
                for (int i = 0; i < ArraySize; ++i) {
                    cJSON *Item = cJSON_GetArrayItem(reader, i);
                    T tmp;
                    GetJsonValue(Item, tmp);
                    list.emplace_back(tmp);
                }
            }
        };

        void EncodeJson(cJSON *writer) override {
            for (int i = 0; i < list.size(); ++i) {
                cJSON *item = cJSON_Create(list[i]);
                cJSON_AddItemToArray(writer, item);
            }
        };

        cJSON *CreateWriter() override {
            return cJSON_CreateArray();
        };

        int Count() {
            return list.size();
        };

        vector<T> &Items() {
            return list;
        };
    };
}

#endif //DATAPACKET_H
