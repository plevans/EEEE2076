/**
 * Basic VTK Cylinder Example with VR output
 * Based on example: https://kitware.github.io/vtk-examples/site/Cxx/GeometricObjects/CylinderExample/
 *
 */



#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCylinderSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

#include <vtkOpenVRRenderWindow.h>				// Change to OpenVR version
#include <vtkOpenVRRenderWindowInteractor.h>	// Change to OpenVR version
#include <vtkOpenVRRenderer.h>					// Change to OpenVR version
#include <vtkOpenVRCamera.h>					// Change to OpenVR version

#include <array>

int main( int, char*[] ) {
  vtkNew<vtkNamedColors> colors;

  // Set the background color.
  std::array<unsigned char, 4> 				bkg{{26, 51, 102, 255}};
  colors->SetColor("BkgColor", bkg.data());


  // This creates a polygonal cylinder model with eight circumferential facets (i.e, in practice an octagonal prism).
  vtkNew<vtkCylinderSource> 				cylinder;
  cylinder->SetResolution( 8 );


  // The mapper is responsible for pushing the geometry into the graphics library. It may also do color mapping, if 
  // scalars or other attributes are defined.
  vtkNew<vtkPolyDataMapper> 				cylinderMapper;
  cylinderMapper->SetInputConnection( cylinder->GetOutputPort() );


  // The actor is a grouping mechanism: besides the geometry (mapper), it also has a property, transformation matrix, 
  // and/or texture map. Here we set its color and rotate it around the X and Y axes.
  vtkNew<vtkActor> 							cylinderActor;
  cylinderActor->SetMapper( cylinderMapper );
  cylinderActor->GetProperty()->SetColor( colors->GetColor4d("Tomato").GetData() );
  cylinderActor->RotateX( 30.0 );
  cylinderActor->RotateY( -45.0 );


  // The renderer generates the image which is then displayed on the render window. It can be thought of as a scene 
  // to which the actor is added
  vtkNew<vtkOpenVRRenderer> 				renderer;							// Change to OpenVR version
  renderer->AddActor( cylinderActor );
  renderer->SetBackground( colors->GetColor3d("BkgColor").GetData() );
  
  
  // Create Open VR Camera
  vtkNew<vtkOpenVRCamera> 					cam;								// This is new
  renderer->SetActiveCamera(cam);												// This is new


  // The render window is the actual GUI window that appears on the computer screen
  vtkNew<vtkOpenVRRenderWindow> 			renderWindow;
  renderWindow->Initialize();
  renderWindow->AddRenderer( renderer );
  renderWindow->SetWindowName( "Cylinder" );


  // The render window interactor captures mouse events and will perform appropriate camera or actor manipulation
  // depending on the nature of the events.
  vtkNew<vtkOpenVRRenderWindowInteractor> 	renderWindowInteractor;				// Change to OpenVR version
  renderWindowInteractor->SetRenderWindow( renderWindow );						// Change to OpenVR version
  renderWindowInteractor->Initialize();


  // This starts the event loop and as a side effect causes an initial render.
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
