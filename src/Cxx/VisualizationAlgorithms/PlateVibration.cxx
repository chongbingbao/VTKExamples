// Translated from vib.tcl

#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataReader.h>
#include <vtkProp3D.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkVectorDot.h>
#include <vtkWarpVector.h>

#include <algorithm>
#include <array>
#include <string>

int main(int argc, char* argv[])
{
  auto Scale = [](std::vector<double>& v, double scale) {
    std::transform(std::begin(v), std::end(v), std::begin(v),
                   [=](double const& n) { return n / scale; });
    return;
  };

  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " filename" << std::endl;
    std::cout << "where: filename is the file plate.vtk." << std::endl;
    std::cout
      << "Produces figure 6-14(a) Beam displacement from the VTK Textbook."
      << std::endl;
    return EXIT_FAILURE;
  }

  std::string fileName = argv[1];

  vtkSmartPointer<vtkNamedColors> colors =
    vtkSmartPointer<vtkNamedColors>::New();

  // Set the background color and plate color. Match those in VTKTextbook.pdf.
  auto SetColor = [&colors](std::array<double, 3>& v,
                            std::string const& colorName) {
    auto const scaleFactor = 256.0;
    std::transform(std::begin(v), std::end(v), std::begin(v),
                   [=](double const& n) { return n / scaleFactor; });
    colors->SetColor(colorName, v.data());
    return;
  };
  std::array<double, 3> bkg{65, 99, 149};
  SetColor(bkg, "BkgColor");
  std::array<double, 3> bar{255, 160, 140};
  SetColor(bar, "PlateColor");

  // Read a vtk file
  //
  vtkSmartPointer<vtkPolyDataReader> plate =
    vtkSmartPointer<vtkPolyDataReader>::New();
  plate->SetFileName(fileName.c_str());
  plate->Update();
  double bounds[6];
  plate->GetOutput()->GetBounds(bounds);
  plate->SetVectorsName("mode2");

  vtkSmartPointer<vtkPolyDataNormals> normals =
    vtkSmartPointer<vtkPolyDataNormals>::New();
  normals->SetInputConnection(plate->GetOutputPort());
  vtkSmartPointer<vtkWarpVector> warp = vtkSmartPointer<vtkWarpVector>::New();
  warp->SetInputConnection(normals->GetOutputPort());
  warp->SetScaleFactor(0.5);
  vtkSmartPointer<vtkVectorDot> color = vtkSmartPointer<vtkVectorDot>::New();
  color->SetInputConnection(warp->GetOutputPort());
  vtkSmartPointer<vtkDataSetMapper> plateMapper =
    vtkSmartPointer<vtkDataSetMapper>::New();
  plateMapper->SetInputConnection(warp->GetOutputPort());
  vtkSmartPointer<vtkActor> plateActor = vtkSmartPointer<vtkActor>::New();
  plateActor->SetMapper(plateMapper);
  plateActor->GetProperty()->SetColor(
    colors->GetColor3d("PlateColor").GetData());
  plateActor->RotateX(-90);

  // Create the outline.
  //
  vtkSmartPointer<vtkOutlineFilter> outline =
    vtkSmartPointer<vtkOutlineFilter>::New();
  outline->SetInputConnection(plate->GetOutputPort());
  vtkSmartPointer<vtkPolyDataMapper> spikeMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  spikeMapper->SetInputConnection(outline->GetOutputPort());
  vtkSmartPointer<vtkActor> outlineActor = vtkSmartPointer<vtkActor>::New();
  outlineActor->SetMapper(spikeMapper);
  outlineActor->RotateX(-90);
  outlineActor->GetProperty()->SetColor(colors->GetColor3d("White").GetData());

  // Create the RenderWindow, Renderer and both Actors
  //
  vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renWin =
    vtkSmartPointer<vtkRenderWindow>::New();
  renWin->AddRenderer(ren);

  vtkSmartPointer<vtkRenderWindowInteractor> iren =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  iren->SetRenderWindow(renWin);

  // Add the actors to the renderer, set the background and size
  //
  ren->AddActor(plateActor);
  ren->AddActor(outlineActor);
  renWin->SetSize(500, 500);

  // Render the image.
  renWin->Render();
  ren->SetBackground(colors->GetColor3d("BkgColor").GetData());
  // This closely matches the original illustration.
  ren->GetActiveCamera()->SetPosition(-3.7, 13, 15.5);
  ren->ResetCameraClippingRange();

  iren->Start();

  return EXIT_SUCCESS;
}
