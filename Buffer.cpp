//
// Created by Yaozy on 2021/1/7.
//

#include "Buffer.h"

namespace Base {
    Buffer::Buffer(size_t initSzie) : data(8 + initSzie) {
        headIndex = 0;
        nextHeadIndex = 0;
        readIndex = 8;
        writeIndex = 8;
    }

    size_t Buffer::WriteableBytes() const {
        return data.size() - writeIndex;
    }

    void Buffer::MakeSapce(int len) {
        if (WriteableBytes() + readIndex < len + 8) {
            data.resize(writeIndex + len);
        } else {
            size_t readable = writeIndex - readIndex;
            std::copy(data.begin() + headIndex,
                      data.begin() + writeIndex,
                      data.begin() + 8);
            readIndex = 8;
            writeIndex = readIndex + readable;
        }
    }

    void Buffer::GetHeadIndex() {
        auto iter = std::search(data.begin() + readIndex, data.begin() + writeIndex, head.begin(), head.end());
        if (iter != data.end()) {
            headIndex = distance(data.begin(), iter);
        }
        iter = std::search(data.begin() + headIndex + head.size(), data.begin() + writeIndex, head.begin(), head.end());
        if (iter != data.end()) {
            nextHeadIndex = distance(data.begin(), iter);
        }
    }

    void Buffer::Append(const std::string &newData) {
        if (WriteableBytes() < newData.size())
            MakeSapce(newData.size());
        std::copy(newData.data(), newData.data() + newData.size(), data.begin() + writeIndex);
        writeIndex += newData.size();
    }

    std::string Buffer::GetPackage() {
        GetHeadIndex();
        std::vector<char> pack;
        pack.resize(nextHeadIndex - headIndex);

        std::copy(data.begin() + headIndex, data.begin() + nextHeadIndex, pack.begin());
        readIndex += pack.size();
        std::string res(&(*pack.begin()), pack.size());
        return res;
    }

    void Buffer::SetHead(const std::string &head_) {
        head.resize(head_.size());
        std::copy(head_.data(), head_.data() + head_.size(), head.begin());
    }
}