/**		@file VRRenderThread.cpp
  *
  *		EEEE2046 - Software Engineering & VR Project
  *
  *		Template to add VR rendering to your application.
  *
  *		P Evans 2022
  */

#include "VRRenderThread.h"


/* Vtk headers */
#include <vtkActor.h>
#include <vtkOpenVRRenderWindow.h>				
#include <vtkOpenVRRenderWindowInteractor.h>	
#include <vtkOpenVRRenderer.h>					
#include <vtkOpenVRCamera.h>	

#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkNamedColors.h>
#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSTLReader.h>
#include <vtkDataSetmapper.h>
#include <vtkCallbackCommand.h>


/* The class constructor is called by MainWindow and runs in the primary program thread, this thread
 * will go on to handle the GUI (mouse clicks, etc). The OpenVRRenderWindowInteractor cannot be start()ed
 * in the constructor, as it will take control of the main thread to handle the VR interaction (headset 
 * rotation etc. This means that a second thread is needed to handle the VR.
 */
VRRenderThread::VRRenderThread( QObject* parent ) {
	/* Initialise actor list */
	actors = vtkActorCollection::New();

	/* Initialise command variables */
	rotateX = 0.;
	rotateY = 0.;
	rotateZ = 0.;
}


/* Standard destructor - this is important here as the class will be destroyed when the user
 * stops the VR thread, and recreated when the user starts it again. If class variables are 
 * not deallocated properly then there will be a memory leak, where the program's total memory
 * usage will increase for each start/stop thread cycle.
 */
VRRenderThread::~VRRenderThread() {

}


void VRRenderThread::addActorOffline( vtkActor* actor ) {

	/* Check to see if render thread is running */
	if (!this->isRunning()) {
		double* ac = actor->GetOrigin();
	
		/* I have found that these initial transforms will position the FS
		 * car model in a sensible position but you can experiment
		 */
		actor->RotateX(-90);
		actor->AddPosition(-ac[0]+0, -ac[1]-100, -ac[2]-200);

		actors->AddItem(actor);
	}
}



void VRRenderThread::issueCommand( int cmd, double value ) {

	/* Update class variables according to command */
	switch (cmd) {
		/* These are just a few basic examples */
		case END_RENDER:
			this->endRender = true;
			break;

		case ROTATE_X:
			this->rotateX = value;
			break;

		case ROTATE_Y:
			this->rotateY = value;
			break;

		case ROTATE_Z:
			this->rotateZ = value;
			break;
	}
}

/* This function runs in a separate thread. This means that the program 
 * can fork into two separate execution paths. This thread is triggered by
 * calling VRRenderThread::start()
 */
void VRRenderThread::run() {
	/* You might want to edit the 3D model once VR has started, however VTK is not "thread safe". 
	 * This means if you try to edit the VR model from the GUI thread while the VR thread is
	 * running, the program could become corrupted and crash. The solution is to get the VR thread
	 * to edit the model. Any decision to change the VR model will come fromthe user via the GUI thread, 
	 * so there needs to be a mechanism to pass data from the GUi thread to the VR thread.
	 */

	vtkNew<vtkNamedColors> colors;

	// Set the background color.
	std::array<unsigned char, 4> bkg{ {26, 51, 102, 255} };
	colors->SetColor("BkgColor", bkg.data());
	
	// The renderer generates the image
	// which is then displayed on the render window.
	// It can be thought of as a scene to which the actor is added
	renderer = vtkOpenVRRenderer::New();	
	
	renderer->SetBackground(colors->GetColor3d("BkgColor").GetData());
	
	/* Loop through list of actors provided and add to scene */
	vtkActor* a;
	actors->InitTraversal();
	while( (a = (vtkActor*)actors->GetNextActor() ) ) {
		renderer->AddActor(a);
	}

	/* The render window is the actual GUI window
	 * that appears on the computer screen
	 */
	window = vtkOpenVRRenderWindow::New();

	window->Initialize();
	window->AddRenderer(renderer);
	
	/* Create Open VR Camera */
	camera = vtkOpenVRCamera::New();				
	renderer->SetActiveCamera(camera);			

	/* The render window interactor captures mouse events
	 * and will perform appropriate camera or actor manipulation
	 * depending on the nature of the events.
	 */
	interactor = vtkOpenVRRenderWindowInteractor::New();									
	interactor->SetRenderWindow(window);													
	interactor->Initialize();
	window->Render();
	

	/* Now start the VR - we will implement the command loop manually
	 * so it can be interrupted to make modifications to the actors
	 * (i.e. to implement animation)
	 */
	endRender = false;
	t_last = std::chrono::steady_clock::now();

	while( !interactor->GetDone() && !this->endRender ) {
		interactor->DoOneEvent( window, renderer );

		/* Check to see if enough time has elapsed since last update 
		 * This looks overcomplicated (and it is, C++ loves to make things unecessarily complicated!) but
		 * is really just checking if more than 20ms have elaspsed since the last animation step. The 
		 * complications comes from the fact that numbers representing time on computers don't usually have
		 * standard second/millisecond units. Because everything is a class in C++, the converion from
		 * computer units to seconds/milliseconds ends up looking like what you see below.
		 * 
		 * My choice of 20ms is arbitrary, if this value is too small the animation calculations could begin to
		 * interfere with the interator processes and make the simulation unresponsive. If it is too large
		 * the animations will be jerky. Play with the value to see what works best.
		 */
		if (std::chrono::duration_cast <std::chrono::milliseconds> (std::chrono::steady_clock::now() - t_last).count() > 20) {

			/* Do things that might need doing ... */
			vtkActorCollection* actorList = renderer->GetActors();
			vtkActor* a;

			/* X Rotation */
			actorList->InitTraversal();
			while ((a = (vtkActor*)actorList->GetNextActor())) {
				a->RotateX(rotateX);
			}

			/* Y Rotation */
			actorList->InitTraversal();
			while ((a = (vtkActor*)actorList->GetNextActor())) {
				a->RotateY(rotateY);
			}

			/* Z Rotation */
			actorList->InitTraversal();
			while ((a = (vtkActor*)actorList->GetNextActor())) {
				a->RotateZ(rotateZ);
			}
			
			/* Remember time now */
			t_last = std::chrono::steady_clock::now();
		}
	}
}



