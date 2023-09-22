#include "ByteBuffer.h"

ByteBuffer::ByteBuffer(size_t size) : capacity(size), writePos(0), readPos(0) {
    buffer = new uint8_t[size];
}

ByteBuffer::~ByteBuffer() {
    delete[] buffer;
}

void ByteBuffer::WriteBool(bool value) {
    if (writePos + sizeof(bool) <= capacity) {
        *((bool*)(buffer + writePos)) = value;
        writePos += sizeof(bool);
    }
}

void ByteBuffer::WriteInt(int value) {
    if (writePos + sizeof(int) <= capacity) {
        *((int*)(buffer + writePos)) = value;
        writePos += sizeof(int);
    }
}

void ByteBuffer::WriteString(const String &str) {
    size_t len = str.length();
    if (writePos + sizeof(size_t) + len <= capacity) {
        WriteInt(len);
        memcpy(buffer + writePos, str.c_str(), len);
        writePos += len;
    }
}

void ByteBuffer::WriteBytes(const uint8_t* data, size_t len) {
    if (writePos + len <= capacity) {
        memcpy(buffer + writePos, data, len);
        writePos += len;
    }
}

bool ByteBuffer::ReadBool() {
    bool value = *((bool*)(buffer + readPos));
    readPos += sizeof(bool);
    return value;
}

int ByteBuffer::ReadInt() {
    int value = *((int*)(buffer + readPos));
    readPos += sizeof(int);
    return value;
}

String ByteBuffer::ReadString() {
    int len = ReadInt();
    char tmp[len + 1];
    memcpy(tmp, buffer + readPos, len);
    tmp[len] = '\0';
    readPos += len;
    return String(tmp);
}

const uint8_t* ByteBuffer::ReadBytes(size_t len) {
    if (readPos + len <= writePos) { // Ensure we don't read more than written
        const uint8_t* result = buffer + readPos;
        readPos += len;
        return result;
    }
    return nullptr;
}

uint8_t* ByteBuffer::ToArray() {
    return buffer;
}

size_t ByteBuffer::size() {
    return writePos;
}

size_t ByteBuffer::remaining() {
    return capacity - writePos;
}

void ByteBuffer::reset() {
    writePos = 0;
    readPos = 0;
}
