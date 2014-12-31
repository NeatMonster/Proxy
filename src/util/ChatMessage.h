#ifndef __Proxy__ChatMessage__
#define __Proxy__ChatMessage__

#include "Types.h"

#include "json11/json11.hpp"

class ChatMessage {
public:
    class Color {
    public:
        static Color BLACK;
        static Color DARK_BLUE;
        static Color DARK_GREEN;
        static Color DARK_AQUA;
        static Color DARK_RED;
        static Color DARK_PURPLE;
        static Color GOLD;
        static Color GRAY;
        static Color DARK_GRAY;
        static Color BLUE;
        static Color GREEN;
        static Color AQUA;
        static Color RED;
        static Color LIGHT_PURPLE;
        static Color YELLOW;
        static Color WHITE;

        string_t getName();

        bool operator!=(Color);

    private:
        string_t name;

        Color(string_t);
    };

    class Style {
    public:
        static Style RESET;
        static Style BOLD;
        static Style ITALIC;
        static Style UNDERLINED;
        static Style STRIKETHROUGH;
        static Style OBFUSCATED;
        static Style values[5];

        string_t getName();

        bool operator==(Style);

    private:
        string_t name;

        Style(string_t);
    };

    ChatMessage();

    ~ChatMessage();

    ChatMessage &operator<<(string_t);

    ChatMessage &operator<<(Color);

    ChatMessage &operator<<(Style);

    string_t getJSON();

    string_t getText();

private:
    string_t text;
    Color color;
    std::vector<Style> styles;
    ChatMessage *prev;
    ChatMessage *next;

    ChatMessage(ChatMessage*, Color, std::vector<Style>);

    std::map<string_t, json11::Json> toJSON();
};

typedef ChatMessage Chat;
typedef ChatMessage::Color Color;
typedef ChatMessage::Style Style;

#endif /* defined(__Proxy__ChatMessage__) */
