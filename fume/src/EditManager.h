#pragma once

#include <JuceHeader.h>

namespace te = tracktion;

class EditManager : private juce::Timer
{
public:
    struct Listener
    {
        virtual ~Listener() = default;
		virtual void onEditUnloaded() = 0;
        virtual void onEditLoaded(te::Edit* newEdit) = 0;
    };

    explicit EditManager(te::Engine& engineToUse)
        : engine(engineToUse)
    {
    }

    ~EditManager() override
    {
        stopTimer();
    }

    void addListener(Listener* l) { listeners.add(l); }
    void removeListener(Listener* l) { listeners.remove(l); }

    void createOrLoadNewEdit(const juce::File& targetFile);

    void loadEdit(const juce::File& fileToLoad);

    void saveEdit(bool warnOfFailure = true, std::function<void(bool)> onComplete = nullptr);

    void saveEditAs(const juce::File& newFile, std::function<void(bool)> onComplete = nullptr);

    void setAutoSaveEnabled(bool shouldEnable);

    bool isAutoSaveEnabled() const noexcept { return autoSaveEnabled; }
    bool isSaveInProgress() const noexcept { return isSaving; }

    void clearActiveEdit() noexcept;

private:
    //==============================================================================
    void timerCallback() override;

    void initialiseEdit();

    void performAtomicAutoSave();
    te::Engine& engine;
    std::unique_ptr<te::Edit> activeEdit = nullptr;
    juce::File currentEditFile;
    bool autoSaveEnabled = false;
    bool isSaving = false;
    juce::ListenerList<Listener> listeners;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditManager)
};