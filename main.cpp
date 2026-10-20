#include <SFML/Graphics.hpp>
#include <iostream>
#include <optional>
#include <stdexcept>
#include "TypingSession.h"
#include "PracticeMode.h"
#include "TimedChallenge.h"
#include "PassageManager.h"
#include "Statistics.h"
#include "FileManager.h"
#include "UI.h"

using namespace std;

int main()
{
    try
    {
        // ---------- SETUP ----------
        PassageManager passageManager;
        passageManager.loadPassages("prompts.txt");

        sf::RenderWindow window(sf::VideoMode({1000, 700}),
                                "TypeMaster - Typing Speed & Accuracy Trainer");
        window.setFramerateLimit(60);

        sf::Font font;
        if(!font.openFromFile("C:/Windows/Fonts/segoeui.ttf"))
            throw runtime_error("Error: segoeui.ttf not found!");

        UI ui(window, font);                       // draws the screen

        int passageNumber = 0;                     // zero-based index
        string targetText = passageManager.getPassage(passageNumber);
        string userInput = "";

        PracticeMode practice(targetText);         // two types of sessions
        TimedChallenge timed(targetText, 60);
        TypingSession* session = &practice;        // polymorphism

        Statistics statistics;
        FileManager fileManager;

        bool started = false, finished = false, timedMode = false;
        float finalTime = 0;

        sf::Clock timer;                           // typing timer
        sf::Clock animClock;                       // animation timer

        // ---------- helper actions ----------

        // load the current passage in the current mode and reset everything
        auto newSession = [&]()
        {
            targetText = passageManager.getPassage(passageNumber);

            if(timedMode) { timed = TimedChallenge(targetText, 60); session = &timed; }
            else          { practice = PracticeMode(targetText);    session = &practice; }

            userInput = "";
            started = finished = false;
            finalTime = 0;
            statistics = Statistics();
        };

        // calculate result, save it in CSV file and end the test
        auto finish = [&](float seconds)
        {
            finalTime = seconds;
            statistics.calculate(targetText, userInput, seconds);

            fileManager.saveResult(timedMode ? "Timed" : "Practice",
                                   statistics.getWPM(), statistics.getAccuracy(),
                                   statistics.getCorrect(), statistics.getWrong());
            finished = true;
            started = false;
        };

        // ---------- MAIN LOOP ----------
        while(window.isOpen())
        {
            // ----- EVENTS -----
            while(const optional event = window.pollEvent())
            {
                if(event->is<sf::Event::Closed>())
                    window.close();

                // mouse click on a button
                if(const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
                {
                    int button = ui.buttonAt(mouse->position.x, mouse->position.y);

                    if(button == 0) { timedMode = false; newSession(); }        // Practice
                    if(button == 1) { timedMode = true;  newSession(); }        // Timed
                    if(button == 2)                                             // Next
                    {
                        passageNumber = (passageNumber + 1) % passageManager.getCount();
                        newSession();
                    }
                    if(button == 3 && (started || finished))                    // Save
                    {
                        float seconds = (started && !finished)
                                        ? timer.getElapsedTime().asSeconds() : finalTime;
                        finish(seconds <= 0 ? 0.01f : seconds);
                    }
                }

                // keyboard typing
                if(const auto* textEvent = event->getIf<sf::Event::TextEntered>())
                {
                    if(started && !finished)
                    {
                        unsigned int ch = textEvent->unicode;

                        if(ch == 8 && !userInput.empty())                       // backspace
                            userInput.pop_back();
                        else if(ch >= 32 && ch < 127 &&
                                userInput.length() < targetText.length())       // letter
                            userInput += static_cast<char>(ch);

                        session->setUserInput(userInput);

                        if(userInput == targetText)
                        {
                            if(!timedMode)                                      // practice done
                            {
                                float s = timer.getElapsedTime().asSeconds();
                                finish(s <= 0 ? 0.01f : s);
                            }
                            else                                                // timed: next passage,
                            {                                                   // timer keeps running
                                passageNumber = (passageNumber + 1) % passageManager.getCount();
                                targetText = passageManager.getPassage(passageNumber);
                                timed = TimedChallenge(targetText, 60);
                                session = &timed;
                                userInput = "";
                            }
                        }
                    }
                }

                // ENTER = start
                if(const auto* key = event->getIf<sf::Event::KeyPressed>())
                {
                    if(key->code == sf::Keyboard::Key::Enter && !started && !finished)
                    {
                        started = true;
                        userInput = "";
                        session->startSession();
                        timer.restart();
                    }
                }
            }

            // ----- UPDATE -----
            float elapsed = 0;
            if(started && !finished) elapsed = timer.getElapsedTime().asSeconds();
            else if(finished)        elapsed = finalTime;

            if(timedMode && started && !finished && elapsed >= 60)   // 60 sec over
                finish(60);

            if(started && !finished)                                 // live WPM / accuracy
                statistics.calculate(targetText, userInput, elapsed > 0 ? elapsed : 0.01f);

            // ----- DRAW -----
            float t = animClock.getElapsedTime().asSeconds();

            window.clear();
            ui.background(t);
            ui.title(t);
            ui.header(passageNumber + 1, timedMode, started, t);
            ui.passage(targetText, userInput, started && !finished, t);
            ui.inputBox(userInput, targetText.length(), started, t);
            ui.stats(timedMode, started, elapsed, statistics);
            ui.buttons(sf::Mouse::getPosition(window), timedMode);
            ui.footer(finished);
            window.display();
        }
    }
    catch(exception &e)
    {
        cout << e.what() << endl;
    }
    return 0;
}
