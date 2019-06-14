#-------------------------------------------------
#
# Project created by QtCreator 2019-06-14T05:33:33
#
#-------------------------------------------------
#QT      += core gui
#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT        -= core gui
TEMPLATE   = app
TARGET     = RhythmBox
CONFIG    += link_pkgconfig
PKGCONFIG += alsa freetype2 x11 xext xinerama webkit2gtk-4.0 gtk+-x11-3.0 libcurl
QMAKE_CXXFLAGS += -std=c++17

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS \
           LINUX=1 DEBUG=1 _DEBUG=1 JUCE_APP_VERSION=1.0.0 \
           JUCE_APP_VERSION_HEX=0x10000 \
           JucePlugin_Build_VST=0 JucePlugin_Build_VST3=0 JucePlugin_Build_AU=0 \
           JucePlugin_Build_AUv3=0 JucePlugin_Build_RTAS=0 JucePlugin_Build_AAX=0 \
           JucePlugin_Build_Standalone=1 JucePlugin_Build_Unity=0

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += /usr/include/freetype2 $$_PRO_FILE_PWD_/../../../Source \
               $$_PRO_FILE_PWD_/../../../JuceLibraryCode $(HOME)/JUCE/modules
LIB_BUILDDIR = $$OUT_PWD/../SharedCode
LIBS        += -m64 -ldl -lpthread -lrt -L"$$LIB_BUILDDIR" -lRhythmBox
QMAKE_CFLAGS = -Wall -Wno-strict-aliasing -Wno-strict-overflow \
               -m64 -O0 -mstackrealign -fPIC -g

HEADERS += ./../../../JuceLibraryCode/AppConfig.h \
           ./../../../JuceLibraryCode/BinaryData.h \
           ./../../../JuceLibraryCode/JuceHeader.h \
           ./../../../Source/BeatDescription.h \
           ./../../../Source/BeatSequence.h \
           ./../../../Source/Configuration.h \
           ./../../../Source/FileSearchers.h \
           ./../../../Source/PluginEditor.h \
           ./../../../Source/PluginProcessor.h \
           ./../../../Source/RhythmChooser.h \
           ./../../../Source/SFZero.h \
           ./../../../Source/SfzChooser.h \
           ./../../../Source/Watchers.h \
           ./../../../Source/sfzero/RIFF.h \
           ./../../../Source/sfzero/SF2.h \
           ./../../../Source/sfzero/SF2Generator.h \
           ./../../../Source/sfzero/SF2Reader.h \
           ./../../../Source/sfzero/SF2Sound.h \
           ./../../../Source/sfzero/SF2WinTypes.h \
           ./../../../Source/sfzero/SFZCommon.h \
           ./../../../Source/sfzero/SFZDebug.h \
           ./../../../Source/sfzero/SFZEG.h \
           ./../../../Source/sfzero/SFZReader.h \
           ./../../../Source/sfzero/SFZRegion.h \
           ./../../../Source/sfzero/SFZSample.h \
           ./../../../Source/sfzero/SFZSound.h \
           ./../../../Source/sfzero/SFZSynth.h \
           ./../../../Source/sfzero/SFZVoice.h \
           ./../../../Source/sfzero/sf2-chunks/generators.h \
           ./../../../Source/sfzero/sf2-chunks/ibag.h \
           ./../../../Source/sfzero/sf2-chunks/igen.h \
           ./../../../Source/sfzero/sf2-chunks/imod.h \
           ./../../../Source/sfzero/sf2-chunks/inst.h \
           ./../../../Source/sfzero/sf2-chunks/iver.h \
           ./../../../Source/sfzero/sf2-chunks/pbag.h \
           ./../../../Source/sfzero/sf2-chunks/pgen.h \
           ./../../../Source/sfzero/sf2-chunks/phdr.h \
           ./../../../Source/sfzero/sf2-chunks/pmod.h \
           ./../../../Source/sfzero/sf2-chunks/shdr.h \
           ./../../../Source/spdlog/async.h \
           ./../../../Source/spdlog/async_logger.h \
           ./../../../Source/spdlog/common.h \
           ./../../../Source/spdlog/details/async_logger_impl.h \
           ./../../../Source/spdlog/details/circular_q.h \
           ./../../../Source/spdlog/details/console_globals.h \
           ./../../../Source/spdlog/details/file_helper.h \
           ./../../../Source/spdlog/details/fmt_helper.h \
           ./../../../Source/spdlog/details/log_msg.h \
           ./../../../Source/spdlog/details/logger_impl.h \
           ./../../../Source/spdlog/details/mpmc_blocking_q.h \
           ./../../../Source/spdlog/details/null_mutex.h \
           ./../../../Source/spdlog/details/os.h \
           ./../../../Source/spdlog/details/pattern_formatter.h \
           ./../../../Source/spdlog/details/periodic_worker.h \
           ./../../../Source/spdlog/details/registry.h \
           ./../../../Source/spdlog/details/thread_pool.h \
           ./../../../Source/spdlog/fmt/bin_to_hex.h \
           ./../../../Source/spdlog/fmt/bundled/chrono.h \
           ./../../../Source/spdlog/fmt/bundled/color.h \
           ./../../../Source/spdlog/fmt/bundled/core.h \
           ./../../../Source/spdlog/fmt/bundled/format-inl.h \
           ./../../../Source/spdlog/fmt/bundled/format.h \
           ./../../../Source/spdlog/fmt/bundled/locale.h \
           ./../../../Source/spdlog/fmt/bundled/ostream.h \
           ./../../../Source/spdlog/fmt/bundled/posix.h \
           ./../../../Source/spdlog/fmt/bundled/printf.h \
           ./../../../Source/spdlog/fmt/bundled/ranges.h \
           ./../../../Source/spdlog/fmt/bundled/time.h \
           ./../../../Source/spdlog/fmt/fmt.h \
           ./../../../Source/spdlog/fmt/ostr.h \
           ./../../../Source/spdlog/formatter.h \
           ./../../../Source/spdlog/logger.h \
           ./../../../Source/spdlog/sinks/android_sink.h \
           ./../../../Source/spdlog/sinks/ansicolor_sink.h \
           ./../../../Source/spdlog/sinks/base_sink.h \
           ./../../../Source/spdlog/sinks/basic_file_sink.h \
           ./../../../Source/spdlog/sinks/daily_file_sink.h \
           ./../../../Source/spdlog/sinks/dist_sink.h \
           ./../../../Source/spdlog/sinks/msvc_sink.h \
           ./../../../Source/spdlog/sinks/null_sink.h \
           ./../../../Source/spdlog/sinks/ostream_sink.h \
           ./../../../Source/spdlog/sinks/rotating_file_sink.h \
           ./../../../Source/spdlog/sinks/sink.h \
           ./../../../Source/spdlog/sinks/stdout_color_sinks.h \
           ./../../../Source/spdlog/sinks/stdout_sinks.h \
           ./../../../Source/spdlog/sinks/syslog_sink.h \
           ./../../../Source/spdlog/sinks/wincolor_sink.h \
           ./../../../Source/spdlog/spdlog.h \
           ./../../../Source/spdlog/tweakme.h \
           ./../../../Source/spdlog/version.h
SOURCES += ./../../../JuceLibraryCode/BinaryData.cpp \
           ./../../../JuceLibraryCode/include_juce_audio_basics.cpp \
           ./../../../JuceLibraryCode/include_juce_audio_devices.cpp \
           ./../../../JuceLibraryCode/include_juce_audio_formats.cpp \
           ./../../../JuceLibraryCode/include_juce_audio_plugin_client_AAX.cpp \
           ./../../../JuceLibraryCode/include_juce_audio_plugin_client_RTAS_1.cpp \
           ./../../../JuceLibraryCode/include_juce_audio_plugin_client_RTAS_2.cpp \
           ./../../../JuceLibraryCode/include_juce_audio_plugin_client_RTAS_3.cpp \
           ./../../../JuceLibraryCode/include_juce_audio_plugin_client_RTAS_4.cpp \
           ./../../../JuceLibraryCode/include_juce_audio_plugin_client_RTAS_utils.cpp \
           ./../../../JuceLibraryCode/include_juce_audio_plugin_client_Standalone.cpp \
           ./../../../JuceLibraryCode/include_juce_audio_plugin_client_Unity.cpp \
           ./../../../JuceLibraryCode/include_juce_audio_plugin_client_VST2.cpp \
           ./../../../JuceLibraryCode/include_juce_audio_plugin_client_VST3.cpp \
           ./../../../JuceLibraryCode/include_juce_audio_plugin_client_utils.cpp \
           ./../../../JuceLibraryCode/include_juce_audio_processors.cpp \
           ./../../../JuceLibraryCode/include_juce_audio_utils.cpp \
           ./../../../JuceLibraryCode/include_juce_core.cpp \
           ./../../../JuceLibraryCode/include_juce_cryptography.cpp \
           ./../../../JuceLibraryCode/include_juce_data_structures.cpp \
           ./../../../JuceLibraryCode/include_juce_events.cpp \
           ./../../../JuceLibraryCode/include_juce_graphics.cpp \
           ./../../../JuceLibraryCode/include_juce_gui_basics.cpp \
           ./../../../JuceLibraryCode/include_juce_gui_extra.cpp \
           ./../../../Source/BeatDescription.cpp \
           ./../../../Source/BeatSequence.cpp \
           ./../../../Source/Configuration.cpp \
           ./../../../Source/FileSearchers.cpp \
           ./../../../Source/PluginEditor.cpp \
           ./../../../Source/PluginProcessor.cpp \
           ./../../../Source/RhythmChooser.cpp \
           ./../../../Source/SFZero.cpp \
           ./../../../Source/SfzChooser.cpp \
           ./../../../Source/sfzero/RIFF.cpp \
           ./../../../Source/sfzero/SF2.cpp \
           ./../../../Source/sfzero/SF2Generator.cpp \
           ./../../../Source/sfzero/SF2Reader.cpp \
           ./../../../Source/sfzero/SF2Sound.cpp \
           ./../../../Source/sfzero/SFZDebug.cpp \
           ./../../../Source/sfzero/SFZEG.cpp \
           ./../../../Source/sfzero/SFZReader.cpp \
           ./../../../Source/sfzero/SFZRegion.cpp \
           ./../../../Source/sfzero/SFZSample.cpp \
           ./../../../Source/sfzero/SFZSound.cpp \
           ./../../../Source/sfzero/SFZSynth.cpp \
           ./../../../Source/sfzero/SFZVoice.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
