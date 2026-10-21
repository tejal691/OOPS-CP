#ifndef UI_H
#define UI_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include "Statistics.h"
using namespace std;

// UI class = everything that is DRAWN on the screen.
// main.cpp only decides WHAT to show, this class decides HOW it looks.
class UI
{
private:
    sf::RenderWindow& window;
    sf::Font& font;

    // ---------- small helpers ----------
    static sf::Color rgba(int r, int g, int b, int a = 255)
    {
        return sf::Color(
            static_cast<std::uint8_t>(max(0, min(255, r))),
            static_cast<std::uint8_t>(max(0, min(255, g))),
            static_cast<std::uint8_t>(max(0, min(255, b))),
            static_cast<std::uint8_t>(max(0, min(255, a))));
    }

    // rectangle with rounded corners
    static sf::ConvexShape rect(sf::Vector2f pos, sf::Vector2f size,
                                float r, sf::Color fill)
    {
        const int seg = 8;
        sf::ConvexShape s;
        s.setPointCount(4 * (seg + 1));

        sf::Vector2f center[4] = {{r, r}, {size.x - r, r},
                                  {size.x - r, size.y - r}, {r, size.y - r}};
        float start[4] = {180, 270, 0, 90};
        size_t n = 0;

        for(int c = 0; c < 4; c++)
            for(int i = 0; i <= seg; i++)
            {
                float a = (start[c] + 90.f * i / seg) * 3.14159265f / 180.f;
                s.setPoint(n++, {center[c].x + r * cos(a),
                                 center[c].y + r * sin(a)});
            }

        s.setPosition(pos);
        s.setFillColor(fill);
        return s;
    }

    // rounded panel with shadow
    void panel(sf::Vector2f pos, sf::Vector2f size, float r,
               sf::Color fill, sf::Color outline, float thick = 2)
    {
        window.draw(rect({pos.x, pos.y + 6}, size, r, rgba(0, 0, 0, 70)));
        sf::ConvexShape p = rect(pos, size, r, fill);
        p.setOutlineThickness(thick);
        p.setOutlineColor(outline);
        window.draw(p);
    }

    void text(const string& str, unsigned size, sf::Color c, float x, float y)
    {
        sf::Text t(font, str, size);
        t.setFillColor(c);
        t.setPosition({x, y});
        window.draw(t);
    }

    // text centred inside a box
    void textIn(const string& str, unsigned size, sf::Color c,
                sf::Vector2f pos, sf::Vector2f box)
    {
        sf::Text t(font, str, size);
        t.setFillColor(c);
        sf::FloatRect b = t.getLocalBounds();
        t.setPosition({pos.x + (box.x - b.size.x) / 2 - b.position.x,
                       pos.y + (box.y - b.size.y) / 2 - b.position.y});
        window.draw(t);
    }

    float adv(char c, unsigned size)
    {
        return font.getGlyph(static_cast<unsigned char>(c), size, false).advance;
    }

    void card(const string& label, const string& value, sf::Color color, float x)
    {
        panel({x, 462}, {205, 68}, 14, rgba(20, 12, 60, 150),
              rgba(255, 255, 255, 60), 1);
        window.draw(rect({x + 10, 474}, {5, 44}, 2, color));
        text(label, 13, rgba(210, 210, 255), x + 26, 471);
        text(value, 28, color, x + 26, 488);
    }

public:
    static constexpr float W = 1000;
    static constexpr float BTN_Y = 570;
    static constexpr float BTN_W = 190;
    static constexpr float BTN_H = 60;
    static constexpr float BTN_X[4] = {75, 295, 515, 735};

    UI(sf::RenderWindow& w, sf::Font& f) : window(w), font(f) {}

    // which button is at this mouse position? (0-3, or -1 = none)
    int buttonAt(int x, int y)
    {
        if(y < BTN_Y || y > BTN_Y + BTN_H)
            return -1;
        for(int i = 0; i < 4; i++)
            if(x >= BTN_X[i] && x <= BTN_X[i] + BTN_W)
                return i;
        return -1;
    }

    // ---------- calm STATIC background (no movement, no bubbles) ----------
    void background(float)
    {
        sf::Color top = rgba(35, 55, 125);       // deep blue
        sf::Color bottom = rgba(95, 55, 150);    // soft purple

        sf::VertexArray bg(sf::PrimitiveType::TriangleStrip, 4);
        bg[0].position = {0, 0};   bg[0].color = top;
        bg[1].position = {W, 0};   bg[1].color = top;
        bg[2].position = {0, 700}; bg[2].color = bottom;
        bg[3].position = {W, 700}; bg[3].color = bottom;
        window.draw(bg);
    }

    // ---------- rainbow title ----------
    void title(float t)
    {
        const string name = "TYPEMASTER";
        sf::Color rainbow[6] = {rgba(255, 99, 132), rgba(255, 159, 64),
                                rgba(255, 220, 90), rgba(110, 240, 150),
                                rgba(90, 200, 255), rgba(190, 140, 255)};

        float total = 0;
        for(char c : name) total += adv(c, 44) + 4;

        float x = (W - total) / 2;
        for(size_t i = 0; i < name.size(); i++)
        {
            float y = 17 + 4 * sin(t * 2 + i * 0.6f);
            text(string(1, name[i]), 44, rgba(0, 0, 0, 100), x + 3, y + 3);
            text(string(1, name[i]), 44, rainbow[i % 6], x, y);
            x += adv(name[i], 44) + 4;
        }
    }

    // ---------- passage number, mode, instruction ----------
    void header(int passageNo, bool timedMode, bool started, float t)
    {
        window.draw(rect({60, 88}, {170, 34}, 17, rgba(255, 205, 60, 235)));
        textIn("Passage " + to_string(passageNo), 19, rgba(60, 30, 80),
               {60, 88}, {170, 34});

        window.draw(rect({680, 88}, {260, 34}, 17,
                    timedMode ? rgba(255, 110, 150, 235) : rgba(60, 220, 240, 235)));
        textIn(timedMode ? "Mode: Timed - 60 Seconds" : "Mode: Practice", 18,
               rgba(40, 20, 70), {680, 88}, {260, 34});

        int alpha = started ? 230 : static_cast<int>(185 + 70 * sin(t * 3));
        textIn(started ? "Type the passage..." : "Press ENTER to start typing", 21,
               rgba(255, 245, 160, alpha), {0, 126}, {W, 30});
    }

    // ---------- passage card (green = correct, red = wrong) ----------
    void passage(const string& target, const string& input, bool typing, float t)
    {
        panel({60, 158}, {880, 182}, 20, rgba(20, 12, 60, 175),
              rgba(255, 255, 255, 90));

        vector<sf::Vector2f> pos;
        unsigned size = 24;

        // word-wrap layout, returns bottom of the text
        auto layout = [&](unsigned s) -> float
        {
            pos.assign(target.size(), sf::Vector2f(0, 0));
            float x = 80, y = 168, lineH = s * 1.45f;
            size_t i = 0;

            while(i < target.size())
            {
                if(target[i] == ' ')
                {
                    pos[i] = {x, y};
                    x += adv(' ', s) + 1;
                    i++;
                    continue;
                }

                size_t j = i;
                float w = 0;
                while(j < target.size() && target[j] != ' ')
                    w += adv(target[j++], s) + 1;

                if(x + w > 920 && x > 80) { x = 80; y += lineH; }

                for(size_t k = i; k < j; k++)
                {
                    pos[k] = {x, y};
                    x += adv(target[k], s) + 1;
                }
                i = j;
            }
            return y + lineH;
        };

        while(layout(size) > 336 && size > 14)
            size -= 2;

        bool blink = (static_cast<int>(t * 2) % 2 == 0);

        for(size_t i = 0; i < target.size(); i++)
        {
            float w = adv(target[i], size) + 1, h = size * 1.3f;
            sf::Color color = rgba(235, 235, 255);

            if(i < input.size())
            {
                bool ok = (input[i] == target[i]);
                color = ok ? rgba(130, 255, 165) : rgba(255, 110, 130);

                if(!ok)   // red box behind wrong letter
                {
                    sf::RectangleShape box({w, h});
                    box.setPosition({pos[i].x, pos[i].y + 3});
                    box.setFillColor(rgba(255, 60, 90, 90));
                    window.draw(box);
                }
            }
            else if(i == input.size() && typing)   // current letter
            {
                sf::RectangleShape box({w, h});
                box.setPosition({pos[i].x, pos[i].y + 3});
                box.setFillColor(rgba(255, 220, 80, blink ? 110 : 40));
                window.draw(box);
            }

            if(target[i] != ' ')
                text(string(1, target[i]), size, color, pos[i].x, pos[i].y);
        }
    }

    // ---------- what the user has typed + progress bar ----------
    void inputBox(const string& input, size_t targetLen, bool started, float t)
    {
        panel({60, 352}, {880, 80}, 16, rgba(12, 8, 45, 200),
              started ? rgba(90, 240, 255, 190 + 65 * sin(t * 4))
                      : rgba(255, 255, 255, 110), 3);

        // split typed text into lines (shows the last 2 lines)
        vector<string> lines;
        string line;
        float width = 0;

        for(char c : input)
        {
            float w = adv(c, 22);
            if(width + w > 840) { lines.push_back(line); line = ""; width = 0; }
            line += c;
            width += w;
        }
        lines.push_back(line);

        if(input.empty())
            text("Your typing appears here...", 22, rgba(200, 200, 255, 110), 80, 372);

        size_t first = lines.size() > 2 ? lines.size() - 2 : 0;
        for(size_t i = first; i < lines.size(); i++)
            text(lines[i], 22, rgba(120, 240, 255), 80, 362 + (i - first) * 30);

        // progress bar
        float p = targetLen ? (float)input.size() / targetLen : 0;
        sf::RectangleShape track({880, 8}), fill({880 * p, 8});
        track.setPosition({60, 441});
        track.setFillColor(rgba(255, 255, 255, 60));
        fill.setPosition({60, 441});
        fill.setFillColor(rgba(110 + 120 * p, 240 - 40 * p, 160));
        window.draw(track);
        window.draw(fill);
    }

    // ---------- 4 statistic cards ----------
    void stats(bool timedMode, bool started, float elapsed, Statistics& s)
    {
        auto f2 = [](float v) { string x = to_string(v); return x.substr(0, x.find('.') + 3); };

        sf::Color timeColor = rgba(255, 215, 100);
        string label = "TIME", value = f2(elapsed) + " sec";

        if(timedMode)
        {
            int left = max(0, 60 - (int)elapsed);
            label = "TIME LEFT";
            value = to_string(left) + " sec";
            if(started && left <= 10) timeColor = rgba(255, 100, 120);
        }

        float acc = s.getAccuracy();
        sf::Color accColor = acc < 70 ? rgba(255, 110, 130)
                           : acc < 90 ? rgba(255, 215, 100)
                                      : rgba(130, 255, 165);

        card(label, value, timeColor, 60);
        card("WPM", f2(s.getWPM()), rgba(100, 215, 255), 285);
        card("ACCURACY", f2(acc) + "%", accColor, 510);

        panel({735, 462}, {205, 68}, 14, rgba(20, 12, 60, 150),
              rgba(255, 255, 255, 60), 1);
        text("CORRECT / WRONG", 13, rgba(210, 210, 255), 751, 471);
        text(to_string(s.getCorrect()), 28, rgba(130, 255, 165), 751, 488);
        text(to_string(s.getWrong()), 28, rgba(255, 110, 130), 847, 488);
    }

    // ---------- 4 coloured buttons ----------
    void buttons(sf::Vector2i mouse, bool timedMode)
    {
        const char* names[4] = {"Practice", "Timed", "Next", "Save Result"};
        sf::Color base[4] = {rgba(52, 152, 219), rgba(255, 140, 60),
                             rgba(46, 204, 113), rgba(155, 89, 182)};

        for(int i = 0; i < 4; i++)
        {
            bool hover = (buttonAt(mouse.x, mouse.y) == i);
            bool active = (i == 0 && !timedMode) || (i == 1 && timedMode);
            float y = BTN_Y - (hover ? 3 : 0);
            sf::Color c = hover ? rgba(base[i].r + 35, base[i].g + 35, base[i].b + 35)
                                : base[i];

            window.draw(rect({BTN_X[i], BTN_Y + 5}, {BTN_W, BTN_H}, 16, rgba(0, 0, 0, 90)));

            sf::ConvexShape b = rect({BTN_X[i], y}, {BTN_W, BTN_H}, 16, c);
            if(active) { b.setOutlineThickness(3); b.setOutlineColor(sf::Color::White); }
            window.draw(b);

            textIn(names[i], 21, sf::Color::White, {BTN_X[i], y}, {BTN_W, BTN_H});
        }
    }

    // ---------- bottom message ----------
    void footer(bool finished)
    {
        if(finished)
        {
            window.draw(rect({150, 642}, {700, 46}, 23, rgba(255, 205, 60, 240)));
            textIn("TEST COMPLETED! Result saved.", 24, rgba(60, 30, 80),
                   {150, 642}, {700, 46});
        }
        else
            textIn("Tip: Backspace fixes mistakes  |  Timed mode keeps going to the next passage",
                   15, rgba(255, 255, 255, 150), {0, 650}, {W, 20});
    }
};

#endif