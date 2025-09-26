#include "Keyboard.h"

bool Keyboard::KeyIsPressed(unsigned char keycode) const { return keystates[keycode]; }

Keyboard::Event Keyboard::ReadKey()
{
    if (!keyBuffer.empty()) { Event e = keyBuffer.front(); keyBuffer.pop(); return e; }
    else return Event();
}

bool Keyboard::KeyIsempty() const { return keyBuffer.empty(); }

char Keyboard::ReadChar()
{
    if (!charbuffer.empty()) { char c = charbuffer.front(); charbuffer.pop(); return c; }
    else return 0;
}

bool Keyboard::CharIsempty() const { return charbuffer.empty(); }

void Keyboard::FlushKey() { keyBuffer = std::queue<Event>(); }
void Keyboard::FlushChar() { charbuffer = std::queue<char>(); }
void Keyboard::EnableAutorepeat() { autorepeatEnabled = true; }
void Keyboard::DisableAutorepeat() { autorepeatEnabled = false; }
bool Keyboard::AutorepeatIsEnabled() const { return autorepeatEnabled; }

void Keyboard::OnKeyPressed(unsigned char keycode) { keystates[keycode] = true; keyBuffer.push(Event(Event::Type::Press, keycode)); TrimBuffer(keyBuffer); }
void Keyboard::OnKeyReleased(unsigned char keycode) { keystates[keycode] = false; keyBuffer.push(Event(Event::Type::Release, keycode)); TrimBuffer(keyBuffer); }
void Keyboard::OnChar(char character) { charbuffer.push(character); TrimBuffer(charbuffer); }
void Keyboard::ClearState() { keystates.reset(); }

template<typename T>
void Keyboard::TrimBuffer(std::queue<T>& buffer) { while (buffer.size() > bufferSize) buffer.pop(); }
