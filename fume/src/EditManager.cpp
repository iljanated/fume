#include "EditManager.h"
#include "Utilities.h"

namespace te = tracktion;
using namespace te::literals;

void EditManager::createOrLoadNewEdit(const juce::File& targetFile)
{
    if (targetFile.existsAsFile())
    {
        loadEdit(targetFile);
        return;
    }

    clearActiveEdit();

    currentEditFile = targetFile;

    te::Edit::Options options{ engine };
    options.editState = te::createEmptyEdit(engine);
    options.role = te::Edit::forEditing;
    options.editFileRetriever = [this] { return currentEditFile; };

    activeEdit = std::make_unique<te::Edit>(std::move(options));

    initialiseEdit();

    DBG("Created new edit: " << currentEditFile.getFullPathName());

    saveEdit(false);

    listeners.call([this](Listener& l) { l.onEditLoaded(activeEdit.get()); });
}

void EditManager::loadEdit(const juce::File& fileToLoad)
{
    clearActiveEdit();

    if (!fileToLoad.existsAsFile())
    {
        return;
    }

    currentEditFile = fileToLoad;

    auto newEdit = te::loadEditFromFile(engine, currentEditFile);
    if (newEdit == nullptr)
    {
        return;
    }

    te::Edit::Options options{ engine };
    options.editState = newEdit->state;
    options.role = te::Edit::forEditing;
    options.editFileRetriever = [this] { return currentEditFile; };

    activeEdit = std::make_unique<te::Edit>(std::move(options));

    DBG("Loaded edit from file: " << currentEditFile.getFullPathName());

    listeners.call([this](Listener& l) { l.onEditLoaded(activeEdit.get()); });
}

void EditManager::saveEdit(bool warnOfFailure, std::function<void(bool)> onComplete)
{
    if (activeEdit == nullptr || currentEditFile == juce::File() || isSaving)
    {
        if (onComplete)
        {
            onComplete(false);
        }
        return;
    }

    isSaving = true;
    activeEdit->flushState();

    te::EditFileOperations(*activeEdit).save(warnOfFailure,
        true,
        false,
        [this, onComplete](bool success)
        {
            isSaving = false;

            if (onComplete)
            {
                onComplete(success);
            }
        });

    DBG("Saving edit to file: " << currentEditFile.getFullPathName());
}

void EditManager::saveEditAs(const juce::File& newFile, std::function<void(bool)> onComplete)
{
    if (activeEdit == nullptr || isSaving)
    {
        if (onComplete)
        {
            onComplete(false);
        }
        return;
    }

    isSaving = true;
    auto previousFile = currentEditFile;
    currentEditFile = newFile;
    activeEdit->flushState();

    te::EditFileOperations(*activeEdit).saveAs(currentEditFile,
        true,
        [this, previousFile, onComplete](bool success)
        {
            isSaving = false;

            if (!success)
            {
                currentEditFile = previousFile;
            }

            if (onComplete)
            {
                onComplete(success);
            }
        });
    DBG("Saving edit as file: " << currentEditFile.getFullPathName());
}

void EditManager::setAutoSaveEnabled(bool shouldEnable)
{
    autoSaveEnabled = shouldEnable;

    if (autoSaveEnabled)
    {
        startTimer(60 * 1000);
    }
    else
    {
        stopTimer();
    }
}

void EditManager::clearActiveEdit() noexcept
{
    if (activeEdit != nullptr)
    {
        activeEdit = nullptr;
        listeners.call([](Listener& l) { l.onEditUnloaded(); });
    }
    currentEditFile = juce::File();
    isSaving = false;
    DBG("Active edit cleared.");
}

//==============================================================================
void EditManager::timerCallback()
{
    if (!autoSaveEnabled || isSaving || activeEdit == nullptr || currentEditFile == juce::File())
    {
        return;
    }

    if (activeEdit->getTransport().isPlaying() || activeEdit->getTransport().isRecording())
    {
        return;
    }

    if (activeEdit->hasChangedSinceSaved())
    {
        performAtomicAutoSave();
    }
}

void EditManager::initialiseEdit()
{
    if (activeEdit == nullptr)
    {
        return;
    }

    auto& edit = *activeEdit;
    
    edit.state.setProperty(FumeIDs::SongLength, 1024, &edit.getUndoManager());

    EngineHelpers::getOrInsertAudioTrackAt(edit, 16);


    auto synthPlugin = dynamic_cast<te::FourOscPlugin*> (edit.getPluginCache().createNewPlugin(te::FourOscPlugin::xmlTypeName, {}).get());
    auto track = te::getAudioTracks(edit)[4];
    track->pluginList.insertPlugin(synthPlugin, 0, nullptr);

    te::TempoSequence& tempoSequence = edit.tempoSequence;

    auto midiClip = track->insertMIDIClip(tempoSequence.toTime({ 0_bp, 16_bp }), nullptr);

    auto& seq = midiClip->getSequence();
    seq.addNote(48, 0_bp, 4_bd, 127, 0, nullptr);
    seq.addNote(41, 4_bp, 4_bd, 100, 0, nullptr);
    seq.addNote(59, 8_bp, 4_bd, 110, 0, nullptr);
    seq.addNote(38, 12_bp, 4_bd, 100, 0, nullptr);

    auto track2 = te::getAudioTracks(edit)[3];


    auto midiClip2 = track2->insertMIDIClip(tempoSequence.toTime({ 16_bp, 32_bp }), nullptr);

    auto& seq2 = midiClip2->getSequence();
    seq2.addNote(66, 0_bp, 4_bd, 127, 0, nullptr);
    seq2.addNote(41, 4_bp, 4_bd, 100, 0, nullptr);
    seq2.addNote(59, 8_bp, 4_bd, 110, 0, nullptr);
    seq2.addNote(38, 12_bp, 4_bd, 100, 0, nullptr);

    if (auto* tempo = edit.tempoSequence.getTempo(0))
    {
        tempo->setBpm(120.0);
    }

    edit.getUndoManager().clearUndoHistory();
}

void EditManager::performAtomicAutoSave()
{
    if (isSaving || activeEdit == nullptr || currentEditFile == juce::File())
    {
        return;
    }

    isSaving = true;
    activeEdit->flushState();

    auto tempFile = currentEditFile.getSiblingFile(currentEditFile.getFileName() + ".tmp");

    te::EditFileOperations(*activeEdit).saveAs(tempFile,
        false, // Keep relative audio paths anchored to currentEditFile
        [this, tempFile](bool success)
        {
            if (success && activeEdit != nullptr)
            {
                if (tempFile.moveFileTo(currentEditFile))
                {
                    activeEdit->resetChangedStatus();
                }
                else
                {
                    tempFile.deleteFile();
                }
            }
            else
            {
                tempFile.deleteFile();
            }
            isSaving = false;
        });
	DBG("Auto-saving edit to file: " << currentEditFile.getFullPathName());
}
