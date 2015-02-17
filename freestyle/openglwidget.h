/***************************************************************************
 *   Copyright (C) 2008 by Mathias Paulin   *
 *   Mathias.Paulin@irit.fr   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H


#include "ftylrenderer.h"

#include "../engine/scenemanager.h"
#include "../engine/camera.h"
#include "../engine/assetmanager.h"
#include "../engine/scenemanager.h"

#include <../engine/picker.h>

#include <QGLWidget>
#include <QImage>

#include "../sculptor/sculptor.h"

/**
   @author Mathias Paulin <Mathias.Paulin@irit.fr>
   @author David Vanderhaeghe <vdh@irit.fr>
*/

namespace vortex{
namespace ui{
class CameraController;
}
}

class OpenGLWidget : public QGLWidget {
    Q_OBJECT
public:
    OpenGLWidget ( QWidget *parent );
    virtual ~OpenGLWidget();

    void clear();
    bool loadScene(std::string fileName);

    void resetCamera();
    vortex::SceneManager *sceneManager(){ return sceneManager_; }
    void switchRenderingMode(bool on);

    inline void setSculptor(Sculptor &s){ sculptor = &s; }

protected:
    void initializeGL();
    void resizeGL ( int w, int h );
    void paintGL();
    virtual void mousePressEvent ( QMouseEvent * e );
    virtual void mouseReleaseEvent ( QMouseEvent * e );
    virtual void mouseMoveEvent ( QMouseEvent * e );
    virtual void wheelEvent ( QWheelEvent * e );
    virtual void keyPressEvent ( QKeyEvent * e );

private slots:


private:
    FtylRenderer *renderer_;
    vortex::Camera *camera_;
    vortex::SceneManager* sceneManager_;
    vortex::AssetManager* assetManager_;

    int width_;
    int height_;

    vortex::ui::CameraController *cameraController_;

    Sculptor *sculptor;

    /* For picking */
    vortex::Picker *objectpicker_;
    int select(const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix, int i, int j, int *selectionBuffer);

};

#endif
