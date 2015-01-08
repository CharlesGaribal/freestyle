/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef SHADEROBJECT_H
#define SHADEROBJECT_H
#include <fstream>
#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <string>

#include "opengl.h"
#include "texture.h"
#include "light.h"
#include "ubo.h"
#include "bindable.h"


namespace vortex {

class Uniform {

};

/**
 * Shader configuration : control the compilation mechanism of shaders.
 * A shader configuration is composed of a set of required uniform variables from shader
 * The compile process of a shader takes a ShaderConfiguration object and a vector of pair
 * <shaderType, filename> and verify that the compilation could be done with this configuration.
 * If no pair are given shaders are loaded form default.vert and default.frag
 * Each shader configuration is identified by its properties and assets manager ensure that only one shaderProgram per configuration exists
 */
class ShaderConfiguration {
public:
    /**
     * Specify the kind of shader that will be used through the configuration
     * TODO Mathias --> add tesseletion shaders ?
     */
    enum ShaderType { VERTEX_SHADER = 1 << 0, GEOMETRY_SHADER = 1 << 1, FRAGMENT_SHADER = 1 << 2, DEFAULT = VERTEX_SHADER | FRAGMENT_SHADER, ALL = DEFAULT | GEOMETRY_SHADER, INVALID = 0 };

//    ShaderConfiguration(ShaderType type = DEFAULT, std::string name = "../shaders/default");
    ShaderConfiguration(ShaderType type = INVALID, std::string name = "null");
    ShaderConfiguration(const ShaderConfiguration &conf){
        mType = conf.mType;
        mProgramName = conf.mProgramName;
        mNumProperties = conf.mNumProperties;
        mProperties = conf.mProperties;
    }

    ~ShaderConfiguration();

    void addProperty(std::string name);
    void removeProperty(std::string name);
    int numProperties() const;
    std::string getPropertyName(unsigned int i) const;

    std::string getProgramName() const {
        return mProgramName;
    }

    ShaderType getType() const {
        return mType;
    }

    /**
     * Comparison operator for map insertion
     * @todo, define a total order on ShaderConfiguration
     */
    bool operator<(const ShaderConfiguration& b) const;

    friend std::ostream & operator << (std::ostream & out, const ShaderConfiguration configuration);

    static void addDefaultProperty(std::string name)
    {
        mDefaultProperties.insert(name);
    }

    static int numDefaultProperties()
    {
        return mDefaultProperties.size();
    }
    static std::string getDefaultPropertyName(unsigned int i){
        std::set<std::string>::iterator it = mDefaultProperties.begin();
        while (i--) ++it;
        return *it;
    }


private:
    /**
     * Vector of properties.
     * This vector is kept ordered and the ShaderConfiguration order is built on this vector
     */
    std::set<std::string> mProperties;
    int mNumProperties;
    std::string mProgramName;
    ShaderType mType;

    static std::set<std::string> mDefaultProperties;
};


class ShaderObject {
public:
    ShaderObject();

    ~ShaderObject();

    /**
     * Create an empty shader of any type
     */
    void create(GLenum type);
    /**
     * Set the configuration string for the shader.
     * A configuration string may be any GLSL compliant set of #define, const declaration, pragma settings, ...
     * This string will then prefix shader source when compiling.
     */
    void configure(std::string configurationString) {
        mConfigurationString = configurationString;
    };
    /**
     * Load the shader from fileName.
     * Note that the source string isn'nt kept in client space.
     * This function call source with the loaded code after prefixing it with the configurationString
     */
    void load(std::string fileName);

    /**
     * Load the shader from fileName.
     * Note that the source string isn'nt kept in client space.
     * This function call source with the loaded code after prefixing it with the configurationString
     */
    void source(const char** codeStrings, int nbStrings);

    /**
     * Reload the shader file.
     */
    void reload() {
        std::cerr << "reload " << mFileName << std::endl;
        load(mFileName);
    }

    /**
     * Compile the shader
     */
    void compile();

    /**
     * check the compilation status
     */
    int check();

    /**
     * ShaderObject OpenGL deletion.
     */
    void del();

    // define ?
    inline GLuint id() {
        return mId;
    }

    std::string &getFileName(){return mFileName;}

    GLenum getType() const;
    void setType(const GLenum &value);

private:
    GLuint mId;
    GLenum mType;
    std::string mFileName;
    std::string mTypeString;
    std::string mConfigurationString;
};

/**
 * Programme GLSL.
 * @todo : regarder  glValidateProgram ...
 */
class ShaderProgram : public Bindable {
public:

    ShaderProgram() : Bindable() {
        //mUBO = new UBO();
    }

    virtual ~ShaderProgram() {
        //deleteProgram();
    }

    void deleteProgram() {
        for (unsigned int i = 0; i < mShaderObjects.size(); ++i) {
            delete mShaderObjects[i];
        }
        //delete mUBO;
    }

    void setTransform(glm::mat4x4 modelView, glm::mat4x4 projection, glm::mat4x4 mvp, glm::mat4x4 normal) {
        /**********     METHOD WITHOUT UBO    *******/
              glAssert ( glUniformMatrix4fv ( modelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr ( modelView ) ) );
              glAssert ( glUniformMatrix4fv ( projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr ( projection ) ) );
              glAssert ( glUniformMatrix4fv ( MVPLocation, 1, GL_FALSE, glm::value_ptr ( mvp ) ) );
              glAssert ( glUniformMatrix4fv ( normalMatrixLocation, 1, GL_FALSE, glm::value_ptr ( normal ) ) );
        /*********************************************/

              /*glm::mat4x4 matriceBlock[] = {
            modelView,
            projection,
            mvp,
            normal
        };

        mUBO->bind(&matriceBlock, sizeof(matriceBlock));*/
    }

    void setConfiguration (const ShaderConfiguration &conf){
        mProgramConfiguration = conf;
    }

    const ShaderConfiguration &getConfiguration () const{
        return mProgramConfiguration;
    }

    /**
     * Specify the OpenGL API to use the ShaderProgram for rendering pass.
     */
    void bind() const {
       glAssert(glUseProgram(mId));
       // mUBO->init(mId, "MatriceBlock");
//#define DEBUG_SHADERS
#ifdef DEBUG_SHADERS
       std::cerr << mProgramConfiguration << std::endl;
       glValidateProgram(mId);

       {
           int infoLogLen = 0;
           int charsWritten = 0;
           GLchar *infoLog;
           glAssert(glGetProgramiv(mId, GL_INFO_LOG_LENGTH, &infoLogLen));
           if (infoLogLen > 0) {
               infoLog = new GLchar[infoLogLen];
               // error check for fail to allocate memory omitted
               glAssert(glGetProgramInfoLog(mId, infoLogLen, &charsWritten, infoLog));
               std::cerr << "InfoLog:" << std::endl << infoLog << std::endl;
               delete [] infoLog;

           }
       }
#endif
    }

    /**
      * useless but polymorphism problem using std::map
      */
    void bind(const GlobalParameter& parameters) const {
        bind();
        parameters.set(this);
    }

    bool operator< (const ShaderProgram &other) const {
        return (mId < other.mId);
    }

    /**
     * ShaderProgram OpenGL creation
     */
    void create();

    /**
     * Add a ShaderObject to the ShaderProgram
     *
     * @param s The ShaderProgram to be added. Must have been checked before calling this method.
     */
    void add(ShaderObject *s);
    /*
    glBindAttribLocation(p, 0, "inPosition");
    glBindAttribLocation(p, 1, "inNormal");
    glBindAttribLocation(p, 2, "inTangent");
    glBindAttribLocation(p, 3, "inTexCoord");
    */

    /**
     * Attach all ShaderObjects added to the ShaderProgram and link it.
     */
    void link();

    /**
     * Check the link status of the ShaderProgram.
     *
     * @return True if the ShaderProgram is correctly linked, false otherwise.
     */
    int check();

    /**
     * ShaderProgram OpenGL deletion.
     */
    void del();

    /**
     * @return The OpenGL id of the ShaderProgram.
     */
    GLuint id() const {
        return mId;
    }

    /**
     * Reload the ShaderProgram.
     *
     * @return True if the ShaderProgram has been correctly reloaded.
     */
    bool reload();

    /*
     uint CreateProgram( void );
     void AttachShader( uint program, uint shader );

     void DetachShader( uint program, uint shader );
     void LinkProgram( uint program );

     void UseProgram( uint program );
     void DeleteProgram( uint program );

    void GetActiveAttrib( uint program, uint index,
    sizei bufSize, sizei *length, int *size, enum *type,
    char *name );

    int GetAttribLocation( uint program, const char *name );

    void BindAttribLocation( uint program, uint index, const
    char *name );


    int GetUniformLocation( uint program, const
    char *name );
    uint GetUniformBlockIndex( uint program, const
    char *uniformBlockName );

    void GetActiveUniformBlockName( uint program,
    uint uniformBlockIndex, sizei bufSize, sizei *length,
    char *uniformBlockName )

    void GetActiveUniformBlockiv( uint program,
    uint uniformBlockIndex, enum pname, int *params );

    void GetUniformIndices( uint program,
    sizei uniformCount, const char **uniformNames,
    uint *uniformIndices );

    void GetActiveUniformName( uint program,
    uint uniformIndex, sizei bufSize, sizei *length,
    char *uniformName );

    void GetActiveUniform( uint program, uint index,
    sizei bufSize, sizei *length, int *size, enum *type,
    char *name );

    void GetActiveUniformsiv( uint program,
    sizei uniformCount, const uint *uniformIndices,
    enum pname, int *params );

    void Uniformf1234gfifg( int location, T value );
    void Uniformf1234gfifgv( int location, sizei count,
    T value );
    void Uniformf1,2,3,4gui( int location, T value );
    void Uniformf1,2,3,4guiv( int location, sizei count,
    T value );
    void UniformMatrixf234gfv( int location, sizei count,
    boolean transpose, const float *value );
    void UniformMatrixf2x3,3x2,2x4,4x2,3x4,4x3gfv(
    int location, sizei count, boolean transpose, const
    float *value );

    */
    void setUniform(const char *name, const glm::vec3 &value) const {
        glAssert( glUniform3fv(glGetUniformLocation(mId, name), 1, glm::value_ptr(value)));
    }

    void setUniform(const char *name, int size, const glm::vec3 *value) const {
        glAssert( glUniform3fv(glGetUniformLocation(mId, name), size, glm::value_ptr(value[0])));
    }

    void setUniform(const char *name, const glm::vec4 &value) const {
        glAssert(glUniform4fv(glGetUniformLocation(mId, name), 1, glm::value_ptr(value)));
    }

    void setUniform(const char *name, const glm::mat4x4 &value) const {
        glAssert ( glUniformMatrix4fv ( glGetUniformLocation ( mId, name ), 1, GL_FALSE, glm::value_ptr ( value ) ) );
    }

    void setUniform(const char *name, const glm::vec2 &value) const {
        glAssert(glUniform2fv(glGetUniformLocation(mId, name), 1, glm::value_ptr(value)));
    }

    void setUniform(const char *name, int value) const{
        glAssert(glUniform1i(glGetUniformLocation(mId, name), value));
    }

    void setUniform(const char *name, unsigned int value) const{
        glAssert(glUniform1i(glGetUniformLocation(mId, name), value));
    }

    void setUniform(const char *name, float value) const{
        glAssert(glUniform1f(glGetUniformLocation(mId, name), value));
    }

    /*void setUniformTexture(const char *texName, Texture *tex, int texUnit) const {
        tex->bind(texUnit);
        glAssert(glUniform1i(glGetUniformLocation(mId, texName), texUnit));
    }
*/

    //! use automatic texture unit computation
    //! if you really want to send a particular texture unit, use setUniform and bind the texture by hand
    //! warning, it binds tex on an "arbitrary" tex unit
    //! warning, perform a find (log active texture in the shader)
    void setUniformTexture(const char *texName, Texture *tex) const {
#ifdef DEBUG_SHADERS
        std::cerr << "Active textures :" << std::endl;
        for (TextureUnitsT::const_iterator itr = textureUnits.begin(); itr != textureUnits.end(); ++itr)
            std::cerr << "Texture : " << itr->first << " --> " << itr->second.texUnit_ << std::endl;
#endif
        TextureUnitsT::const_iterator itr = textureUnits.find(std::string(texName));
        if(itr != textureUnits.end()){
            tex->bind(itr->second.texUnit_);
            glAssert(glUniform1i(itr->second.location_, itr->second.texUnit_));
        }
        #ifdef DEBUG_SHADERS
        else {
            // if not found in texutreBindings, this texture is not active in the shader
            std::cerr << "ShaderProgram::setUniformTexture Texture " << texName << " not active" << std::endl;
        }
        #endif
    }


private:

    struct TextureBinding{
        int texUnit_;
        int location_;
        TextureBinding(int unit, int location):texUnit_(unit), location_(location){}
        TextureBinding():texUnit_(-1), location_(-1){}
    };
    typedef std::map<std::string, TextureBinding> TextureUnitsT;
    TextureUnitsT textureUnits;

    std::vector<ShaderObject*> mShaderObjects;
    GLuint mId;
    ShaderConfiguration mProgramConfiguration;
    UBO *mUBO;

    // Location for default parameters ...
    // queried only once when program is linked
    GLuint modelViewMatrixLocation;
    GLuint projectionMatrixLocation;
    GLuint MVPLocation;
    GLuint normalMatrixLocation;

};

//-----------------------------
// Shader parameter management
//-----------------------------


// Global parameters for general shaders
class ShadersGlobalParameters : public GlobalParameter {
public :
    ShadersGlobalParameters(){
    }

    void set(const Bindable *object) const {
        const ShaderProgram* theProgram = static_cast<const ShaderProgram*>(object);
        for (TextureParameterVector::const_iterator i = mTextureParameters.begin(); i != mTextureParameters.end();  ++i ){
            theProgram->setUniformTexture( (*i).texName, (*i).tex);
        }
        for (MatrixParameterVector::const_iterator i = mMatrixParameters.begin(); i != mMatrixParameters.end();  ++i ){
            theProgram->setUniform( (*i).matrixName, (*i).matrix);
        }
        for (IntegerParameterVector::const_iterator i = mIntegerParameters.begin(); i != mIntegerParameters.end();  ++i ){
            theProgram->setUniform( (*i).valueName, (*i).value);
        }
        for (FloatParameterVector::const_iterator i = mFloatParameters.begin(); i != mFloatParameters.end();  ++i ){
            theProgram->setUniform( (*i).valueName, (*i).value);
        }
        for (Vec4ParameterVector::const_iterator i = mVec4Parameters.begin(); i != mVec4Parameters.end();  ++i ){
            theProgram->setUniform( (*i).valueName, (*i).value);
        }
    }


    void addParameter(const char *texName, Texture *tex){
        mTextureParameters.push_back( TextureParameter(texName, tex) );
    }

    void addParameter(const char *matrixName, const glm::mat4 &matrix){
        mMatrixParameters.push_back( MatrixParameter(matrixName, matrix) );
    }

    void addParameter(const char *valueName,  int value){
        mIntegerParameters.push_back( IntegerParameter(valueName, value) );
    }

    void addParameter(const char *valueName,  float value){
        mFloatParameters.push_back( FloatParameter(valueName, value) );
    }

    void addParameter(const char *valueName,  glm::vec4 value){
        mVec4Parameters.push_back( Vec4Parameter(valueName, value) );
    }

private:
    /* global textures for lighting */
    struct TextureParameter{
        const char* texName;
        Texture *tex;
        TextureParameter( const char*tn, Texture *t) : texName(tn), tex(t){}
    };
    typedef std::vector< TextureParameter > TextureParameterVector;
    TextureParameterVector mTextureParameters;

    /* global matrices for lighting */
    struct MatrixParameter{
        const char* matrixName;
        glm::mat4 matrix;
        MatrixParameter( const char*mn, const glm::mat4 &m) : matrixName(mn), matrix(m) {}
    };
    typedef std::vector< MatrixParameter > MatrixParameterVector;
    MatrixParameterVector mMatrixParameters;

    /* global int for lighting */
    struct IntegerParameter{
        const char* valueName;
        int value;
        IntegerParameter( const char*vn, int v) : valueName(vn), value(v) {}
    };
    typedef std::vector< IntegerParameter > IntegerParameterVector;
    IntegerParameterVector mIntegerParameters;

    /* global float for lighting */
    struct FloatParameter{
        const char* valueName;
        float value;
        FloatParameter( const char*vn, float v) : valueName(vn), value(v) {}
    };    
    typedef std::vector< FloatParameter > FloatParameterVector;
    FloatParameterVector mFloatParameters;

    struct Vec4Parameter{
        const char* valueName;
        glm::vec4 value;
        Vec4Parameter( const char*vn, glm::vec4 v) : valueName(vn), value(v) {}
    };

    typedef std::vector< Vec4Parameter > Vec4ParameterVector;
    Vec4ParameterVector mVec4Parameters;

};


// Global parameters for shaders with one light source
class LightGlobalParameters : public ShadersGlobalParameters {
public :
    LightGlobalParameters( const Light &l) : ShadersGlobalParameters(), mLight(l){
    }
    void set(const Bindable *object) const {
        ShadersGlobalParameters::set(object);
        const ShaderProgram* theProgram = static_cast<const ShaderProgram*>(object);
        mLight.bind(theProgram->id());
    }
private:
    const Light &mLight;
};


}

#endif // SHADEROBJECT_H
