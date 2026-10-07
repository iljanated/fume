#include <SDL3/SDL.h>
#include <iostream>
#include <atomic>
#include <thread>

#include <JuceHeader.h>
#include "MainComponent.h"

class TracktionAppApplication : public juce::JUCEApplication
{
public:
    TracktionAppApplication() {}

    const juce::String getApplicationName() override { return "Fume"; }
    const juce::String getApplicationVersion() override { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String& commandLine) override
    {
		// gamepadloop thread starten
        inputThread = std::thread([this]() {
            runGamepadInputLoop();
            });


        // Maak het hoofdvenster aan wanneer de app start
        mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }

    void shutdown() override
    {
        isRunning = false;
        inputThread.join();

        // Ruim het venster netjes op bij het afsluiten
        mainWindow = nullptr;

    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted(const juce::String& commandLine) override {}

    //==============================================================================
    // Het hoofdvenster van de applicatie
    //==============================================================================
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name)
            : DocumentWindow(name,
                juce::Desktop::getInstance().getDefaultLookAndFeel()
                .findColour(juce::ResizableWindow::backgroundColourId),
                DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent(), true);

#if JUCE_MINIMISE_WINDOW_FIRST_AND_FOREMOST
            setConstrainer(&minimiseConstrainer);
#endif

            setResizable(true, true);
            centreWithSize(getWidth(), getHeight());
            setVisible(true);
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
	std::atomic<bool> isRunning{ true }; // for gamepadloop thread
    std::thread inputThread;

    void runGamepadInputLoop() {
        // 1. Allow the application to capture gamepad inputs while running in the background
        SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

        // 2. Initialize ONLY the Gamepad subsystem
        if (!SDL_InitSubSystem(SDL_INIT_GAMEPAD)) {
            DBG("Failed to init SDL Gamepad: " << SDL_GetError());
            return;
        }
        std::cout << "SDL3 Gamepad Subsystem ready." << std::endl;

        SDL_Event event;
        while (isRunning) {
            // 3. Pump and poll OS events safely
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                case SDL_EVENT_GAMEPAD_ADDED: {
                    // Open the device by its instance identifier
                    SDL_Gamepad* gamepad = SDL_OpenGamepad(event.gdevice.which);
                    if (gamepad) {
						DBG("Controller Connected: " << SDL_GetGamepadName(gamepad));
                    }
                    break;
                }
                case SDL_EVENT_GAMEPAD_REMOVED: {
					DBG("Controller Disconnected: " << event.gdevice.which);
                    break;
                }
                case SDL_EVENT_GAMEPAD_BUTTON_DOWN: {
                    // 4. Map actions cleanly to tracktion_engine transport layers
                    if (event.gbutton.button == SDL_GAMEPAD_BUTTON_SOUTH) { // 'A' on Xbox, 'Cross' on PS
                        DBG("Trigger: Start/Pause Track Playback");
                        // Example: tracktionEngine.getPlaybackControl().togglePlay();
                    }
                    break;
                }
                case SDL_EVENT_GAMEPAD_AXIS_MOTION: {
                    // Thumbstick axis values range from -32768 to 32767
                    // You can use these to control tracks fader volumes or pan parameters
                    int16_t axisValue = event.gaxis.value;
                    if (event.gaxis.axis == SDL_GAMEPAD_AXIS_LEFTX && abs(axisValue) > 8000) { // Deadzone filter
                        DBG("Left Stick Motion: " << axisValue);
                    }
                    break;
                }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(8)); // ~120Hz polling rate
        }

        // 5. Cleanup gracefully on exit
        SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
    }
};

// Macro die de main()-functie genereert en de app opstart
START_JUCE_APPLICATION(TracktionAppApplication)
