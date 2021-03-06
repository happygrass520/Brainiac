// Brainiac is a free and open source tool for the creation of crowd simulation

// Copyright (C) 2012  Dirk Fuchs dirkfux@googlemail.com

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "gui/osgmultithreadedqtwidget.h"

#ifdef BRAINIAC_SUPPRESS_THIRD_PARTY_WARNINGS
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Woverloaded-virtual"
#endif
#include "osgDB/WriteFile"
#ifdef BRAINIAC_SUPPRESS_THIRD_PARTY_WARNINGS
    #pragma clang diagnostic pop
#endif
#include "core/brainiaclogger.h"
#include "core/brainiacglobals.h"

OsgMultithreadedViewerWidget::OsgMultithreadedViewerWidget( osg::Camera* camera, osg::Node* scene, bool fixVerticalAxis ):
    m_rootNode(new osg::Group),
    m_showOriginCoordCross(false),
    m_camera(camera),
    m_cameraManipulator(new BrainiacCameraManipulator),
    m_sceneNode(new osg::Group),
    m_needGlUpdate(false)
{
    m_osgFileName=QDir::tempPath()%"/osgOut.osg";

    m_rootNode->setName("Viewer rootGroupNode");
    if(!m_camera)
        m_camera=createCamera( 50, 50, 640, 480 );
    m_viewer.setCamera( m_camera );
    if(scene) {
        m_sceneNode->addChild(scene);
    } else {

    }

    if(fixVerticalAxis) {
        osg::MatrixTransform *trans=new osg::MatrixTransform();
        trans->setName("Fix vertical axis node");
        osg::Matrix m;
        m.makeRotate(M_PI_2,1.,0.,0.);
        trans->setMatrix(m);
        m_rootNode->addChild(trans);
        addOriginCoordCross(trans);
        trans->addChild(m_sceneNode);
    } else {
        addOriginCoordCross(m_rootNode);
        m_rootNode->addChild(m_sceneNode);
    }

    m_viewer.setSceneData(m_rootNode);
    m_viewer.addEventHandler( new osgViewer::StatsHandler );
    if(fixVerticalAxis) {
        m_cameraManipulator->setVerticalAxisFixed(true);
        m_cameraManipulator->setHomePosition(osg::Vec3d(0.,-2000.,0.),osg::Vec3d(0.,0.,0.),osg::Vec3d(0.,0.,1.),false);
    } else {
        m_cameraManipulator->setHomePosition(osg::Vec3d(-2000.,0.,0.),osg::Vec3d(0.,0.,0.),osg::Vec3d(0.,1.,0.),false);
    }
    m_viewer.setCameraManipulator(m_cameraManipulator);

    m_viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);

    m_gw = dynamic_cast<osgQt::GraphicsWindowQt*>( m_camera->getGraphicsContext() );
    if ( m_gw )
    {
        QVBoxLayout* layout = new QVBoxLayout;
        layout->addWidget( m_gw->getGLWidget() );
        setLayout( layout );
    }
    if(false) {
        m_thread.viewerPtr = &m_viewer;
        m_thread.start();
    } else {
        connect( &m_timer, SIGNAL(timeout()), this, SLOT(update()) );
        m_timer.start( 40 );
    }
    setGeometry( 100, 100, 800, 600 );
    setWindowFlags(Qt::Tool);
    show();
    m_viewer.getCameraManipulator()->home(0);

}


void OsgMultithreadedViewerWidget::addOriginCoordCross(osg::Group *node)
{
    osg::MatrixTransform *trans=new osg::MatrixTransform();
    m_originCoordCrossWitch=new osg::Switch();
    m_scaleOriginCoordCross.identity();
    m_scaleOriginCoordCross.makeScale(100,100,100);
    trans->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL,osg::StateAttribute::ON);
    trans->setMatrix(m_scaleOriginCoordCross);
    QFile coordCross("://gui/geo/axes.osgt");
    m_originCoordCrossWitch->addChild(trans,m_showOriginCoordCross);
    trans->addChild(BrainiacGlobals::loadOsgNodeFromQFile(coordCross));
    node->addChild(m_originCoordCrossWitch);
}


bool OsgMultithreadedViewerWidget::saveSceneGraph()
{
    bool success=osgDB::writeNodeFile(*m_rootNode,m_osgFileName.toStdString());
    if(!success) {
        qWarning() << __PRETTY_FUNCTION__ << "Error writing scene graph to " << m_osgFileName;
    }
    return success;
}

osg::Camera *OsgMultithreadedViewerWidget::createCamera(int x, int y, int w, int h)
{
    osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
    Q_UNUSED(ds);
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->windowDecoration = false;
    traits->x = x;
    traits->y = y;
    traits->width = w;
    traits->height = h;
    traits->doubleBuffer = true;

    osg::ref_ptr<osg::Camera> camera = new osg::Camera;
    m_glWindow=new BrainiacGlWindow(traits.get());
    camera->setGraphicsContext( m_glWindow );
    camera->setClearColor( osg::Vec4(0.2, 0.2, 0.6, 1.0) );
    camera->setViewport( new osg::Viewport(0, 0, traits->width, traits->height) );
    camera->setProjectionMatrixAsPerspective(
                30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0f, 10000.0f );

    camera->getOrCreateStateSet()->setGlobalDefaults(); //! \bug "wrong gfx" http://forum.openscenegraph.org/viewtopic.php?t=13111
    return camera.release();
}

BrainiacGlWindow *OsgMultithreadedViewerWidget::getGlWindow()
{
    return m_glWindow;
}

osgGA::CameraManipulator *OsgMultithreadedViewerWidget::getCameraManipulator() {
    return m_viewer.getCameraManipulator();
}

void OsgMultithreadedViewerWidget::needGlUpdate()
{
    m_needGlUpdate=true;
}

void OsgMultithreadedViewerWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if(m_viewer.checkNeedToDoFrame() || m_needGlUpdate) {
        m_viewer.frame();
        m_needGlUpdate=false;
    }
}

void OsgMultithreadedViewerWidget::showOriginCoordCross(bool show)
{
    m_showOriginCoordCross=show;
    m_originCoordCrossWitch->setValue(0,m_showOriginCoordCross);
}

void OsgMultithreadedViewerWidget::toggleOriginCoordCross()
{
    showOriginCoordCross(!m_showOriginCoordCross);
}
