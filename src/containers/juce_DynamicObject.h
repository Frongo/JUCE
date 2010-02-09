/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-9 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

#ifndef __JUCE_DYNAMICOBJECT_JUCEHEADER__
#define __JUCE_DYNAMICOBJECT_JUCEHEADER__

#include "juce_NamedValueSet.h"
#include "juce_ReferenceCountedObject.h"


//==============================================================================
/**
    Represents a dynamically implemented object.

    This class is primarily intended for wrapping scripting language objects,
    but could be used for other purposes.

    An instance of a DynamicObject can be used to store named properties, and
    by subclassing hasMethod() and invokeMethod(), you can give your object
    methods.
*/
class JUCE_API  DynamicObject  : public ReferenceCountedObject
{
public:
    //==============================================================================
    DynamicObject();

    /** Destructor. */
    virtual ~DynamicObject();

    //==============================================================================
    /** Returns true if the object has a property with this name.
        Note that if the property is actually a method, this will return false.
    */
    virtual bool hasProperty (const var::identifier& propertyName) const;

    /** Returns a named property.

        This returns a void if no such property exists.
    */
    virtual const var getProperty (const var::identifier& propertyName) const;

    /** Sets a named property. */
    virtual void setProperty (const var::identifier& propertyName, const var& newValue);

    /** Removes a named property. */
    virtual void removeProperty (const var::identifier& propertyName);

    //==============================================================================
    /** Checks whether this object has the specified method.

        The default implementation of this just checks whether there's a property
        with this name that's actually a method, but this can be overridden for
        building objects with dynamic invocation.
    */
    virtual bool hasMethod (const var::identifier& methodName) const;

    /** Invokes a named method on this object.

        The default implementation looks up the named property, and if it's a method
        call, then it invokes it.

        This method is virtual to allow more dynamic invocation to used for objects
        where the methods may not already be set as properies.
    */
    virtual const var invokeMethod (const var::identifier& methodName,
                                    const var* parameters,
                                    int numParameters);

    /** Sets up a method.

        This is basically the same as calling setProperty (methodName, (var::MethodFunction) myFunction), but
        helps to avoid accidentally invoking the wrong type of var constructor. It also makes
        the code easier to read,

        The compiler will probably force you to use an explicit cast your method to a (var::MethodFunction), e.g.
        @code
        setMethod ("doSomething", (var::MethodFunction) &MyClass::doSomething);
        @endcode
    */
    void setMethod (const var::identifier& methodName,
                    var::MethodFunction methodFunction);

    //==============================================================================
    /** Removes all properties and methods from the object. */
    void clear();

    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    NamedValueSet properties;
};



#endif   // __JUCE_DYNAMICOBJECT_JUCEHEADER__