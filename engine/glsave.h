/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

/**
   small util to ease the burden of keeping openGL states
   safe after a step : at destruction, restore a state to the value
   it had at construction time.
*/

#ifndef GL_SAVE_H
#define GL_SAVE_H

namespace vortex {

/** depth function */
class GLDepthFuncSave {
private:
    GLint mDepthFunc;

public:

    GLDepthFuncSave() {
        save();
    }

    ~GLDepthFuncSave() {
        restore();
    }

    void save() {
        glAssert(glGetIntegerv(GL_DEPTH_FUNC, &mDepthFunc));
    }

    void restore() {
        glAssert(glDepthFunc(mDepthFunc));
    }
};

#if 0

/** enable/disable */
/*
class GLEnabledSave {
private:
    GLenum m_capacity;
    GLboolean m_value;
    bool m_saved;
public:
    GLEnabledSave(GLenum capacity_, bool change = false, bool enabled = false) : m_capacity(capacity_) {
        save(change, enabled);
    }

    GLEnabledSave(bool, GLenum capacity_) : m_capacity(capacity_) {
        m_saved = false;
    }

    void save(bool change = false, bool enabled = false) {
        m_saved = true;
        glAssert(m_value = glIsEnabled(m_capacity));

        if (change) {
            if (m_value != enabled) {
                if (enabled) {
                    glAssert(glEnable(m_capacity));
                } else {
                    glAssert(glDisable(m_capacity));
                }
            }
        }
    }

    void restore() {
        if (!m_saved) {
            return;
        }
        if (glIsEnabled(m_capacity) != m_value) {
            if (m_value) {
                glAssert(glEnable(m_capacity));
            } else {
                glAssert(glDisable(m_capacity));
            }
        }
    }

    ~GLEnabledSave() {
        restore();
    }
};
*/
/** blending, saves GL_BLEND_SRC, GL_BLEND_DST, GL_BLEND_EQUATION_RGB and GL_BLEND_EQUATION_ALPHA */
class GLBlendSave {
private:
    GLint mSrc, mDst, mEqRGB, mEqAlpha;

public:
    GLBlendSave() {
        save();
    }

    GLBlendSave(bool) {
        mSrc = -1;
    }

    void save() {
        glAssert(glGetIntegerv(GL_BLEND_SRC, &mSrc));
        glAssert(glGetIntegerv(GL_BLEND_DST, &mDst));
        glAssert(glGetIntegerv(GL_BLEND_EQUATION_RGB, &mEqRGB));
        glAssert(glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &mEqAlpha));
    }

    void restore() {
        if (mSrc != -1) {
            glAssert(glBlendEquationSeparate(mEqRGB, mEqAlpha));
            glAssert(glBlendFunc(mSrc, mDst));
        }
    }

    ~GLBlendSave() {
        restore();
    }
};

#if 0
/** the currently active matrix mode. */
class GLMatrixModeSave {
private:
    GLint mMode;
public:

    GLMatrixModeSave() {
        save();
    }

    void save() {
        glAssert(glGetIntegerv(GL_MATRIX_MODE, &mMode));
    }

    void restore() {
        glAssert(glMatrixMode(mMode));
    }

    ~GLMatrixModeSave() {
        restore();
    }
};
#endif
/** current clear colors */
class GLClearColorSave {
private:

    float mColor[4];

public:

    GLClearColorSave() {
        save();
    }

    ~GLClearColorSave() {
        restore();
    }

    void save() {
        glAssert(glGetFloatv(GL_COLOR_CLEAR_VALUE, mColor));
    }

    void restore() {
        glAssert(glClearColor(mColor[0], mColor[1], mColor[2], mColor[3]));
    }
};

/** current clear depth */
class GLClearDepthSave {
private:

    float mDepth;

public:

    GLClearDepthSave() {
        save();
    }

    ~GLClearDepthSave() {
        restore();
    }

    void save() {
        glAssert(glGetFloatv(GL_DEPTH_CLEAR_VALUE, &mDepth));
    }

    void restore() {
        glAssert(glClearDepth(mDepth));
    }
};

/** current clear stencil index */
class GLClearStencilSave {
private:

    GLint mIndex;

public:

    GLClearStencilSave() {
        save();
    }

    ~GLClearStencilSave() {
        restore();
    }

    void save() {
        glAssert(glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &mIndex));
    }

    void restore() {
        glAssert(glClearStencil(mIndex));
    }
};

/** current depth mask */
class GLDepthMaskSave {
private:

    GLboolean mDepthMask;

public:

    GLDepthMaskSave() {
        save();
    }

    ~GLDepthMaskSave() {
        restore();
    }

    void save() {
        glAssert(glGetBooleanv(GL_DEPTH_WRITEMASK, &mDepthMask));
    }

    void restore() {
        glAssert(glDepthMask(mDepthMask));
    }
};





/** cull face mode */
class GLCullFaceSave {
private:
    GLint mCull;

public:

    GLCullFaceSave() {
        save();
    }

    ~GLCullFaceSave() {
        restore();
    }

    void save() {
        glAssert(glGetIntegerv(GL_CULL_FACE_MODE, &mCull));
    }

    void restore() {
        glAssert(glCullFace(mCull));
    }
};


/** front face mode */
class GLFrontFaceSave {
private:
    GLint mCull;

public:

    GLFrontFaceSave() {
        save();
    }

    ~GLFrontFaceSave() {
        restore();
    }

    void save() {
        glAssert(glGetIntegerv(GL_FRONT_FACE, &mCull));
    }

    void restore() {
        glAssert(glFrontFace(mCull));
    }
};
#if 0
/** texture modulation mode */
class GLTexEnvModeSave {
private:

    GLint mTexEnvMode;

public:

    GLTexEnvModeSave() {
        save();
    }

    ~GLTexEnvModeSave() {
        restore();
    }

    void save() {
        glAssert(glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &mTexEnvMode));
    }

    void restore() {
        glAssert(glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mTexEnvMode));
    }
};
#endif
/** DrawBuffer */
class GLDrawBufferSave {
private:

    GLint mDrawBuffer;

public:

    GLDrawBufferSave() {
        save();
    }

    GLDrawBufferSave(bool) {
        mDrawBuffer = -1;
    }

    ~GLDrawBufferSave() {
        restore();
    }

    void save() {
        glAssert(glGetIntegerv(GL_DRAW_BUFFER, &mDrawBuffer));
    }

    void restore() {
        if (mDrawBuffer != -1) {
            glAssert(glDrawBuffer(mDrawBuffer));
        }
    }
};
#endif

} // namespace vortex


#endif
