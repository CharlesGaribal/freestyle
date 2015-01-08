/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#include <iostream>
#include "shaderobject.h"

namespace vortex {


unsigned long getFileLength(std::ifstream& file)
{
    if (!file.good()) return 0;

    unsigned long curpos = file.tellg();

    file.seekg(0, std::ios::end);
    unsigned long len = file.tellg();
    file.seekg(curpos, std::ios::beg);

    return len;
}

char* loadFile(const char *fileName)
{
    std::ifstream file;
    file.open(fileName, std::ios::in);
    if (!file) {
        std::cerr << "could not open file " << fileName << std::endl;
        GLchar *shaderSource = (GLchar*) new char[1];
        shaderSource[0]=0;
        return shaderSource;
    }

    unsigned long len = getFileLength(file);
    if (len == 0) {

        GLchar *shaderSource = (GLchar*) new char[1];
        shaderSource[0]=0;
        return shaderSource;

    }

    GLchar *shaderSource = (GLchar*) new char[len+1];

    // len isn't always strlen cause some characters are stripped in ascii read...
    // it is important to 0-terminate the real length later, len is just max possible value...
    shaderSource[len] = 0;
    unsigned int i = 0;
    while (file.good()) {
        // get character from file.
        shaderSource[i] = file.get();
        if (!file.eof()) i++;
    }

    shaderSource[i] = 0;  // 0 terminate it.
    file.close();
    return shaderSource;
}

void printShaderInfoLog(GLint shader)
{
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;
    glAssert(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen));
    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        glAssert(glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog));
        std::cerr << "InfoLog:" << std::endl << infoLog << std::endl;
        delete [] infoLog;
    }
}

void printProgramInfoLog(GLint program)
{
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;
    glAssert(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen));
    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        glAssert(glGetProgramInfoLog(program, infoLogLen, &charsWritten, infoLog));
        std::cerr << "InfoLog:" << std::endl << infoLog << std::endl;
        delete [] infoLog;
    }
}



ShaderObject::ShaderObject() {
}

ShaderObject::~ShaderObject() {

}

/// @todo add more shader type
void ShaderObject::create(GLenum type)
{
    glAssert(mId = glCreateShader(type));
    mType = type;
    switch (mType) {
    case GL_VERTEX_SHADER :
        mTypeString = "Vertex ";
        break;
    case GL_FRAGMENT_SHADER :
        mTypeString = "Fragment ";
        break;
        /** @todo : find a way to define automagically these strings ...
        */
    case GL_GEOMETRY_SHADER :
        mTypeString = "Geometry ";
        break;
    case GL_TESS_CONTROL_SHADER :
        mTypeString = "Tess control ";
        break;
    case GL_TESS_EVALUATION_SHADER :
        mTypeString = "Tess eval  ";
        break;
    default:
        std::cerr <<"shader type " << type << "not supported"<<std::endl;
        break;
    }

}

void ShaderObject::load(std::string fileName)
{
    mFileName = fileName;
    char* code = loadFile(fileName.c_str());
    const char *fullCode[3];
#ifdef __APPLE__
    fullCode[0] = "#version 410\n#define __APPLE__\n";
#else
    fullCode[0] = "#version 410\n";
#endif
    fullCode[1] = mConfigurationString.c_str();
    fullCode[2] = code;
 // std :: cerr << fullCode[0] << std::endl << fullCode[1] << std::endl << fullCode[2] << std::endl << std::endl << std::endl << std::endl << std::endl;
    source(fullCode, 3);
    delete [] code;
}

void ShaderObject::source(const char** codeStrings, int nbStrings)
{
    glAssert(glShaderSource(mId, nbStrings, (const GLchar **) codeStrings, NULL));
}

void ShaderObject::compile()
{
    glAssert(glCompileShader(mId));
}

int ShaderObject::check()
{
    GLint compiled;
    glAssert(glGetShaderiv(mId, GL_COMPILE_STATUS, &compiled));
    if (!compiled) {
        std::cerr << mTypeString << " shader not compiled : " << mFileName << std::endl;
        printShaderInfoLog(mId);
        return 0;
    }
    return 1;

}

//      // detach the texture
//      mFBO->detachTexture(GL_DEPTH_ATTACHMENT);
//      mFBO->unbind();
//      // set the shader
//      ShaderConfiguration depthConfig(ShaderConfiguration::DEFAULT,"depth");
//      ShaderProgram *shader = mSceneManager->getAsset()->getShaderProgram(depthConfig);
//      glAssert( glUseProgram(shader->id()) );
////        glAssert ( glUniform1i ( glGetUniformLocation ( shader->id(), "width" ), mWidth) );
////        glAssert ( glUniform1i ( glGetUniformLocation ( shader->id(), "height" ), mHeight) );
//      // bind the texture
//      mDepthMap->bind(GL_TEXTURE0);
//      glAssert ( glUniform1i ( glGetUniformLocation ( shader->id(), "depth" ), 0) );
//      // draw the quad
//      mScreenQuad->draw();
void ShaderObject::del()
{
    glAssert(glDeleteShader(mId));
    mId = -1;
}
GLenum ShaderObject::getType() const
{
    return mType;
}

void ShaderObject::setType(const GLenum &value)
{
    mType = value;
}



/******************/
/* shader program */
/******************/

void ShaderProgram::create()
{
    glAssert(mId = glCreateProgram());
}

void ShaderProgram::del()
{
    int status;
    glAssert(glGetProgramiv(mId, GL_DELETE_STATUS, &status));
    std::cerr << "delete status " << status << std::endl;
    glAssert(glDeleteProgram(mId));
    std::cerr <<" delete shader " << mId << "  " << (GL_TRUE == glIsProgram(mId)) << std::endl;
    if(glIsProgram(mId)){
        glAssert(glGetProgramiv(mId, GL_DELETE_STATUS, &status));
        std::cerr << "delete status " << status << std::endl;
    }
    else{
        std::cerr << mId << " is not a program " << std::endl;
    }
    glCheckError();
    mId = -1;
}


void ShaderProgram::add(ShaderObject *s)
{
    mShaderObjects.push_back(s);
}

void ShaderProgram::link()
{

    //      attach all shaderObjects
    for (std::vector<ShaderObject*>::iterator itr = mShaderObjects.begin(), stop = mShaderObjects.end();
            itr != stop;
            ++itr) {
        glAssert(glAttachShader(mId, (*itr)->id()));
    }

    // always bind this attribts
    glAssert(glBindAttribLocation(mId, 0, "inPosition"));
    glAssert(glBindAttribLocation(mId, 1, "inNormal"));
    glAssert(glBindAttribLocation(mId, 2, "inTangent"));
    glAssert(glBindAttribLocation(mId, 3, "inTexCoord"));


    glAssert(glLinkProgram(mId));
    // check link
    check();

    //automatic texture unit and location management
    int texUnit = 0;
    int total = -1;
    glGetProgramiv( mId, GL_ACTIVE_UNIFORMS, &total );

    modelViewMatrixLocation = glGetUniformLocation( mId, "modelViewMatrix");
    projectionMatrixLocation = glGetUniformLocation( mId, "projectionMatrix");
    MVPLocation = glGetUniformLocation( mId, "MVP");
    normalMatrixLocation = glGetUniformLocation( mId, "normalMatrix");

    textureUnits.clear();

    for(int i=0; i<total; ++i)  {
        int name_len=-1, num=-1;
        GLenum type = GL_ZERO;
        char name[100];
        glGetActiveUniform( mId, GLuint(i), sizeof(name)-1,
            &name_len, &num, &type, name );
        name[name_len] = 0;
/// TODO : add other sampler type
        if(type == GL_SAMPLER_2D || type ==GL_SAMPLER_CUBE|| type == GL_SAMPLER_2D_RECT){
            GLuint location = glGetUniformLocation( mId, name );
            textureUnits[std::string(name)] = TextureBinding(texUnit++, location);
        }
    }
}

int ShaderProgram::check()
{

    GLint linked;
    glAssert(glGetProgramiv(mId, GL_LINK_STATUS, &linked));
    if (!linked) {
        std::cerr << "Program not linked" << std::endl;
        for (unsigned int i = 0; i < mShaderObjects.size(); ++i) {
            std::cerr << mShaderObjects[i]->getFileName() << std::endl;
        }
        printProgramInfoLog(mId);
        return 0;
    }
    return 1;
}

bool ShaderProgram::reload()
{
    bool ok = true;

    for (unsigned int i = 0; ok && i < mShaderObjects.size(); ++i) {
        glAssert(glDetachShader(mId, mShaderObjects[i]->id()));
        mShaderObjects[i]->del();
        mShaderObjects[i]->create(mShaderObjects[i]->getType());
        mShaderObjects[i]->reload();
        mShaderObjects[i]->compile();
        ok |= mShaderObjects[i]->check();
    }
    if (ok) {
        del();
        create();
        link();
        check();
    }
    return ok;
}


/*
 * Shader configuration
 */

std::set<std::string> ShaderConfiguration::mDefaultProperties;

ShaderConfiguration::ShaderConfiguration(ShaderType type, std::string name) : mNumProperties(0), mProgramName(name), mType(type)
{
}

ShaderConfiguration::~ShaderConfiguration()
{
}

void ShaderConfiguration::addProperty(std::string name)
{
    mProperties.insert(name);
    mNumProperties = mProperties.size();
}

void ShaderConfiguration::removeProperty(std::string name)
{
    mProperties.erase(name);
    mNumProperties = mProperties.size();
}

int ShaderConfiguration::numProperties() const
{
    return mProperties.size();
}

std::string ShaderConfiguration::getPropertyName(unsigned int i) const
{
    std::set<std::string>::iterator it = mProperties.begin();
    while (i--) ++it;
    return *it;
}

bool ShaderConfiguration::operator<(const ShaderConfiguration& b) const
{
    if (mProgramName == b.mProgramName) {
        if (mNumProperties == b.mNumProperties) {
            if (mNumProperties == 0)
                return false;
            std::set<std::string>::iterator lhs = mProperties.begin();
            std::set<std::string>::iterator rhs = b.mProperties.begin();
            while ((lhs != mProperties.end()) && (*lhs == *rhs)) {
                ++lhs;
                ++rhs;
            }
            if (lhs == mProperties.end())
                return false;
            else
                return (*lhs < *rhs);
        } else
            return (mNumProperties < b.mNumProperties);
    } else
        return (mProgramName < b.mProgramName);
}

std::ostream & operator << (std::ostream & out, const ShaderConfiguration configuration)
{
    out << "Program Name : " << configuration.mProgramName << std::endl;
    out << "Configuration string (" << configuration.mNumProperties << ") : " ;
    for (std::set<std::string>::const_iterator it = configuration.mProperties.begin(); it != configuration.mProperties.end(); ++it) {
        out << *it << " ";
    }
    return out;
}


}
