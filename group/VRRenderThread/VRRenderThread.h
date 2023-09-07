/**		@file VRRenderThread.h
  *
  *		EEEE2046 - Software Engineering & VR Project
  *
  *		Template to add VR rendering to your application.
  *
  *		P Evans 2022
  */
#ifndef VR_RENDER_THREAD_H
#define VR_RENDER_THREAD_H

/* Project headers */

/* Qt headers */
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

/* Vtk headers */
#include <vtkActor.h>
#include <vtkOpenVRRenderWindow.h>				
#include <vtkOpenVRRenderWindowInteractor.h>	
#include <vtkOpenVRRenderer.h>					
#include <vtkOpenVRCamera.h>	
#include <vtkActorCollection.h>
#include <vtkCommand.h>



/* Note that this class inherits from the Qt class QThread which allows it to be a parallel thread
 * to the main() thread, and also from vtkCommand which allows it to act as a "callback" for the 
 * vtkRenderWindowInteractor. This callback functionallity means that once the renderWindowInteractor
 * takes control of this thread to enable VR, it can callback to a function in the class to check to see
 * if the user has requested any changes
 */
class VRRenderThread : public QThread {
    Q_OBJECT

public:
    /** List of command names */
    enum {
        END_RENDER,
        ROTATE_X,
        ROTATE_Y,
        ROTATE_Z
    } Command;


    /**  Constructor
      */
    VRRenderThread(QObject* parent = nullptr);

    /**  Denstructor
      */
    ~VRRenderThread();

    /** This allows actors to be added to the VR renderer BEFORE the VR
      * interactor has been started 
     */
    void addActorOffline(vtkActor* actor);


    /** This allows commands to be issued to the VR thread in a thread safe way. 
      * Function will set variables within the class to indicate the type of
      * action / animation / etc to perform. The rendering thread will then impelement this.
      */
    void issueCommand( int cmd, double value );


protected:
    /** This is a re-implementation of a QThread function 
      */
    void run() override;

private:
    /* Standard VTK VR Classes */
    vtkSmartPointer<vtkOpenVRRenderWindow>              window;
    vtkSmartPointer<vtkOpenVRRenderWindowInteractor>    interactor;
    vtkSmartPointer<vtkOpenVRRenderer>                  renderer;
    vtkSmartPointer<vtkOpenVRCamera>                    camera;

    /* Use to synchronise passing of data to VR thread */
    QMutex                                              mutex;      
    QWaitCondition                                      condition;

    /** List of actors that will need to be added to the VR scene */
    vtkSmartPointer<vtkActorCollection>                 actors;

    /** A timer to help implement animations and visual effects */
    std::chrono::time_point<std::chrono::steady_clock>  t_last;

    /** This will be set to false by the constructor, if it is set to true
      * by the GUI then the rendering will end 
      */
    bool                                                endRender;

    /* Some variables to indicate animation actions to apply.
     *
     */
    double rotateX;         /*< Degrees to rotate around X axis (per time-step) */
    double rotateY;         /*< Degrees to rotate around Y axis (per time-step) */
    double rotateZ;         /*< Degrees to rotate around Z axis (per time-step) */
};



#endif
