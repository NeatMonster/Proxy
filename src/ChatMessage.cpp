#include "ChatMessage.h"

ChatMessage::Color ChatMessage::Color::BLACK("black");
ChatMessage::Color ChatMessage::Color::DARK_BLUE("dark_blue");
ChatMessage::Color ChatMessage::Color::DARK_GREEN("dark_green");
ChatMessage::Color ChatMessage::Color::DARK_AQUA("dark_aqua");
ChatMessage::Color ChatMessage::Color::DARK_RED("dark_red");
ChatMessage::Color ChatMessage::Color::DARK_PURPLE("dark_purple");
ChatMessage::Color ChatMessage::Color::GOLD("gold");
ChatMessage::Color ChatMessage::Color::GRAY("gray");
ChatMessage::Color ChatMessage::Color::DARK_GRAY("dark_gray");
ChatMessage::Color ChatMessage::Color::BLUE("blue");
ChatMessage::Color ChatMessage::Color::GREEN("green");
ChatMessage::Color ChatMessage::Color::AQUA("aqua");
ChatMessage::Color ChatMessage::Color::RED("red");
ChatMessage::Color ChatMessage::Color::LIGHT_PURPLE("light_purple");
ChatMessage::Color ChatMessage::Color::YELLOW("yellow");
ChatMessage::Color ChatMessage::Color::WHITE("white");

string_t ChatMessage::Color::getName() {
    return name;
}

bool ChatMessage::Color::operator!=(Color color) {
    return name != color.getName();
}

ChatMessage::Color::Color(string_t name) : name(name) {}

ChatMessage::Style ChatMessage::Style::RESET("reset");
ChatMessage::Style ChatMessage::Style::BOLD("bold");
ChatMessage::Style ChatMessage::Style::ITALIC("italic");
ChatMessage::Style ChatMessage::Style::UNDERLINED("underlined");
ChatMessage::Style ChatMessage::Style::STRIKETHROUGH("strikethrough");
ChatMessage::Style ChatMessage::Style::OBFUSCATED("obfuscated");
ChatMessage::Style ChatMessage::Style::values[5] = {
    BOLD, ITALIC, UNDERLINED, STRIKETHROUGH, OBFUSCATED
};

string_t ChatMessage::Style::getName() {
    return name;
}

bool ChatMessage::Style::operator==(ChatMessage::Style style) {
    return name == style.getName();
}

ChatMessage::Style::Style(string_t name) : name(name) {}

ChatMessage::ChatMessage() : color(Color::WHITE), prev(nullptr), next(nullptr) {}

ChatMessage::~ChatMessage() {
    if (next != nullptr)
        delete next;
}

ChatMessage &ChatMessage::operator<<(string_t text) {
    if (next == nullptr)
        this->text += text;
    else
        *next << text;
    return *this;
}

ChatMessage &ChatMessage::operator<<(Style style) {
    if (next == nullptr) {
        if (style == Style::RESET) {
            if (text.empty())
                styles.clear();
            else if (!styles.empty())
                next = new ChatMessage(this, color, {});
        } else {
            if (text.empty())
                styles.push_back(style);
            else if (contains<Style>(style, styles)) {
                std::vector<Style> newStyles(styles);
                newStyles.push_back(style);
                next = new ChatMessage(this, color, newStyles);
            }
        }
    } else
        *next << style;
    return *this;
}

ChatMessage &ChatMessage::operator<<(Color color) {
    if (next == nullptr) {
        if (text.empty())
            this->color = color;
        else if (this->color != color)
            next = new ChatMessage(this, color, styles);
    } else
         *next << color;
    return *this;
}

string_t ChatMessage::getJSON() {
    std::map<string_t, json11::Json> json = toJSON();

    std::vector<std::map<string_t, json11::Json>> extra;
    ChatMessage *next = this->next;
    while (next != nullptr) {
        extra.push_back(next->toJSON());
        next = next->next;
    }
    if (!extra.empty())
        json["extra"] = extra;

    return json11::Json(json).dump();
}

string_t ChatMessage::getText() {
    return text += (next == nullptr ? "" : next->getText());
}

ChatMessage::ChatMessage(ChatMessage *prev, Color color, std::vector<Style> styles) :
    color(color), styles(styles), prev(prev), next(nullptr) {}

std::map<string_t, json11::Json> ChatMessage::toJSON() {
    std::map<string_t, json11::Json> json;

    if (text.empty())
        return json;
    json["text"] = text;

    Color prevColor = prev == nullptr ? Color::WHITE : prev->color;
    if (color != prevColor)
        json["color"] = color.getName();

    std::vector<Style> prevStyles = (prev == nullptr) ? std::vector<Style>() : prev->styles;
    for (Style style : Style::values)
        if (contains<Style>(style, styles) != contains<Style>(style, prevStyles))
            json[style.getName()] = (bool) contains<Style>(style, styles);

    return json;
}
