/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-11 by Raw Material Software Ltd.

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

#ifndef __JUCE_OPENGLCONTEXTVERSIONSPECIFICATION_JUCEHEADER__
#define __JUCE_OPENGLCONTEXTVERSIONSPECIFICATION_JUCEHEADER__


//==============================================================================
/**
    Represents the attributes that specify an OpenGL context version.

    @see OpenGLContext::setContextVersionSpecification
*/
class JUCE_API  OpenGLContextVersionSpecification
{
public:
    //==============================================================================
    /** Creates an OpenGLContextVersionSpecification.

        The default constructor gives the same behaviour as creating a context
        without specifying any attributes would. On windows it will create a 
        compatibility context of the latest version of OpenGL available on your
        hardware. On OSX 10.7 this will create a context with version 2.1.

        Note: Starting with OSX 10.7, a 3.2 core context is available. Earlier
        versions only support 2.1.
    */
    OpenGLContextVersionSpecification (uint8 majorContextVersion = uint8 (0),
                                       uint8 minorContextVersion = uint8 (0),
                                       bool useCoreProfile = false) noexcept;

    bool operator== (const OpenGLContextVersionSpecification&) const noexcept;
    bool operator!= (const OpenGLContextVersionSpecification&) const noexcept;

    //==============================================================================
    uint8 contextMajorVersion;      /**< The major context version to use when creating a context. */
    uint8 contextMinorVersion;      /**< The major context version to use when creating a context. */

    bool contextUseCoreProfile;     /**< Flag for whether to create a context with a core profile or a compatibility profile. */
};


#endif   // __JUCE_OPENGLCONTEXTVERSIONSPECIFICATION_JUCEHEADER__
