/*
    ==============================================================================

    Copyright 2019 - Paul Ferrand (paulfd@outlook.fr)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include <functional>
#include <type_traits>
/**
 * A simple callback holder watching for the visibility of a JUCE component
 * Instantiate with a component and a callback and let it handle the rest!
 */
template<class T>
struct SimpleVisibilityWatcher: public ComponentMovementWatcher
{
    static_assert(std::is_base_of<Component, T>::value);
    SimpleVisibilityWatcher() = delete;
    SimpleVisibilityWatcher(T& component, std::function<void()> callback)
    : ComponentMovementWatcher(&component), callback(callback) { }
    void componentMovedOrResized (bool, bool) override {}
    void componentPeerChanged () override {}
    void componentVisibilityChanged () override { callback(); }
private:
    std::function<void()> callback;
};

/**
 * An object holding a callback responding to a specific change in a value tree.
 */
struct ValueTreeChangedListener: public ValueTree::Listener
{
    ValueTreeChangedListener() = delete;
    ValueTreeChangedListener(const Identifier& id, std::function<void(const var&)> callback)
    : id(id), callback(callback) { }
    void valueTreePropertyChanged (ValueTree& vt, const Identifier &property) override
    {
        if (property == id)
            callback(vt.getProperty(property));
    }
    // Unused
    void valueTreeChildAdded (ValueTree&, ValueTree&) override {}
    void valueTreeChildRemoved (ValueTree&, ValueTree&, int) override {}
    void valueTreeChildOrderChanged (ValueTree&, int, int) override {};
    void valueTreeParentChanged (ValueTree&) override {}
    void valueTreeRedirected (ValueTree&) override {}
private:
    const Identifier& id;
    std::function<void(const var&)> callback;
};