cmake_minimum_required(VERSION 3.22)

project(OneKnobSaturator VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Download e integrazione JUCE 8
include(FetchContent)
FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.4
)
FetchContent_MakeAvailable(juce)

# 2. Configurazione Asset Grafici (Sfondo e Knob)
juce_add_binary_data(PluginAssets
    SOURCES
        Resources/background_oneknob.jpg
        Resources/knob_oneknob.png
)

# 3. Definizione del Plugin
juce_add_plugin(OneKnobSaturator
    COMPANY_NAME "DAEBAER Plugins"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Daeb
    PLUGIN_CODE Oks1
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "OneKnobSaturator"
)

# Generazione Header JUCE
juce_generate_juce_header(OneKnobSaturator)

# 4. Sorgenti del progetto
target_sources(OneKnobSaturator
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
)

# 5. Link delle Librerie JUCE e Asset Binari
target_link_libraries(OneKnobSaturator
    PRIVATE
        PluginAssets
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# 6. Flag di Compilazione
target_compile_definitions(OneKnobSaturator
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)