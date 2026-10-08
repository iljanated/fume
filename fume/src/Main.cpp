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
        // Maak het hoofdvenster aan wanneer de app start
        mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }

    void shutdown() override
    {
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
};

// Macro die de main()-functie genereert en de app opstart
START_JUCE_APPLICATION(TracktionAppApplication)
