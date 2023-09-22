#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H

#include <Arduino.h>

class ByteBuffer {
public:
    ByteBuffer(size_t size);
    ~ByteBuffer();

    void WriteBool(bool value);
    void WriteInt(int value);
    void WriteString(const String &str);
    void WriteBytes(const uint8_t* data, size_t len);

    bool ReadBool();
    int ReadInt();
    String ReadString();
    const uint8_t* ReadBytes(size_t len);

    uint8_t* ToArray();
    size_t size();
    size_t remaining();
    void reset();

private:
    uint8_t* buffer;
    size_t capacity;
    size_t writePos;
    size_t readPos;
};

#endif // BYTEBUFFER_H
