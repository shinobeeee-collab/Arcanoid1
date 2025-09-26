#pragma once
#include <queue>
#include <bitset>

class Keyboard
{
public:
    class Event
    {
    public:
        enum class Type { Press, Release, Invalid };
        Type type = Type::Invalid;
        unsigned char code = 0;

        Event() = default;
        Event(Type t, unsigned char c) : type(t), code(c) {}
        bool IsPress() const { return type == Type::Press; }
        bool IsRelease() const { return type == Type::Release; }
        bool IsValid() const { return type != Type::Invalid; }
        unsigned char GetCode() const { return code; }
    };

private:
    static constexpr unsigned int bufferSize = 16;
    std::bitset<256> keystates;
    std::queue<Event> keyBuffer;
    std::queue<char> charbuffer;
    bool autorepeatEnabled = false;

public:
    bool KeyIsPressed(unsigned char keycode) const;
    Event ReadKey();
    bool KeyIsempty() const;
    char ReadChar();
    bool CharIsempty() const;
    void FlushKey();
    void FlushChar();
    void EnableAutorepeat();
    void DisableAutorepeat();
    bool AutorepeatIsEnabled() const;
    void OnKeyPressed(unsigned char keycode);
    void OnKeyReleased(unsigned char keycode);
    void OnChar(char character);
    void ClearState();

private:
    template<typename T> void TrimBuffer(std::queue<T>& buffer);
};
